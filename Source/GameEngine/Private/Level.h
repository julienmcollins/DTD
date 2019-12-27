#ifndef Level_h
#define Level_h

#include "Source/ObjectManager/Private/Element.h"
#include "Source/ObjectManager/Private/Entity.h"
#include "Source/ObjectManager/Private/Object.h"

#include "Source/RenderingEngine/Private/Texture.h"

#include "Source/GameEngine/Private/Application.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <Box2D/Box2D.h>

#include <string>
#include <vector>
#include <unordered_map>

// Prototype
class Enemy;

/***** LEVEL CLASS *******/
class Level {
   public:
      // Level constructor takes in a string to the template file
      Level(std::string file, Application::FOREST level);

      // Update function for the level, which will Render all of its assets
      void Update();

      // Add an enemy to the level
      void add_enemy(Enemy *new_enemy);

      // Destroy enemy
      void destroy_enemy(Enemy *enemy_to_delete);

      // Flag for level completion
      bool completed;

      // Get if level completed
      bool is_level_completed() {
         return level_completed_;
      }

      /*******************************************/
      // Textures for background and platforms
      Element background;
      Element platforms;

      // Animation map
      std::unordered_map<std::string, Animation *> animations;
      /*******************************************/

      // Level number
      Application::FOREST level;

      // Destructor will delete the entire level
      ~Level();

   private:
      // Number of enemies and platforms
      int num_of_enemies_;
      int num_of_platforms_;

      // enemies to kill
      int num_of_kills_;

      // Vector for the enemies
      std::vector<Enemy *> enemies_;
      std::vector<Enemy *> deferred_enemy_spawns_;
      std::vector<Enemy *> enemies_marked_for_death_;

      // Vector for the platforms
      std::vector<Platform *> platforms_;

      // Dir path
      std::string dir_;

      // Load media
      bool LoadMedia_();

      // flag for completed level
      bool level_completed_;
};

#endif
