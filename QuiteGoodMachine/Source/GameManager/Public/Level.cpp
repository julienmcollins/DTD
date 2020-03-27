#include "QuiteGoodMachine/Source/GameManager/Private/Level.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Object.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Enemy.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/ObjectManager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <Box2D/Box2D.h>

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

/************************** LEVEL CLASS ********************/

// Constructor will do all of the setting up essentially
Level::Level() : 
   num_of_enemies_(0), num_of_platforms_(0), completed(false), 
   background(0, 0, 0, 0), platforms(0, 0, 0, 0), fills(0, 0, 0, 0), level_completed_(false) {

   // Now, load the media and quit if false
   if (LoadMedia_() == false) {
      cerr << "Failed to load level media" << endl;
      Application::GetInstance().set_quit();
   }
}

// Load media function, private
bool Level::LoadMedia_() {
   // Flag for quit
   bool success = true;

   // Get the level backgrounds
   std::string dir = Application::GetInstance().sprite_path + "Menu/forest_background_sheet.png";
   background.sprite_sheet = RenderingEngine::GetInstance().LoadTexture("forest_background_sheet", dir.c_str());
   background.animations.emplace("forest_background_1", new Animation(background.sprite_sheet, "forest_background_1", 1920.0f, 1080.0f, 0.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_2", new Animation(background.sprite_sheet, "forest_background_2", 1920.0f, 1080.0f, 1080.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_3", new Animation(background.sprite_sheet, "forest_background_3", 1920.0f, 1080.0f, 2160.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_4", new Animation(background.sprite_sheet, "forest_background_4", 1920.0f, 1080.0f, 3240.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_5", new Animation(background.sprite_sheet, "forest_background_5", 1920.0f, 1080.0f, 4320.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_6", new Animation(background.sprite_sheet, "forest_background_6", 1920.0f, 1080.0f, 5400.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_7", new Animation(background.sprite_sheet, "forest_background_7", 1920.0f, 1080.0f, 6480.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_8", new Animation(background.sprite_sheet, "forest_background_8", 1920.0f, 1080.0f, 7560.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_9", new Animation(background.sprite_sheet, "forest_background_9", 1920.0f, 1080.0f, 8640.0f, 2, 1.0f / 3.0f));
   background.animations.emplace("forest_background_10", new Animation(background.sprite_sheet, "forest_background_10", 1920.0f, 1080.0f, 9720.0f, 2, 1.0f / 3.0f));
   RenderingEngine::GetInstance().LoadResources(&background);

   // Get the platform backgrounds
   dir = Application::GetInstance().sprite_path + "Menu/forest_platform_sheet.png";
   platforms.sprite_sheet = RenderingEngine::GetInstance().LoadTexture("forest_platform_sheet", dir.c_str());
   platforms.animations.emplace("forest_platform_1", new Animation(platforms.sprite_sheet, "forest_platform_1", 1920.0f, 1080.0f, 0.0f, 1, 1.0f / 1.0f));
   platforms.animations.emplace("forest_platform_2", new Animation(platforms.sprite_sheet, "forest_platform_2", 1920.0f, 1080.0f, 1080.0f, 1, 1.0f / 1.0f));
   platforms.animations.emplace("forest_platform_3", new Animation(platforms.sprite_sheet, "forest_platform_3", 1920.0f, 1080.0f, 2160.0f, 1, 1.0f / 1.0f));
   platforms.animations.emplace("forest_platform_4", new Animation(platforms.sprite_sheet, "forest_platform_4", 1920.0f, 1080.0f, 3240.0f, 1, 1.0f / 1.0f));
   platforms.animations.emplace("forest_platform_5", new Animation(platforms.sprite_sheet, "forest_platform_5", 1920.0f, 1080.0f, 4320.0f, 1, 1.0f / 1.0f));
   platforms.animations.emplace("forest_platform_6", new Animation(platforms.sprite_sheet, "forest_platform_6", 1920.0f, 1080.0f, 5400.0f, 1, 1.0f / 1.0f));
   platforms.animations.emplace("forest_platform_7", new Animation(platforms.sprite_sheet, "forest_platform_7", 1920.0f, 1080.0f, 6480.0f, 1, 1.0f / 1.0f));
   platforms.animations.emplace("forest_platform_8", new Animation(platforms.sprite_sheet, "forest_platform_8", 1920.0f, 1080.0f, 7560.0f, 1, 1.0f / 1.0f));
   platforms.animations.emplace("forest_platform_9", new Animation(platforms.sprite_sheet, "forest_platform_9", 1920.0f, 1080.0f, 8640.0f, 1, 1.0f / 1.0f));
   dir = Application::GetInstance().sprite_path + "Menu/forest_background_boss.png";
   platforms.extra_sheets["forest_boss"] = RenderingEngine::GetInstance().LoadTexture("forest_platform_boss", dir.c_str());
   platforms.animations.emplace("forest_platform_10", new Animation(platforms.extra_sheets["forest_boss"], "forest_platform_10", 1920.0f, 1080.0f, 9720.0f, 44, 1.0f / 24.0f, 6, 8));
   RenderingEngine::GetInstance().LoadResources(&platforms);

   // Return success
   return success;
}

// Update function will Render the stuff itself
void Level::Update() {
   //std::cout << "Level::Update() - num_of_kills_ = " << num_of_kills_ << std::endl;
   // Only complete levels when number of enemies is 0
   if (num_of_kills_ == 0 && !completed) {
      // Delete the right wall
      if (platforms_.back()) {
         delete platforms_.back();
         platforms_.back() = nullptr;
      }

      //std::cout << "Level::Update() - level = " << level << std::endl;
      if ((level < Application::FOREST6 || level == Application::FOREST9) && Application::GetInstance().get_player()->get_x() >= 1890) {
         completed = true;
      } else if (level >= Application::FOREST6 && Application::GetInstance().get_player()->get_y() <= -100) {
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
            // if ((*it)->is_marked_for_destroy()) {
            //    delete *it;
            // }
            it = enemies_marked_for_death_.erase(it);
         } else {
            std::cerr << "Level::Update() - enemy marked for death not found in level!\n";
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
   int lvl = static_cast<int>(level);
   std::string bg = "forest_background_" + std::to_string(lvl);
   background.sprite_sheet->Animate(background.GetAnimationByName(bg));
   background.sprite_sheet->Render(0.0f, 0.0f, 0.0f, background.GetAnimationByName(bg));

   // Render the platforms
   std::string pf = "forest_platform_" + std::to_string(lvl);
   if (lvl != 10) {
      platforms.sprite_sheet->Render(0.0f, 0.0f, 0.0f, platforms.GetAnimationByName(pf));
   } else {
      platforms.extra_sheets["forest_boss"]->Animate(platforms.GetAnimationByName(pf), platforms.GetAnimationByName(pf)->reset_frame, platforms.GetAnimationByName(pf)->max_frame);
      platforms.extra_sheets["forest_boss"]->Render(0.0f, 0.0f, 0.0f, platforms.GetAnimationByName(pf));
   }

   // Render enemies
   for (vector<Enemy *>::iterator it = enemies_.begin(); it != enemies_.end();) {
      if (*it) {
         (*it)->Update();
         ++it;
      } else {
         it = enemies_.erase(it);
      }
   }
}

void Level::SetBoard(std::string file, Application::FOREST level) {
   // Reset everything
   num_of_enemies_ = 0;
   num_of_platforms_ = 0;
   completed = false;
   level_completed_ = false;

   // Set level
   this->level = level;

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

      // Compensate for fullscreen vs windowed
      y += 55;

      // Spawn enemies accordingly
      if (name == "Fecreez") {
         enemies_.push_back(new Fecreez(x, y)); 
      } else if (name == "Rosea") {
         float angle;
         input >> angle;
         enemies_.push_back(new Rosea(x, y, angle));
         num_of_kills_ -= 1;
      } else if (name == "Mosquibler") {
         enemies_.push_back(new Mosquibler(x, y));
      } else if (name == "Fruig") {
         enemies_.push_back(new Fruig(x, y));
      } else if (name == "Fleet") {
         enemies_.push_back(new Fleet(x, y));
      } else if (name == "Mosqueenbler") {
         enemies_.push_back(new Mosqueenbler(x, y));
         num_of_kills_ -= 1;
      } else if (name == "Wormored") {
         enemies_.push_back(new Wormored(x, y));
      }
   }

   // Get the platforms and their position
   input >> num_of_platforms_;
   for (int i = 0; i < num_of_platforms_; i++) {
      int x, y, height, width;
      input >> width >> height >> x >> y;
      x += width / 2;
      y += height / 2;
      platforms_.push_back(new Platform(x, y, width, height));
      platforms_[i]->setup();
   }

   // Add invisible walls
   platforms_.push_back(new Platform(-10, 540, 10, 1080));
   platforms_.back()->setup();
   platforms_.push_back(new Platform(1930, 540, 10, 1080));
   platforms_.back()->setup();
   num_of_platforms_ += 2;

   // Set player's location
   int x, y;
   input >> x >> y;
   Application::GetInstance().get_player()->set_x(x);
   Application::GetInstance().get_player()->set_y(y);
}

void Level::ClearBoard() {
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
   ClearBoard();
}
