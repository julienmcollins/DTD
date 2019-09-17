#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <Box2D/Box2D.h>

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "Level.h"
#include "Application.h"
#include "Texture.h"
#include "Entity.h"
#include "Object.h"
#include "Enemy.h"

using namespace std;

/************************** LEVEL CLASS ********************/

// Constructor will do all of the setting up essentially
Level::Level(string file, Application::FOREST level, Application *application) : 
   num_of_enemies_(0), num_of_platforms_(0), completed(false), level(level), 
   background_(0, 0, 1080, 1920, application), level_completed_(false), 
   application_(application) {

   // Read from a file based on a specific format
   ifstream input;
   input.open(file);
   if (!input) {
      cerr << "Unable to open format file!" << endl;
      exit(1);
   }

   // Store directory into var
   size_t pos = file.find("format");
   if (pos != string::npos) {
      file.erase(pos, 6);
      dir_ = file;
   }

   // Get the enemies and their position
   input >> num_of_enemies_;
   num_of_kills_ = num_of_enemies_;
   for (int i = 0; i < num_of_enemies_; i++) {
      int x, y;
      string name;
      input >> name >> x >> y;
      y += 55;
      if (name == "Fecreez") {
         enemies_.push_back(new Fecreez(x, y, application)); 
      } else if (name == "Rosea") {
         float angle;
         input >> angle;
         enemies_.push_back(new Rosea(x, y, angle, application));
         num_of_kills_ -= 1;
      } else if (name == "Mosquibler") {
         enemies_.push_back(new Mosquibler(x, y, application));
      } else if (name == "Fruig") {
         enemies_.push_back(new Fruig(x, y, application));
      } else if (name == "Fleet") {
         enemies_.push_back(new Fleet(x, y, application));
      } else if (name == "Mosqueenbler") {
         enemies_.push_back(new Mosqueenbler(x, y, application));
         num_of_kills_ -= 1;
      }
   }

   // Get the platforms and their position
   input >> num_of_platforms_;
   for (int i = 0; i < num_of_platforms_; i++) {
      int x, y, height, width;
      input >> width >> height >> x >> y;
      x += width / 2;
      y += (height / 2);
      platforms_.push_back(new Platform(x, y, height, width, application));
      platforms_[i]->setup();
   }

   // Add invisible walls
   platforms_.push_back(new Platform(-10, 540, 1080, 10, application));
   platforms_.back()->setup();
   platforms_.push_back(new Platform(1930, 540, 1080, 10, application));
   platforms_.back()->setup();
   num_of_platforms_ += 2;

   // Now, load the media and quit if false
   if (load_media_() == false) {
      cerr << "Failed to load level media" << endl;
      application_->set_quit();
   }

   // Set player's location
   int x, y;
   input >> x >> y;
   application_->get_player()->set_x(x);
   application_->get_player()->set_y(y);
}

// Load media function, private
bool Level::load_media_() {
   // Flag for quit
   bool success = true;

   // Get background
   if (!background_.texture.loadFromFile(dir_ + "background.png")) {
      cerr << "Failed to load background.png" << endl;
      success = false;
   } else {
      // Automatically calculate the number of sprites
      int num_clips = background_.texture.getWidth() / 1920;

      // Set the clips
      background_.texture.clips_ = new SDL_Rect[num_clips];
      SDL_Rect *temp = background_.texture.clips_;

      // Calc sprite
      for (int i = 0; i < num_clips; i++) {
         temp[i].x = i * 1920;
         temp[i].y = 0;
         temp[i].w = 1920;
         temp[i].h = 1080;
      }

      // Set background location
      background_.texture.set_x(0);
      background_.texture.set_y(0);

      // Set fps and stuff
      background_.texture.fps = 1.0f / 3.0f;
      background_.texture.max_frame_ = num_clips - 1;
      background_.texture.curr_clip_ = &temp[0];
   }

   // Get platforms texture
   if (!platform_texture_.loadFromFile(dir_ + "platforms.png")) {
      cerr << "Failed to load platforms.png" << endl;
      success = false;
   } else {
      // Set platforms location
      platform_texture_.set_x(0);
      platform_texture_.set_y(0);
   }

   // Return success
   return success;
}

// Update function will render the stuff itself
void Level::update() {
   // Only complete levels when number of enemies is 0
   if (num_of_kills_ == 0 && !completed) {
      // Delete the right wall
      if (platforms_.back()) {
         delete platforms_.back();
         platforms_.back() = nullptr;
      }

      if ((level < Application::FOREST6 || level == Application::FOREST9) && application_->get_player()->get_x() >= 1890) {
         completed = true;
      } else if (level >= Application::FOREST6 && application_->get_player()->get_y() <= -100) {
         completed = true;
      }
   }

   // Spawn enemies that were marked for deferred spawning
   if (!deferred_enemy_spawns_.empty()) {
      for (vector<Enemy *>::iterator it = deferred_enemy_spawns_.begin(); it != deferred_enemy_spawns_.end();) {
         enemies_.push_back(*it);
         it = deferred_enemy_spawns_.erase(it);
      }
   }

   // Destroy enemies that were marked for death
   if (!enemies_marked_for_death_.empty()) {
      for (vector<Enemy *>::iterator it = enemies_marked_for_death_.begin(); it != enemies_marked_for_death_.end();) {
         std::vector<Enemy *>::iterator destroy_it = find(enemies_.begin(), enemies_.end(), *it);
         if (destroy_it != enemies_.end()) {
            enemies_.erase(destroy_it);
            if ((*it)->is_marked_for_destroy()) {
               delete *it;
            }
            it = enemies_marked_for_death_.erase(it);
         } else {
            std::cerr << "Level::update() - enemy marked for death not found in level!\n";
            ++it;
         }
      }
   }

   // Need to remove the bodies of dead creatures
   for (vector<Enemy *>::iterator it = enemies_.begin(); it != enemies_.end(); ++it) {
      if ((*it) && (*it)->get_enemy_state() == Enemy::DEATH && (*it)->is_alive()) {
         if ((*it)->type() != "Rosea") {
            num_of_kills_ -= 1;
            (*it)->alive = false;
         }
      }
   }

   // TODO: Keep track of projectiles

   // Render the background
   background_.draw();

   // Render the platforms
   platform_texture_.render(0, 0);

   // Render platforms
   for (vector<Platform *>::iterator it = platforms_.begin(); it != platforms_.end(); ++it) {
      if (*it) {
         (*it)->update();
      }
   }

   // Render enemies
   for (vector<Enemy *>::iterator it = enemies_.begin(); it != enemies_.end();) {
      if (*it) {
         (*it)->update();
         ++it;
      } else {
         it = enemies_.erase(it);
      }
   }
}

// Add an enemy to the level
void Level::add_enemy(Enemy *new_enemy) {
   deferred_enemy_spawns_.push_back(new_enemy);
   //std::cout << "Level::add_enemy() - enemy = " << new_enemy->type() << std::endl;
   //std::cout << "Level::add_enemy() - size of array = " << enemies_.size() << std::endl;
   num_of_kills_ += 1;
}

// Destroy enemy
void Level::destroy_enemy(Enemy *enemy_to_delete) {
   enemies_marked_for_death_.push_back(enemy_to_delete);
   num_of_kills_ -= 1;
}

// Level destructor will just delete everything related to the level
Level::~Level() {
   // Free textures for background
   background_.texture.free();
   platform_texture_.free();

   // Delete platforms
   for (vector<Platform *>::iterator it = platforms_.begin(); it != platforms_.end(); ++it) {
      if (*it) {
         delete (*it);
         (*it) = NULL;
      }
   }

   // Delete enemies
   for (vector<Enemy *>::iterator it = enemies_.begin(); it != enemies_.end(); ++it) {
      if (*it) {
         delete (*it);
         (*it) = NULL;
      }
   }

   // Delete enemies
   for (vector<Enemy *>::iterator it = deferred_enemy_spawns_.begin(); it != deferred_enemy_spawns_.end(); ++it) {
      if (*it) {
         delete (*it);
         (*it) = NULL;
      }
   }

   // Delete enemies
   for (vector<Enemy *>::iterator it = enemies_marked_for_death_.begin(); it != enemies_marked_for_death_.end(); ++it) {
      if (*it) {
         delete (*it);
         (*it) = NULL;
      }
   }

   // Clear both vectors
   platforms_.clear();
   enemies_.clear();
}
