//
//  Texture.h
//  Janitor: The Story
//
//  Created by Julien Collins on 9/14/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#ifndef Texture_h
#define Texture_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "OpenGLIncludes.h"
#include "Source/RenderingEngine/Private/GLData.h"
#include "Source/ObjectManager/Private/Timer.h"

#include <string>
#include <vector>

class Element;
class Animation;

class Texture {
   public:
      // Initialize
      Texture(Element *element = NULL, int max_frame = 0, float fps_val = 0);
      
      // Load image at specified path
      bool LoadFromFile(const GLchar *file, GLboolean alpha);
      
      // Deallocate texture
      void Free();

      /****** OPENGL *********/
      // Overloaded opengl Render function
      void Render(float x, float y, GLFloatRect *clip = NULL, GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
      void Render(float x, float y, GLfloat rotate = 0.0f, Animation *clip = NULL, glm::vec3 color = glm::vec3(1.0f));
      
      // Animate function
      void Animate(Animation *anim, int reset = 0, int max = 0, int start = 0);
      /***********************/

      // Rect for sprite
      GLFloatRect *clips_;
      GLFloatRect *curr_clip_;

      // Frame related variables
      int frame_;
      int max_frame_;
      int frame_num;
      float fps;
      float last_frame;
      Timer fps_timer;

      // Start and stop frames
      int reset_frame;
      int stop_frame;

      // Flag for completed animation
      bool completed_;

      // Flip parameter
      bool has_flipped_;

      // Angle
      float angle;

      // Get image dimensions
      int GetImageWidth() const {
         return image_width;
      }
      int GetImageHeight() const {
         return image_height;
      }

      // Get texture dimensions
      int GetTextureWidth() const {
         return texture_width;
      }
      int GetTextureHeight() const {
         return texture_height;
      }

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
      // Texture &operator= (const Texture &src);

      // Destructor
      ~Texture();

   // protected:
      // Initialize and Free vertex buffer object
      void init_VAO();
      void Free_VBO();

      // Texture ID
      unsigned int texture_ID;

      // Pixels related to the texture
      void *pixels_32;

      // Pixel format
      unsigned int pixel_format;
      unsigned int internal_format;
      unsigned int image_format;

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
         
   private:
      // Image position
      int x;
      int y;

};

class TextureData {
   public:
      // Constructors
      TextureData(int num_of_frames, float fps, std::string name, std::string path);
      TextureData(int width, int height, int num_of_frames);

      // Data associated to texture
      int width;
      int height;
      int frame_width;
      int frame_height;
      int num_of_frames;
      float fps;
      std::string name;
      std::string path;
};

/* Holds relevant vertex data for rendering */
struct FrameData {
   /* Texture width and height */
   GLfloat normal_width;
   GLfloat normal_height;

   /* Texture coordinates */
   GLfloat l;
   GLfloat r;
   GLfloat b;
   GLfloat t;
};

/* Animations data */
class Animation {
   public:
      /* Default constructor does not do anything (for now) */
      Animation(GLfloat image_width, GLfloat image_height, GLfloat texture_width, GLfloat texture_height, GLfloat offset, int num_of_frames, float fps);

      /* Image width */
      GLfloat image_width;
      GLfloat image_height;

      /* Texture width */
      GLfloat texture_width;
      GLfloat texture_height;

      /* Half width and height for vertext */
      GLfloat half_width;
      GLfloat half_height;

      /* FRAME INFORMATION */
      int num_of_frames;
      int curr_frame;
      int max_frame;
      float last_frame;
      float fps;
      bool completed;

      /* Animation data for the frame */
      std::vector<FrameData> frames;
};

#endif /* Texture_h */
