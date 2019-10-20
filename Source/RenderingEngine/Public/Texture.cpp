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

#include <iostream>

using namespace std;

// Texture constructor
Texture::Texture(Element *element, int max_frame, float fps_val) : clips_(NULL), curr_clip_(NULL), 
   frame_(0), completed_(false), max_frame_(max_frame), frame_num(max_frame + 1), fps(fps_val), last_frame(0.0f),
   reset_frame(0), stop_frame(max_frame), has_flipped_(false), angle(0.0f), element_(element), 
   image_width(0), image_height(0), texture_width(0), texture_height(0), x(0), y(0) {

    // Reset everything
    texture_ID = 0;
    VBO_ID = 0;
    IBO_ID = 0;
    VAO_ID = 0;

    // Start timer
    fps_timer.start();
}

// Loads textures from files
bool Texture::LoadFromFile(const GLchar *file, GLboolean alpha) {
    // Free preexisting textures
    Free();
    
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(file);

    // Width and Height
    image_width = loadedSurface->w;
    image_height = loadedSurface->h;
    texture_width = loadedSurface->w / frame_num;
    texture_height = loadedSurface->h;
    
    // Check loaded surface
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", file, IMG_GetError());
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
    pixels_32 = loadedSurface->pixels;

    // Set internal format
    internal_format = GL_RGB;
    image_format = GL_RGB;
    if (loadedSurface->format->BytesPerPixel == 4) {
        internal_format = GL_RGBA;
        image_format = GL_RGBA;
    }

    // Generate tex image
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, loadedSurface->w, loadedSurface->h, 0, image_format, GL_UNSIGNED_BYTE, loadedSurface->pixels);

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
    texture_width = 0;
    texture_height = 0;

    //Set pixel format
    pixel_format = 0;
    internal_format = 0;
    image_format = 0;
}

void Texture::init_VAO() {
    // If texture is loaded and VBO does not already exist
    if (VBO_ID == 0) {
        // Vertex data
        // GLTexturedVertex2D vData[ 4 ];
        float vertices[] = {
            // positions  // texture coords
            1.0f,  1.0f,  1.0f, 1.0f, // top right
            1.0f, -1.0f,  1.0f, 0.0f, // bottom right
           -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
           -1.0f,  1.0f,  0.0f, 1.0f  // top left
        };

        // Indices
        GLuint indices[] = {
            0, 1, 3, // First triangle
            1, 2, 3  // Second triangle
        };

        // Generate buffers
        glGenVertexArrays(1, &VAO_ID);
        glGenBuffers( 1, &VBO_ID );
        glGenBuffers( 1, &IBO_ID );

        // Bind vertex array
        glBindVertexArray(VAO_ID);

        // Create VBO
        glBindBuffer( GL_ARRAY_BUFFER, VBO_ID );
        glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

        // Create IBO
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO_ID );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Color attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
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

// Render OpenGL type textures
void Texture::Render(float x, float y, GLFloatRect *clip, GLfloat rotate, glm::vec3 color) {
    if (texture_ID != 0) {
        // Use shader
        ShaderProgram shader = RenderingEngine::get_instance().GetShader("texture_shader").Use();

        // Set top right and bottom textures
        float left, right, bottom, top;

        // Check if clip exists
        if (clip) {
            left = clip->x;
            right = clip->x + clip->w;
            bottom = clip->y;
            top = clip->y + clip->h;
        } else {
            left = 0.0f;
            right = 1.0f;
            bottom = 0.0f;
            top = 1.0f;
        }

        // std::cout << left << " " << right << " " << top << " " << bottom << std::endl;

        float w, h;
        w = (float) texture_width / 2.0f;
        h = (float) texture_height / 2.0f;

        // std::cout << w << " " << h << std::endl;
        // std::cout << texture_ID << std::endl;

        // Initialize vertices 
        float vertices[] = {
            // positions  // texture coords
            w,  h,        right, top, // top right
            w, -h,        right, bottom, // bottom right
           -w, -h,        left, bottom, // bottom left
           -w,  h,        left, top  // top left
        };

        // Transform it
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + texture_width / 2, y + texture_height / 2, 0.0f));
        // model = glm::scale(model, glm::vec3(1, 1, 1.0f));

        // Set transforms and color
        shader.SetMatrix4("model", model);
        shader.SetVector3f("color", color);
        
        // Activate texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_ID);
        // if (clip) glTexSubImage2D(GL_TEXTURE_2D, 0, clip->x, clip->y, clip->w, clip->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels_32);
        // else glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_width, texture_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels_32);

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

void Texture::Render(float x, float y, GLfloat rotate, Animation *clip, glm::vec3 color) {
    if (texture_ID != 0) {
        // Use shader
        ShaderProgram shader = RenderingEngine::get_instance().GetShader("texture_shader").Use();

        // If clip doesn't exist, render entire texture
        float h_w, h_h;
        float l, r, b, t;
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
        float vertices[] = {
            // positions  // texture coords
            h_w,  h_h,    r, t, // top right
            h_w, -h_h,    r, b, // bottom right
           -h_w, -h_h,    l, b, // bottom left
           -h_w,  h_h,    l, t  // top left
        };

        // Transform it
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + texture_width / 2, y + texture_height / 2, 0.0f));

        // Set transforms and color
        shader.SetMatrix4("model", model);
        shader.SetVector3f("color", color);
        
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

    // Set next frame based on fps
    anim->last_frame += fps_timer.getDeltaTime() / 1000.0f;
    if (anim->last_frame > anim->fps) {
        if (anim->curr_frame == temp_max) {
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

// Texture &Texture::operator=(const Texture &src) {
//    // Set dimensions equal to image dimension
//    image_width = src.image_width;
//    image_height = src.image_height;

//    // Add a bunch of other missing variables
//    max_frame_ = src.max_frame_;
//    clips_ = new GLFloatRect[max_frame_ + 1];
//    for (int i = 0; i < max_frame_ + 1; i++) {
//       clips_[i] = src.clips_[i];
//    }
   
//    // Return
//    return (*this);
// }

Animation *Texture::GetAnimationFromTexture(std::string name) {
   return animations[name];
}

// Destructor calls Free
Texture::~Texture() {
   //Free();
}

TextureData::TextureData(int num_of_frames, float fps, std::string name, std::string path) :
    num_of_frames(num_of_frames), fps(fps), name(name), path(path) {}

TextureData::TextureData(int width, int height, int num_of_frames) :
    width(width), height(height), num_of_frames(num_of_frames) {}

Animation::Animation(GLfloat image_width, GLfloat image_height, GLfloat texture_width, GLfloat texture_height, GLfloat offset, int num_of_frames, float fps) :
    image_width(image_width), image_height(image_height),
    texture_width(texture_width), texture_height(texture_height),
    half_width(texture_width / 2.0f), half_height(texture_height / 2.0f),
    num_of_frames(num_of_frames), curr_frame(0), max_frame(num_of_frames - 1),
    last_frame(0.0f), fps(fps), completed(false) {

    // Set normal width and height
    for (int i = 0; i < num_of_frames; i++) {
        // Create new frame
        frames.push_back(
            {
                texture_width / image_width, 
                texture_height / image_height,
                0.0f,
                0.0f,
                offset,
                offset + (texture_height / image_height)
            }
        );
    }
}