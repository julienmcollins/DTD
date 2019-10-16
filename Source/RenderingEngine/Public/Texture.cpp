//
//  Texture.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/14/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

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
   frame_(0), completed_(false), max_frame_(max_frame), fps(fps_val), last_frame(0.0f),
   reset_frame(0), stop_frame(max_frame), flip_(SDL_FLIP_NONE), has_flipped_(false), angle(0.0f), element_(element), 
   m_texture(NULL), m_width(0), m_height(0), x(0), y(0) {

    // Set rect
    tex_dimensions = {1.f, 0.f, 0.f, 1.f};

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
    m_width = loadedSurface->w;
    m_height = loadedSurface->h;
    center_ = {m_width / 2, m_height / 2};
    
    // Check loaded surface
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", file, IMG_GetError());
        return false;
    } else {
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
        
        // Set format
        pixel_format = GL_RGB;
        if (loadedSurface->format->BytesPerPixel == 4) {
            pixel_format = GL_RGBA;
        }

        // Generate tex image
        glTexImage2D(GL_TEXTURE_2D, 0, pixel_format, loadedSurface->w, loadedSurface->h, 0, pixel_format, GL_UNSIGNED_BYTE, loadedSurface->pixels);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Check error
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            cout << "Error loading texture from " << loadedSurface->pixels << " pixels! " << gluErrorString(error) << endl;
            return false;
        }
    }
    
    return true;
}

// Free textures
void Texture::Free() {
    // Free texture if it exists
    if (texture_ID != 0) {
        glDeleteTextures( 1, &texture_ID );
        texture_ID = 0;
    }

    // Change EVERYTHING
    image_width = 0;
    image_height = 0;
    texture_width = 0;
    texture_height = 0;

    //Set pixel format
    pixel_format = 0;
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
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glBindVertexArray(0);
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
void Texture::Render(float x, float y, GLFloatRect *clip) {
    if (texture_ID != 0) {
        // float quad_width = (float) m_width / Application::get_instance().get_width();
        // float quad_height = (float) m_height / Application::get_instance().get_height();

        // if (clip != NULL) {
        //     tex_dimensions.l = clip->x / m_width;
        //     tex_dimensions.r = (clip->x + clip->w) / m_width;
        //     tex_dimensions.t = clip->y / m_height;
        //     tex_dimensions.b = (clip->y + clip->h) / m_height;

        //     quad_width = clip->w;
        //     quad_height = clip->h;
        // }

        // // Move to Render point
        // // BasicShaderProgram::get_instance().left_mult_modelview(glm::translate(glm::vec3(x, y, 0.f)));
        // // BasicShaderProgram::get_instance().update_modelview_matrix();

        // // Set vertex data
        // GLTexturedVertex2D v_data[4];
    
        // // Vertex coords
        // v_data[0].position.x = quad_width / 2.f; v_data[0].position.y = quad_height / 2.f; // Top right
        // v_data[0].position.x = quad_width / 2.f; v_data[0].position.y = -quad_height / 2.f; // Bottom right
        // v_data[0].position.x = -quad_width / 2.f; v_data[0].position.y = -quad_height / 2.f; // Bottom left
        // v_data[0].position.x = -quad_width / 2.f; v_data[0].position.y = quad_height / 2.f; // Top left

        // // Tex coords
        // v_data[0].tex_coord.s = tex_dimensions.r; v_data[0].tex_coord.t = tex_dimensions.t;
        // v_data[1].tex_coord.s = tex_dimensions.r; v_data[1].tex_coord.t = tex_dimensions.b;
        // v_data[2].tex_coord.s = tex_dimensions.l; v_data[2].tex_coord.t = tex_dimensions.b;
        // v_data[3].tex_coord.s = tex_dimensions.l; v_data[3].tex_coord.t = tex_dimensions.t;

        // // Set texture ID
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture_ID);

        // // Enable vertex and tex coordinate arrays
        // // BasicShaderProgram::get_instance().bind();
        // glBindVertexArray(VAO_ID);

        // // Update texture
        // //glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(GLTexturedVertex2D), v_data);

        // // Draw it
        // glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

        // Use shader
        ShaderProgram shader = RenderingEngine::get_instance().GetShader("texture_shader").Use();
        
        // Transform it
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(960, 540, 0.0f));
        model = glm::scale(model, glm::vec3(1920.0f, 1080.0f, 1.0f));

        // Set transforms and color
        shader.SetMatrix4("model", model);
        shader.SetVector3f("color", glm::vec3(1.0f, 0.8f, 0.2f));
        
        // Activate texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_ID);

        // Draw
        glBindVertexArray(VAO_ID);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

// Gets image width
int Texture::getWidth() const {
    return m_width;
}

// Gets image height
int Texture::getHeight() const {
    return m_height;
}

// Get x position
int Texture::get_x() const {
    return x;
}

// Get y position
int Texture::get_y() const {
    return y;
}

Texture &Texture::operator=(const Texture &src) {
   // Set Render equal to it
   // renderer = src.renderer;
   
   // Set textures equal to eachother
   m_texture = src.m_texture;
   
   // Set dimensions equal to image dimension
   m_width = src.m_width;
   m_height = src.m_height;

   // Add a bunch of other missing variables
   max_frame_ = src.max_frame_;
   clips_ = new GLFloatRect[max_frame_ + 1];
   for (int i = 0; i < max_frame_ + 1; i++) {
      clips_[i] = src.clips_[i];
   }
   
   // Return
   return (*this);
}

// Destructor calls Free
Texture::~Texture() {
   //Free();
}

TextureData::TextureData(int width, int height, int num_of_frames, int frame_width, int frame_height, std::string path) :
    width(width), height(height), frame_width(frame_width), frame_height(frame_height), num_of_frames(num_of_frames), path(path) {}