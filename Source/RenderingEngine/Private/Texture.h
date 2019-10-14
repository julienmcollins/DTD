//
//  Texture.h
//  Janitor: The Story
//
//  Created by Julien Collins on 9/14/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#ifndef Texture_h
#define Texture_h

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "OpenGLIncludes.h"
#include "GLData.h"
#include "Timer.h"

class Element;

class Texture {
   public:
      // Initialize
      Texture(Element *element = NULL, int max_frame = 0, float fps_val = 0);

      // Copy constructor
      //Texture(const Texture &tex);
      
      // Load image at specified path
      bool loadFromFile(std::string path);
      
      // Deallocate texture
      void free();
      
      // Render texture at given point
      void render(int x, int y, GLFloatRect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

      /****** OPENGL *********/
      // Overloaded opengl render function
      void render(float x, float y, GLFloatRect *clip = NULL);
      /***********************/

      // Rect for sprite
      GLFloatRect *clips_;
      GLFloatRect *curr_clip_;

      // Frame related variables
      int frame_;
      int max_frame_;
      float fps;
      float last_frame;
      Timer fps_timer;

      // Start and stop frames
      int reset_frame;
      int stop_frame;

      // Flag for completed animation
      bool completed_;

      // Flip parameter
      SDL_RendererFlip flip_;
      bool has_flipped_;

      // Center parameter
      SDL_Point center_;

      // Angle
      float angle;

      // Get image dimensions
      int getWidth() const;
      int getHeight() const;

      // Set image positions
      void set_x(int new_x) {
         x = new_x;
      }
      void set_y(int new_y) {
         y = new_y;
      }

      // Add to image positions
      void add_x(int new_x) {
         x += new_x;
      }
      void add_y(int new_y) {
         y += new_y;
      }

      // Get image positions
      int get_x() const;
      int get_y() const;

      // Element referring to the texture
      Element *element_;
   
      // Assignement operator
      Texture &operator= (const Texture &src);

      // Destructor
      ~Texture();
   
   private:
      // Hardware texture
      SDL_Texture* m_texture;
      
      // Image dimensions
      int m_width;
      int m_height;

      // Image position
      int x;
      int y;

   protected:
      // Initialize and free vertex buffer object
      void init_VAO();
      void free_VBO();

      // Texture ID
      unsigned int texture_ID;

      // Pixels related to the texture
      unsigned int *pixels_32;

      // Pixel format
      unsigned int pixel_format;

      // Texture width and height
      unsigned int texture_width;
      unsigned int texture_height;

      // Image width and height
      unsigned int image_width;
      unsigned int image_height;

      // Tex rect
      GLTexRect tex_dimensions;

      // Vertex and indices
      unsigned int VBO_ID;
      unsigned int IBO_ID;

      // VAO
      unsigned int VAO_ID;
};

class TextureData {
   public:
      // Constructor
      TextureData(int width, int height, int num_of_frames, int frame_width = 0, int frame_height = 0, std::string path = "");

      // Data associated to texture
      int width;
      int height;
      int frame_width;
      int frame_height;
      int num_of_frames;
      std::string path;
};

#endif /* Texture_h */
