//
//  Texture.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/14/17.
//  Copyright © 2017 The Boys. All rights reserved.

#include "Source/RenderingEngine/Private/Texture.h"
#include "Source/RenderingEngine/Private/BasicShaderProgram.h"
#include "Source/RenderingEngine/Private/RenderingEngine.h"

#include "Source/ObjectManager/Private/Global.h"
#include "Source/ObjectManager/Private/Element.h"

#include "Source/GameEngine/Private/Application.h"

#include "SOIL.h"
#include <iostream>
#include <math.h>
#include <glm/gtx/string_cast.hpp>

using namespace std;

// Texture constructor
Texture::Texture(Element *element, int max_frame, float fps_val) : clips_(NULL), curr_clip_(NULL), 
   frame_(0), completed_(false), max_frame_(max_frame), frame_num(max_frame + 1), fps(fps_val), last_frame(0.0f),
   reset_frame(0), stop_frame(max_frame), has_flipped_(false), angle(0.0f), element_(element), 
   image_width(0.0f), image_height(0.0f), x(0), y(0) {

    // Reset everything
    texture_ID = 0;
    VBO_ID = 0;
    IBO_ID = 0;
    VAO_ID = 0;

    // Set formats
    internal_format = GL_RGBA;
    image_format = GL_RGBA;

    // Start timer
    fps_timer.start();
}

// Loads textures from filesSDL_image Error: %s\n
bool Texture::LoadFromFile(const GLchar *file, GLboolean alpha) {
   // Free preexisting textures
   Free();
   
   // Load image at specified path
   // SDL_Surface* loadedSurface = IMG_Load(file);

   internal_format = GL_RGBA;
   image_format = GL_RGBA;
   int width, height;
   unsigned char *image = SOIL_load_image(file, &width, &height, 0, image_format == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);

   // Set width and height
   image_width = width;
   image_height = height;

   // Check loaded surface
   if (!image) {
      printf("Unable to load image %s!\n", file);
      return false;
   }

   // Init VAO
   init_VAO();

   // Generate the texture
   glGenTextures(1, &texture_ID);
   glBindTexture(GL_TEXTURE_2D, texture_ID);

   // Set parameters
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

   // Store pixels
   pixels_32 = image;

   // Generate tex image
   glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, GL_UNSIGNED_BYTE, image);

   // Unbind texture
   glBindTexture(GL_TEXTURE_2D, 0);
   
   return true;
}

// Free textures
void Texture::Free() {
    // Free texture if it exists
    if (texture_ID != 0) {
        glDeleteTextures( 1, &texture_ID );
        texture_ID = 0;
        VBO_ID = 0;
        IBO_ID = 0;
        VAO_ID = 0;
    }

    // Change EVERYTHING
    image_width = 0;
    image_height = 0;

    //Set pixel format
    internal_format = 0;
    image_format = 0;
}

void Texture::init_VAO() {
    // If texture is loaded and VBO does not already exist
    if (VBO_ID == 0) {
        // Vertex data
        // GLTexturedVertex2D vData[ 4 ];
        double vertices[] = {
            // positions  // texture coords
            1.0f,  1.0f,  1.0f, 1.0f, // top right
            1.0f, -1.0f,  1.0f, 0.0f, // bottom right
           -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
           -1.0f,  1.0f,  0.0f, 1.0f  // top left
        };

        // Indices
        GLuint indices[] = {
            3, 1, 2, // First triangle
            3, 0, 1  // Second triangle
        };

        // Generate buffers
        glGenVertexArrays(1, &VAO_ID);
        glGenBuffers( 1, &VBO_ID );
        glGenBuffers( 1, &IBO_ID );

        // Bind vertex array
        glBindVertexArray(VAO_ID);

        // Create VBO
        glBindBuffer( GL_ARRAY_BUFFER, VBO_ID );
        glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW );

        // Create IBO
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO_ID );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW );

        // Position attribute
        glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Color attribute
        glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), (void*)(2 * sizeof(double)));
        glEnableVertexAttribArray(1);

        // Disable
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void Texture::Free_VBO() {
    // Free VBO and IBO
    if (VBO_ID != 0) {
        glDeleteBuffers( 1, &VBO_ID );
        glDeleteBuffers( 1, &IBO_ID );
    }
}

void Texture::Render(float x, float y, GLfloat rotate, Animation *clip,  bool render_from_center, glm::vec3 color) {
   // std::cout << texture_ID << std::endl;
   if (texture_ID != 0) {
      // Use shader
      ShaderProgram *shader = RenderingEngine::GetInstance().GetShaderReference("texture_shader")->Use();

      // If clip doesn't exist, render entire texture
      GLdouble h_w, h_h;
      GLdouble l, r, b, t;
      if (!clip) {
         h_w = image_width / 2.0f;
         h_h = image_height / 2.0f;
         l = 0.0f;
         r = 1.0f;
         b = 0.0f;
         t = 1.0f;
      } else {
         h_w = clip->half_width;
         h_h = clip->half_height;
         l = clip->frames[clip->curr_frame].l;
         r = clip->frames[clip->curr_frame].r;
         b = clip->frames[clip->curr_frame].b;
         t = clip->frames[clip->curr_frame].t;
      }

      // Initialize vertices 
      double vertices[] = {
         // positions  // texture coords
         -h_w, -h_h,   l, b, // bottom left
          h_w, -h_h,   r, b, // bottom right
          h_w,  h_h,   r, t, // bottom left
         -h_w,  h_h,   l, t  // top left
      };

      // Transform it
      glm::mat4 model = glm::mat4(1.0f);
      if (clip) {
         float final_x = render_from_center ? x : x + clip->half_width;
         float final_y = render_from_center ? y : y + clip->half_height;
         model = glm::translate(model, glm::vec3(final_x, final_y, 0.0f));
         rotate = rotate * (M_PI / 180.0f);
         model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
      } else {
         model = glm::translate(model, glm::vec3(x + image_width / 2.0, y + image_height / 2.0, 0.0));
      }

      // Set transforms and color
      shader->SetMatrix4("model", model);
      shader->SetVector3f("color", color);

      // Activate texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_ID);

      // Draw
      glBindVertexArray(VAO_ID);
      glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      
      // Unbind everything
      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
   }
}

void Texture::Render(glm::mat4 m, GLfloat rotate, Animation *clip, glm::vec3 color) {
   // std::cout << texture_ID << std::endl;
   if (texture_ID != 0) {
      // Use shader
      ShaderProgram *shader = RenderingEngine::GetInstance().GetShaderReference("texture_shader")->Use();

      // If clip doesn't exist, render entire texture
      GLdouble h_w, h_h;
      GLdouble l, r, b, t;
      if (!clip) {
         h_w = image_width / 2.0f;
         h_h = image_height / 2.0f;
         l = 0.0f;
         r = 1.0f;
         b = 0.0f;
         t = 1.0f;
      } else {
         h_w = clip->half_width;
         h_h = clip->half_height;
         l = clip->frames[clip->curr_frame].l;
         r = clip->frames[clip->curr_frame].r;
         b = clip->frames[clip->curr_frame].b;
         t = clip->frames[clip->curr_frame].t;
      }

      // Initialize vertices 
      double vertices[] = {
         // positions  // texture coords
         -h_w, -h_h,   l, b, // bottom left
          h_w, -h_h,   r, b, // bottom right
          h_w,  h_h,   r, t, // bottom left
         -h_w,  h_h,   l, t  // top left
      };

      // Transform it
      glm::mat4 model = m;
      rotate = rotate * (M_PI / 180.0f);
      model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));

      // Set transforms and color
      shader->SetMatrix4("model", model);
      shader->SetVector3f("color", color);

      // Activate texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_ID);

      // Draw
      glBindVertexArray(VAO_ID);
      glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      
      // Unbind everything
      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
   }
}

void Texture::Animate(Animation *anim, int reset, int max, int start) {
   // Define standards
   int temp_max = (max == 0) ? anim->max_frame : max;
   int temp_start = (start == 0) ? 0 : start;

   // Method for going back to first frame...
   if (max == -1) temp_max = 0;
   if (start == -1) temp_start = 0;

   // Set next frame based on fps
   anim->last_frame += anim->fps_timer.getDeltaTime() / 1000.0f;
   if (anim->last_frame > anim->fps) {
      if (anim->curr_frame >= temp_max) {
         anim->curr_frame = reset;
         anim->completed = true;
         anim->last_frame = 0.0f;
         return;
      } else if (anim->curr_frame <= reset + 1) {
         anim->completed = false;
      }
      ++anim->curr_frame;
      anim->last_frame = 0.0f;
   }
}

// Get x position
int Texture::get_x() const {
    return x;
}

// Get y position
int Texture::get_y() const {
    return y;
}

// Destructor calls Free
Texture::~Texture() {
   //Free();
}

Animation::Animation(Texture *texture, std::string name, GLdouble texture_width, GLdouble texture_height, GLdouble offset, int num_of_frames, float fps) :
    parent(texture), name(name),
    texture_width(texture_width), texture_height(texture_height),
    half_width(texture_width / 2.0), half_height(texture_height / 2.0),
    num_of_frames(num_of_frames), curr_frame(0), max_frame(num_of_frames - 1),
    reset_frame(0), stop_frame(0),
    last_frame(0.0f), fps(fps), completed(false), flipped(false) {

    // Set normal width and height
    for (int i = 0; i < num_of_frames; i++) {
        // Create new frame
        frames.push_back(
            {
                texture_width / parent->image_width, 
                texture_height / parent->image_height,
                0.0f,
                0.0f,
                (offset / parent->image_height),
                (offset / parent->image_height) + (texture_height / parent->image_height)
            }
        );
    }

    // Start fps timer
    fps_timer.start();
}

void Animation::flipAnimation() {
   // Iterate through frames vector to flip l and r
   for (std::vector<FrameData>::iterator it = frames.begin(); it != frames.end(); ++it) {
      std::swap(it->l, it->r);
   }

   // Set flipped to true
   flipped = true;
}