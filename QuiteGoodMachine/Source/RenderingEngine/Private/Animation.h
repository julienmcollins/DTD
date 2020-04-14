#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

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
      FPSTimer fps_timer;

      /* Animation data for the frame */
      std::vector<FrameData> frames;
};

#endif