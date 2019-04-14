#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <Box2D/Box2D.h>

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "Level.h"
#include "Texture.h"
#include "Entity.h"
#include "Object.h"

using namespace std;

/************************** LEVEL CLASS ********************/

// Constructor will do all of the setting up essentially
Level::Level(string file, Application *application) : 
   num_of_enemies_(0), num_of_platforms_(0), application_(application) {

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
      int x, y;
      input >> x >> y;
      platforms_.push_back(new Platform(x, y, application));
   }
}

bool Level::load_media_(string dir) {
   // Flag for quit
   bool success = true;

   // Get background
   if (!background_texture_.loadFromFile(dir + "background.png")) {
      cerr << "Failed to load background.png" << endl;
      success = false;
   } else {
      background_texture_.clips_ = new SDL_Rect[2];
      SDL_Rect *temp = background_texture_.clips_;

      // Calc sprite
      for (int i = 0; i < 2; i++) {
         temp[i].x = i * 1920;
         temp[i].y = 0;
         temp[i].w = 1920;
         temp[i].h = 1080;
      }
   }

   // Get platforms texture
   if (!platform_texture_.loadFromFile(dir + "platforms.png")) {
      cerr << "Failed to load platforms.png" << endl;
      success = false;
   }

   // Get enemy textures

}
