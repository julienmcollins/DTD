#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

Animation::Animation(Texture *texture, std::string name, GLdouble texture_width, GLdouble texture_height, GLdouble offset, int num_of_frames, float fps, int rows, int columns) :
   parent(texture), name(name),
   texture_width(texture_width), texture_height(texture_height),
   half_width(texture_width / 2.0), half_height(texture_height / 2.0),
   num_of_frames(num_of_frames), rows(rows), columns(columns), curr_frame(0), max_frame(num_of_frames - 1),
   reset_frame(0), stop_frame(0),
   last_frame(0.0f), fps(fps), completed(false), flipped(false) {

   // Set columns if bigger than 1
   if (rows > 0) {
      columns = num_of_frames / rows;
   } else {
      columns = 0;
   }

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
   fps_timer.Start();
}

void Animation::flipAnimation() {
   // Iterate through frames vector to flip l and r
   for (std::vector<FrameData>::iterator it = frames.begin(); it != frames.end(); ++it) {
      std::swap(it->l, it->r);
   }

   // Set flipped to true
   flipped = true;
}