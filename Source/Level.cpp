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
Level::Level(string file, Application *application) : 
   num_of_enemies_(0), num_of_platforms_(0), background_(0, -55, 1080, 1920, application), 
   application_(application) {

   // Read from a file based on a specific format
   ifstream input;
   input.open(file);
   if (!input) {
      cout << "Unable to open format file!" << endl;
      exit(1);
   }

   // Store directory into var
   size_t pos = file.find("format");
   if (pos != std::string::npos) {
      file.erase(pos, 6);
      dir_ = file;
   }

   // Get the enemies and their position
   input >> num_of_enemies_;
   for (int i = 0; i < num_of_enemies_; i++) {
      int x, y;
      input >> x >> y;
      enemies_.push_back(new Enemy(x, y, application)); 
   }

   // Get the platforms and their position
   input >> num_of_platforms_;
   for (int i = 0; i < num_of_platforms_; i++) {
      int x, y, height, width;
      input >> x >> y >> height >> width;
      platforms_.push_back(new Platform(x, y, height, width, application));
      platforms_[i]->setup();
   }

   // Now, load the media and quit if false
   if (load_media_() == false) {
      cout << "HEllo?" << endl;
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
      background_.texture.set_y(-55);

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
      platform_texture_.set_y(-55);
   }

   // Get enemy textures
   for (vector<Enemy *>::iterator it = enemies_.begin(); it != enemies_.end(); ++it) {
      if (application_->loadMediaEnemy((*it)) == false) {
         success = false;
      }
   }

   // Return success
   return success;
}

// Update function will render the stuff itself
void Level::update() {
   // Need to remove the bodies of dead creatures
   for (vector<Enemy *>::iterator it = enemies_.begin(); it != enemies_.end(); ++it) {
      if ((*it) && !(*it)->is_alive()) {
         if ((*it)->body) {
            application_->world_.DestroyBody((*it)->body);
            (*it)->body = NULL;
            application_->set_completed_level();
         }
      }
   }

   // Render the background
   background_.draw();

   // Render the platforms
   platform_texture_.render(0, -55);

   // Render platforms
   for (vector<Platform *>::iterator it = platforms_.begin(); it != platforms_.end(); ++it) {
      (*it)->update();
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

   // Clear both vectors
   platforms_.clear();
   enemies_.clear();
}
