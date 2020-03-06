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
#include "QuiteGoodMachine/Source/RenderingEngine/Private/GLData.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Timer.h"

#include <string>
#include <vector>
#include <unordered_map>

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
      void Render(float x, float y, GLfloat rotate = 0.0f, Animation *clip = NULL, bool render_from_center = false, glm::vec3 color = glm::vec3(1.0f));
      void Render(glm::mat4 m, GLfloat rotate = 0.0f, Animation *clip = NULL, glm::vec3 color = glm::vec3(1.0f));
      
      // Animate function
      void Animate(Animation *anim, int reset = 0, int max = 0, int start = 0);

      // Variables for texture width and height
      GLdouble image_width;
      GLdouble image_height;
      /***********************/

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
   
      // Destructor
      ~Texture();

   // protected:
      // Initialize and Free vertex buffer object
      void init_VAO();

      // Texture ID
      unsigned int texture_ID;

      // Pixels related to the texture
      unsigned char *image = nullptr;

      // Pixel format
      unsigned int internal_format;
      unsigned int image_format;

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

/* Holds relevant vertex data for rendering */
struct FrameData {
   /* Texture width and height */
   GLdouble normal_width;
   GLdouble normal_height;

   /* Texture coordinates */
   GLdouble l;
   GLdouble r;
   GLdouble b;
   GLdouble t;
};

/* Animations data */
class Animation {
   public:
      /* Default constructor does not do anything (for now) */
      Animation(Texture *texture, std::string name, GLdouble texture_width, GLdouble texture_height, GLdouble offset, int num_of_frames, float fps, int rows = 0, int columns = 0);

      /* Function to flip animation */
      void flipAnimation();

      /* Texture reference */
      Texture *parent;

      /* Animation name */
      std::string name;

      /* Texture width */
      GLdouble texture_width;
      GLdouble texture_height;

      /* Half width and height for vertext */
      GLdouble half_width;
      GLdouble half_height;

      /* FRAME INFORMATION */
      int num_of_frames;
      int rows;
      int columns;
      int curr_frame;
      int max_frame;
      int reset_frame;
      int stop_frame;
      float last_frame;
      float fps;
      bool completed;
      bool flipped;

      /* FPS TIMER */
      Timer fps_timer;

      /* Animation data for the frame */
      std::vector<FrameData> frames;
};

#endif /* Texture_h */
