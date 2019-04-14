#ifndef Level_h
#define Level_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <Box2D/Box2D.h>

#include <string>
#include <vector>

#include "Element.h"
#include "Texture.h"
#include "Entity.h"
#include "Object.h"

/***** LEVEL CLASS *******/
class Level {
   public:
      // Level constructor takes in a string to the template file
      Level(std::string file, Application *application);

      // Update function for the level, which will render all of its assets
      void update();

   private:
      // Number of enemies and platforms
      int num_of_enemies_;
      int num_of_platforms_;

      // Background texture
      Texture background_texture_;

      // Platform texture
      Texture platform_texture_;

      // Vector for the enemies
      std::vector<Enemy *> enemies_;

      // Vector for the platforms
      std::vector<Platform *> platforms_;

      // Dir path
      std::string dir_;

      // Load media
      bool load_media_(string dir);

      // Application holder
      Application *application_;
};
