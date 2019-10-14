 //
//  Texture.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/14/17.
//  Copyright © 2017 The Boys. All rights reserved.
//
#include "Source/Private/Texture.h"
#include "Source/Private/Global.h"
#include "Source/Private/Element.h"
#include "Source/Private/BasicShaderProgram.h"

#include <iostream>

using namespace std;

// Texture constructor
Texture::Texture(Element *element, int max_frame, float fps_val) : clips_(NULL), curr_clip_(NULL), 
   frame_(0), completed_(false), max_frame_(max_frame), fps(fps_val), last_frame(0.0f),
   reset_frame(0), stop_frame(max_frame), flip_(SDL_FLIP_NONE), has_flipped_(false), angle(0.0f), element_(element), 
   m_texture(NULL), m_width(0), m_height(0), x(0), y(0) {

    // Set rect
    tex_dimensions = {0.f, 1.f, 0.f, 1.f};

    // Start timer
    fps_timer.start();
}

// Loads textures from files
bool Texture::loadFromFile(std::string path) {
    // Free preexisting textures
    free();
    
    // The final texture
    SDL_Texture* newTexture = NULL;
    
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());

    // Width and Height
    m_width = loadedSurface->w;
    m_height = loadedSurface->h;
    center_ = {m_width / 2, m_height / 2};
    
    // Check loaded surface
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    } else {
        /******* OPENGL SETUP **********/
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
        glBindTexture(GL_TEXTURE_2D, NULL);

        // Check error
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            cout << "Error loading texture from " << loadedSurface->pixels << " pixels! " << gluErrorString(error) << endl;
            return false;
        }

        /*******************************/

        // // Color key image
        // SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        
        // // Create texture from surface pixels
        // newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        // if (newTexture == NULL) {
        //     printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        // } else {
        //     // Get image dimensions
        //     m_width = loadedSurface->w;
        //     m_height = loadedSurface->h;
        //     center_ = {m_width / 2, m_height / 2};
        // }
        
        // // Get rid of old loaded surface
        // SDL_FreeSurface(loadedSurface);
    }
    
    // Return success
    // m_texture = newTexture;
    // return m_texture != NULL;
    return true;
}

// Free textures
void Texture::free() {
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
    pixel_format = NULL;
}

void Texture::init_VAO() {
    // If texture is loaded and VBO does not already exist
    if (VBO_ID == 0) {
        cout << "Texture::init_VAO() - INITIALIZING VAO\n";

        // Vertex data
        GLTexturedVertex2D vData[ 4 ];
        unsigned int iData[ 4 ];

        //Set rendering indices
        iData[ 0 ] = 0;
        iData[ 1 ] = 1;
        iData[ 2 ] = 2;
        iData[ 3 ] = 3;

        // Generate buffers
        glGenVertexArrays(1, &VAO_ID);
        glGenBuffers( 1, &VBO_ID );
        glGenBuffers( 1, &IBO_ID );

        // Bind vertex array
        glBindVertexArray(VAO_ID);

        // Create VBO
        glBindBuffer( GL_ARRAY_BUFFER, VBO_ID );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(GLTexturedVertex2D), vData, GL_STATIC_DRAW );

        // Create IBO
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO_ID );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), iData, GL_STATIC_DRAW );

        // Set attributes
        BasicShaderProgram::get_instance().set_vertex_pointer(sizeof(GLTexturedVertex2D), (GLvoid *) offsetof(GLTexturedVertex2D, tex_coord));
        BasicShaderProgram::get_instance().set_tex_pointer(sizeof(GLTexturedVertex2D), (GLvoid *) offsetof(GLTexturedVertex2D, position));

        // Unbind buffers
        // glBindBuffer( GL_ARRAY_BUFFER, NULL );
        // glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL );

        // Generate, bind and enable vertex array objects
        //BasicShaderProgram::get_instance().enable_data_pointers();

        // Set vertex data
        // glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
        // BasicShaderProgram::get_instance().set_vertex_pointer(sizeof(GLTexturedVertex2D), (GLvoid *) offsetof(GLTexturedVertex2D, tex_coord));
        // BasicShaderProgram::get_instance().set_tex_pointer(sizeof(GLTexturedVertex2D), (GLvoid *) offsetof(GLTexturedVertex2D, position));

        // Unbind
        // glBindVertexArray(NULL);
    }
}

void Texture::free_VBO() {
    // Free VBO and IBO
    if (VBO_ID != 0) {
        glDeleteBuffers( 1, &VBO_ID );
        glDeleteBuffers( 1, &IBO_ID );
    }
}

// Render textures
void Texture::render(int x, int y, GLFloatRect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    // Set rendering space and render to screen
    GLFloatRect renderQuad = {x, y, m_width, m_height};
    
    // Set clip rendering dimensions
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    
    // Render to screen
    //SDL_RenderCopyEx(renderer, m_texture, clip, &renderQuad, angle, center, flip);

    // Set x and y position
    this->x = x;
    this->y = y;
}

// Render OpenGL type textures
void Texture::render(float x, float y, GLFloatRect *clip) {
    if (texture_ID != 0) {
        float quad_width = m_width;
        float quad_height = m_height;

        if (clip != NULL) {
            tex_dimensions.l = clip->x / m_width;
            tex_dimensions.r = (clip->x + clip->w) / m_width;
            tex_dimensions.t = clip->y / m_height;
            tex_dimensions.b = (clip->y + clip->h) / m_height;

            quad_width = clip->w;
            quad_height = clip->h;
        }

        // Move to render point
        BasicShaderProgram::get_instance().left_mult_modelview(glm::translate(glm::vec3(x, y, 0.f)));
        BasicShaderProgram::get_instance().update_modelview_matrix();

        // Set vertex data
        GLTexturedVertex2D v_data[4];

        // Tex coords
        v_data[0].tex_coord.s = tex_dimensions.l; v_data[0].tex_coord.t = tex_dimensions.t;
        v_data[1].tex_coord.s = tex_dimensions.r; v_data[1].tex_coord.t = tex_dimensions.t;
        v_data[2].tex_coord.s = tex_dimensions.r; v_data[2].tex_coord.t = tex_dimensions.b;
        v_data[3].tex_coord.s = tex_dimensions.l; v_data[3].tex_coord.t = tex_dimensions.b;
    
        // Vertex coords
        v_data[0].position.x =        0.f; v_data[0].position.y =         0.f;
        v_data[0].position.x = quad_width; v_data[0].position.y =         0.f;
        v_data[0].position.x = quad_width; v_data[0].position.y = quad_height;
        v_data[0].position.x =        0.f; v_data[0].position.y = quad_height;

        // Set texture ID
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_ID);

        // Enable vertex and tex coordinate arrays
        BasicShaderProgram::get_instance().bind();
        glBindVertexArray(VAO_ID);

        // Update texture
        //glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(GLTexturedVertex2D), v_data);

        // Draw it
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);
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
   // Set render equal to it
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

// Destructor calls free
Texture::~Texture() {
   //free();
}

TextureData::TextureData(int width, int height, int num_of_frames, int frame_width, int frame_height, std::string path) :
    width(width), height(height), frame_width(frame_width), frame_height(frame_height), num_of_frames(num_of_frames), path(path) {}