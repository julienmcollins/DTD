//
//  Application.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>
#include <Box2D/Box2D.h>
#include <vector>
#include <unordered_map>
#include <string>

#include "Application.h"
#include "Entity.h"
#include "Global.h"
#include "Level.h"
#include "Enemy.h"

//#define NUM_BLOCKS 11

// Constructs application
Application::Application() : SCREEN_WIDTH(1920.0f), SCREEN_HEIGHT(1080.0f), 
   SCREEN_FPS(60), SCREEN_TICKS_PER_FRAME(1000 / SCREEN_FPS), pause(-1), mainWindow(NULL), 
   current_key_states_(NULL), mouseButtonPressed(false), quit(false), 
   countedFrames(0), menu_flag(true),
   app_flag_(MAIN_SCREEN),
   menu_screen_(FIRST),
   game_flag_(SETUP),
   world_(gravity_), to_meters_(0.01f), to_pixels_(100.0f), debugDraw(this), test(0),
   timeStep_(1.0f / 60.0f), velocityIterations_(6), positionIterations_(2), animation_speed_(20.0f), 
   animation_update_time_(1.0f / animation_speed_), time_since_last_frame_(0.0f), 
   menu_background_(0, 0, 1080, 1920, this),
   menu_title_(640, 70, 513, 646, this),
   menu_items_(800, 650, 299, 321, this),
   world_items_(850, 650, 332, 193, this),
   ruler_(200, 722, 200, 50, this),
   gameover_screen_(0, 0, 1080, 1920, this),
   thanks_screen_(0, 0, 1080, 1920, this) {
    
    //Initialize SDL
    if (init()) {
        // Get screen dimensions
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        SCREEN_WIDTH = DM.w;
        SCREEN_HEIGHT = DM.h;
        
        // Creates window
        mainWindow = SDL_CreateWindow("Doodle 'Till Death", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
        
        if (mainWindow == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(0);
        } else {
            // Create renderer for window
            renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                exit(0);
            } else {
                // Initialize renderer color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }

        // Set gravity up
        gravity_ = {0.0f, -11.81f};
        world_.SetGravity(gravity_);
        // Set up debug drawer
        world_.SetDebugDraw(&debugDraw);
        debugDraw.AppendFlags( b2Draw::e_shapeBit );
        debugDraw.AppendFlags( b2Draw::e_aabbBit );
        debugDraw.AppendFlags( b2Draw::e_centerOfMassBit );
        // Set contact listener
        world_.SetContactListener(&contact_listener_);
    }

   // Start counting frames per second
   fpsTimer.start();
}

// Checks initialization of SDL functions
bool Application::init() {
   // Success flag 
   bool success = true;
    
   // Open audio first
   Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);

   //Initialize SDL
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
      printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
      success = false;
   } else {
      //Initialize PNG loading
      int imgFlags = IMG_INIT_PNG;
      if (!(IMG_Init(imgFlags) & imgFlags)) {
         printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
         success = false;
      }
      if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
         printf("Could not initialize mixer: %s\n", Mix_GetError());
         success = false;
      }
   }
   return success;
}

// Loads images and other media
// TODO: Move loadMedia function to constructor of each object --> like level
bool Application::loadMedia() {
   // Start success = true 
   bool success = true;

   /************** MAIN MENU STUFF *********************************/
   // Load forest
   menu_background_.load_image(menu_background_.textures, &menu_background_, 1920, 1080, 3, 1.0f / 4.0f, "forest", "images/miscealaneous/forestscreen.png", success);

   // Load cloud
   menu_background_.load_image(menu_background_.textures, &menu_background_, 1920, 1080, 3, 1.0f / 4.0f, "cloud", "images/miscealaneous/cloudscreen.png", success);

   // Load title
   if (!menu_title_.texture.loadFromFile("images/miscealaneous/title.png")) {
      printf("Failed to load title.png\n");
      success = false;
   } else {
      menu_title_.texture.clips_ = new SDL_Rect[3];
      SDL_Rect *temp = menu_title_.texture.clips_;

      for (int i = 0; i < 3; i++) {
         temp[i].x = i * 646;
         temp[i].y = 0;
         temp[i].w = 646;
         temp[i].h = 513;
      }

      // Set curr clip
      menu_title_.texture.curr_clip_ = &temp[0];
   }

   // Load menu
   if (!menu_items_.texture.loadFromFile("images/miscealaneous/menu.png")) {
      printf("Failed to load menu.png\n");
      success = false;
   } else {
      // Allocate room
      menu_items_.texture.clips_ = new SDL_Rect[3];
      SDL_Rect *temp = menu_items_.texture.clips_;

      // Allocate enough room
      for (int i = 0; i < 3; i++) {
         temp[i].x = i * 321;
         temp[i].y = 0;
         temp[i].w = 321;
         temp[i].h = 299;
      }

      // Set curr clip
      menu_items_.texture.curr_clip_ = &temp[0];
   }

   // Load world items
   if (!world_items_.texture.loadFromFile("images/miscealaneous/worlds.png")) {
      printf("Failed to load worlds.png\n");
      success = false;
   } else {
      // Allocate room
      world_items_.texture.clips_ = new SDL_Rect[3];
      SDL_Rect *temp = world_items_.texture.clips_;

      // Allocate enough room
      for (int i = 0; i < 3; i++) {
         temp[i].x = i * 193;
         temp[i].y = 0;
         temp[i].w = 193;
         temp[i].h = 332;
      }

      // Set curr clip
      world_items_.texture.curr_clip_ = &temp[0];
   }

   // Gameover screen
   if (!gameover_screen_.texture.loadFromFile("images/miscealaneous/gameover.png")) {
      printf("Failed to load gameover.png\n");
      success = false;
   } else {
      // Allocate room
      gameover_screen_.texture.clips_ = new SDL_Rect[3];
      SDL_Rect *temp = gameover_screen_.texture.clips_;

      // Allocate enough room
      for (int i = 0; i < 3; i++) {
         temp[i].x = i * 1920;
         temp[i].y = 0;
         temp[i].w = 1920;
         temp[i].h = 1080;
      }

      // Set curr clip
      gameover_screen_.texture.curr_clip_ = &temp[0];

      // Set fps
      gameover_screen_.texture.fps = 1.0f / 3.0f;
      gameover_screen_.texture.max_frame_ = 2;
   }

   // load ruler
   if (!ruler_.texture.loadFromFile("images/miscealaneous/ruler_200.png")) {
      printf("Failed to load ruler.png\n");
      success = false;
   }

   // Load thanks
   if (!thanks_screen_.texture.loadFromFile("images/miscealaneous/thanks.png")) {
      printf("Failed to load thanks.png\n");
      success = false;
   }
      
   // Return state
   return success;
}

// Setup main menu
void Application::setup_menu() {
   // Create player
   player = new Player(this);
   if (player->load_media() == false) {
      quit = true;
   }

   // Create finger
   finger_ = new Finger(this);
   if (finger_->load_media() == false) {
      quit = true;
   }
   point_ = false;
   item_ = false;
   clicked = false;

   // Set player location
   player->set_x(50);
   player->set_y(782);

   // Setup platform
   menu_platform_ = new Platform(960, 925, 10, 1920, this);
   menu_platform_->setup();

   // Setup invisible wall
   invisible_wall_ = new Platform(0, 1055, 1000, 10, this);
   invisible_wall_->setup();

   // Setup menu background
   //menu_background_.texture.fps = 1.0f / 4.0f;
   //menu_background_.texture.max_frame_ = 2;

   // Setup menu title
   menu_title_.texture.fps = 1.0f / 4.0f;
   menu_title_.texture.max_frame_ = 2;

   // Setup menu items
   menu_items_.texture.fps = 1.0f / 4.0f;
   menu_items_.texture.max_frame_ = 2;

   // World menu items
   world_items_.texture.fps = 1.0f / 4.0f;
   world_items_.texture.max_frame_ = 2;

   // Load and play music
   music = Mix_LoadMUS("sounds/hobbits.mp3");
   //Mix_PlayMusic(music, 1);

   // Set menu screen to first screen
   menu_screen_ = FIRST;
}

// Load specific textures when needed
SDL_Texture* Application::loadTexture(std::string path) {
    
   //The final optimized image
   SDL_Texture* newTexture = NULL;
   
   //Load image at specified path
   SDL_Surface* loadedSurface = IMG_Load(path.c_str());
   
   if(loadedSurface == NULL) {
       printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
   } else {
       
       //Create texture from surface pixels
       newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
       if(newTexture == NULL)
       {
           printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
       }
       
       //Get rid of old loaded surface
       SDL_FreeSurface(loadedSurface);
   }
   
   return newTexture;
}

// Updates the screen
void Application::update() {
   //SDL_SetWindowFullscreen(mainWindow, SDL_WINDOW_FULLSCREEN);
   //SDL_SetWindowDisplayMode(mainWindow, NULL);

   // Game loop
   while(!quit) {
      if (app_flag_ == MAIN_SCREEN) {
         if (pause == -1)
            main_screen();
      } else if (app_flag_ == PLAYGROUND) {
         playground();
      } else if (app_flag_ == GAMEOVER_SCREEN) {
         gameover_screen();
      } else if (app_flag_ == THANKS) {
         // Get current keyboard states
         current_key_states_ = SDL_GetKeyboardState(NULL);

         // Handle events on queue
         while (SDL_PollEvent( &e )) {
            //User requests quit
            if (e.type == SDL_QUIT) {
               quit = true;
            } else if (current_key_states_[SDL_SCANCODE_RETURN]) {
               app_flag_ = MAIN_SCREEN;
               menu_flag = true;
            }
         }

         // Start cap timer
         capTimer.start();

         // Clear screen
         SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
         SDL_RenderClear(renderer);

         // Calculate and correct fps
         float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
         if( avgFPS > 2000000 ) {
            avgFPS = 0;
         }

         // Draw title screen
         thanks_screen_.render(&thanks_screen_.texture);

         // Update the screen
         SDL_RenderPresent(renderer);
         ++countedFrames;

         // If frame finished early
         int frameTicks = capTimer.getTicks();
         if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            // Wait remaining time
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
         }
      }
   }
}

// ANIMATE FUNCTION
void Application::animate(const float &fps, Element *element, 
      Texture *texture, Timer &timer, float &last_frame) {
   last_frame += (timer.getDeltaTime() / 1000.0f); 
   if (last_frame > fps) {
      if (texture->frame_ > texture->max_frame_) {
         texture->frame_ = 0;
         texture->completed_ = true;
      }
      texture->curr_clip_ = &texture->clips_[texture->frame_];
      ++texture->frame_;
      last_frame = 0.0f;
   }

   // Draw title screen
   texture->render(element->get_x(), element->get_y(), texture->curr_clip_, 0.0, 
          &texture->center_, texture->flip_);
}

// GAMEOVER FUNCTION
void Application::gameover_screen() {
   // Get current keyboard states
   current_key_states_ = SDL_GetKeyboardState(NULL);

   // Handle events on queue
   while (SDL_PollEvent( &e )) {
      //User requests quit
      if (e.type == SDL_QUIT) {
          quit = true;
      } else if (current_key_states_[SDL_SCANCODE_RETURN]) {
         app_flag_ = MAIN_SCREEN;
      }
   }

   // Start cap timer
   capTimer.start();

   // Clear screen
   SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
   SDL_RenderClear(renderer);

   // Calculate and correct fps
   float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
   if( avgFPS > 2000000 ) {
      avgFPS = 0;
   }

   // Draw title screen
   animate(gameover_screen_.texture.fps, &gameover_screen_, &gameover_screen_.texture, 
      gameover_screen_.texture.fps_timer, gameover_screen_.texture.last_frame);

   // Update the screen
   SDL_RenderPresent(renderer);
   ++countedFrames;

   // If frame finished early
   int frameTicks = capTimer.getTicks();
   if (frameTicks < SCREEN_TICKS_PER_FRAME) {
      // Wait remaining time
      SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
   }
}

// MAIN SCREEN FUNCTION
void Application::main_screen() {
   // Setup menu
   if (menu_flag) {
      setup_menu();
      menu_flag = false;
   }

   // Get current keyboard states
   current_key_states_ = SDL_GetKeyboardState(NULL);

   // Clear screen
   SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
   SDL_RenderClear(renderer);

   // Update world timer
   world_.Step(timeStep_, velocityIterations_, positionIterations_);

   // Start cap timer
   capTimer.start();

   // Calculate and correct fps
   float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
   if( avgFPS > 2000000 ) {
      avgFPS = 0;
   }

   // Handle events on queue
   while (SDL_PollEvent( &e )) {
      //User requests quit
      if(e.type == SDL_QUIT) {
          quit = true;
      } else if (e.type == SDL_KEYDOWN) {
         if (e.key.keysym.sym == SDLK_p) {
            pause *= -1;
         } if (finger_ && finger_->updating) {
            if (finger_->get_y() == OPTIONS && e.key.keysym.sym == SDLK_UP) {
               finger_->set_y(START);
               finger_->set_x(700);
            } else if (finger_->get_y() == EGGS && e.key.keysym.sym == SDLK_UP) {
               finger_->set_y(OPTIONS);
               finger_->set_x(650);
            } else if (finger_->get_y() == START && e.key.keysym.sym == SDLK_DOWN) {
               finger_->set_y(OPTIONS);
               finger_->set_x(650);
            } else if (finger_->get_y() == OPTIONS && e.key.keysym.sym == SDLK_DOWN) {
               finger_->set_y(EGGS);
               finger_->set_x(720);
            } else if (finger_->get_y() == WORLD2 && e.key.keysym.sym == SDLK_UP) {
               finger_->set_y(WORLD1);
            } else if (finger_->get_y() == WORLD3 && e.key.keysym.sym == SDLK_UP) {
               finger_->set_y(WORLD2);
            } else if (finger_->get_y() == WORLD4 && e.key.keysym.sym == SDLK_UP) {
               finger_->set_y(WORLD3);
            } else if (finger_->get_y() == WORLD5 && e.key.keysym.sym == SDLK_UP) {
               finger_->set_y(WORLD4);
            } else if (finger_->get_y() == WORLD1 && e.key.keysym.sym == SDLK_DOWN) {
               finger_->set_y(WORLD2);
            } else if (finger_->get_y() == WORLD2 && e.key.keysym.sym == SDLK_DOWN) {
               finger_->set_y(WORLD3);
            } else if (finger_->get_y() == WORLD3 && e.key.keysym.sym == SDLK_DOWN) {
               finger_->set_y(WORLD4);
            } else if (finger_->get_y() == WORLD4 && e.key.keysym.sym == SDLK_DOWN) {
               finger_->set_y(WORLD5);
            } else if (e.key.keysym.sym == SDLK_RETURN) {
               finger_->finger_state = Finger::POINT;
            }
         }
      }
   }

   // Process player inputs
   //player->process_input(current_key_states_);

   /* ANIMATION FOR TITLE SCREEN */
   // Animate background
   if (finger_) {
      if (finger_->get_y() == START || finger_->get_y() == WORLD1) {
         animate(menu_background_.textures["forest"].fps, &menu_background_, &menu_background_.textures["forest"], 
            menu_background_.textures["forest"].fps_timer, menu_background_.textures["forest"].last_frame);
      } else if (finger_->get_y() == WORLD2) {
         animate(menu_background_.textures["cloud"].fps, &menu_background_, &menu_background_.textures["cloud"], 
            menu_background_.textures["cloud"].fps_timer, menu_background_.textures["cloud"].last_frame);
      }
   }

   // Animate menu items
   if (menu_screen_ == FIRST) {
      // Animate menu items
      animate(menu_items_.texture.fps, &menu_items_, &menu_items_.texture, 
            menu_items_.texture.fps_timer, menu_items_.texture.last_frame);

      // Animate player
      player->update(true);

      // Update finger
      finger_->update();
   } else if (menu_screen_ == SECOND) {
      // Animate world items
      animate(world_items_.texture.fps, &world_items_, &world_items_.texture,
            world_items_.texture.fps_timer, world_items_.texture.last_frame);

      // Animate player
      player->update(true);

      // Update finger
      finger_->update();
   } else if (menu_screen_ == THIRD) {
      // Animate final background
      // Update player for real (and projectiles)
      player->update();
      update_projectiles();

      // Check to see if player has reached the edge
      if (player->get_x() >= 1890) {
         app_flag_ = PLAYGROUND;
         level_flag_ = FORESTBOSS;
         game_flag_ = SETUP;
         delete menu_platform_;
         delete invisible_wall_;
         //Mix_FreeMusic(music);
                  
         // Delete finger_
         delete finger_;
         finger_ = nullptr;
      }
   }

   // Animate title
   animate(menu_title_.texture.fps, &menu_title_, &menu_title_.texture,
         menu_title_.texture.fps_timer, menu_title_.texture.last_frame);

   // Check enter key state
   if (finger_ && finger_->finger_state == Finger::POINT) {
      if (finger_->get_y() == START) {
         if (finger_->textures["point"].completed_) {
            menu_screen_ = SECOND;
            finger_->finger_state = Finger::SHAKE;
            finger_->set_y(WORLD1);
            finger_->set_x(720);
            finger_->textures["point"].completed_ = false;
         }
      } else if (finger_->get_y() == WORLD1) {
         if (finger_->textures["point"].completed_) {
            // Set to final menu screen
            menu_screen_ = THIRD;

            // Tell finger not to update
            finger_->updating = false;
         }
      }
   }
   /*
   // DEBUG DRAW
   world_.DrawDebugData();

   for (int i = 0; i < 7; i++) {
      SDL_Rect m;
      m.w = r[i].w;
      m.h = r[i].h;
      m.x = r[i].x;
      m.y = r[i].y - m.h;

      SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
      SDL_RenderDrawRect(renderer, &m);
   }
   */
  
   // Update the screen
   SDL_RenderPresent(renderer);
   ++countedFrames;

   // If frame finished early
   int frameTicks = capTimer.getTicks();
   if (frameTicks < SCREEN_TICKS_PER_FRAME) {
      // Wait remaining time
      SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
   }
}

// UPDATE PROJECTILES
void Application::update_projectiles() {
   // ITERATE THROUGH THE PROJECTILES AND DRAW THEM
   for (std::vector<Projectile *>::iterator it = projectiles_.begin(); it != projectiles_.end();) {
      // Check to see if it's still allocated
      if (*it) {
         // Check if it's alive or not
         if (!(*it)->is_alive()) {
            //std::cout << "IN here" << std::endl;
            delete (*it);
            it = projectiles_.erase(it);
         } else {
            //std::cout << "NO, in here" << std::endl;
            (*it)->update();
            ++it;
         }
      } else {
         it = projectiles_.erase(it);
      }
   }
}

// PLAYGROUND FUNCTION
void Application::playground() {
   // Setup level
   if (game_flag_ == SETUP) {
      std::string path1("images/levels/Forest/board");
      std::string path2("/format");
      std::string path = path1 + std::to_string(static_cast<int>(level_flag_)) + path2;
      level = new Level(path, level_flag_, this);
      game_flag_ = PLAY;
   }

   // Clear screen as the first things that's done?
   SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
   SDL_RenderClear(renderer);

   // Update world timer
   world_.Step(timeStep_, velocityIterations_, positionIterations_);

   // Start cap timer
   capTimer.start();
      
   // Calculate and correct fps
   float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
   if( avgFPS > 2000000 ) {
      avgFPS = 0;
   }

   // Get current keyboard states
   current_key_states_ = SDL_GetKeyboardState(NULL);

   // Handle events on queue
   while (SDL_PollEvent( &e )) {
      //User requests quit
      if (e.type == SDL_QUIT) {
          quit = true;
      }
      if (level_flag_ == TEMP & e.type == SDL_KEYDOWN) {
         app_flag_ = MAIN_SCREEN;
      }
   }

   /******** UPDATE THE LEVEL *************/
   // Do this as the last thing (testing)
   level->update();
   /***************************************/
   
   // ITERATE THROUGH THE PROJECTILES AND DRAW THEM
   update_projectiles();
      
   //std::cout << player << std::endl;

   // Update player
   if (player->is_alive()) {
      // Process player inputs
      //player->process_input(current_key_states_);

      // Update player
      player->update();

      // Render the hit markers
      for (int i = 0; i < player->hit_markers.size(); i++) {
         player->hit_markers[i]->update();
      }

      // Check for completed level and that player as walked to the edge of the screen
      if (level->completed) {
         // Destroy the level
         delete level;
         level = nullptr;

         // Change mode to setup
         game_flag_ = SETUP;

         // Increment level_flag_
         int inc = static_cast<int>(level_flag_);
         level_flag_ = static_cast<FOREST>(inc + 1);
      }
   } else {
      app_flag_ = GAMEOVER_SCREEN;
      delete player;
      delete level;
      level = nullptr;
      menu_flag = true;
   }

   /*
   // DEBUG DRAW
   world_.DrawDebugData();

   for (int i = 0; i < 7; i++) {
      SDL_Rect m;
      m.w = r[i].w;
      m.h = r[i].h;
      m.x = r[i].x;
      m.y = r[i].y - m.h;

      SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
      SDL_RenderDrawRect(renderer, &m);
   }
   */

   // Update the screen
   SDL_RenderPresent(renderer);
   ++countedFrames;

   // If frame finished early
   int frameTicks = capTimer.getTicks();
   if (frameTicks < SCREEN_TICKS_PER_FRAME) {
      // Wait remaining time
      SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
   }
}

// Set the viewport for minimaps and stuff like that if needed
void Application::setViewport() {
    // Change these values depending on if and where you want your minimap or whatever to be at
    viewport.x = 0;
    viewport.y = 0;
    viewport.h = SCREEN_HEIGHT / 2;
    viewport.w = SCREEN_WIDTH / 2;
    SDL_RenderSetViewport(renderer, &viewport);
}

// Get the height
const int Application::get_height() {
    return SCREEN_HEIGHT;
}

// Get the width
const int Application::get_width() {
    return SCREEN_WIDTH;
}

// Get ground
Texture Application::get_ground() {
    return ground->texture;
}

// Destructs application
Application::~Application() {
   // Clear the screen
   SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
   SDL_RenderClear(renderer);
   
   // Stop fps timer
   fpsTimer.stop();
   capTimer.stop();

   //Destroy window
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(mainWindow);
   mainWindow = NULL;
   renderer = NULL;
   
   //Quit SDL subsystems
   IMG_Quit();
   SDL_Quit();
}

/********** FINGER ***************/

// Constructor
Finger::Finger(Application *application) : 
   Element(700, 665, 67, 124, application), finger_state(SHAKE),
   updating(true) {
   // Setup finger
   textures["shake"].fps = 1.0f / 20.0f;
   textures["point"].fps = 1.0f / 20.0f;
   textures["shake"].max_frame_ = 7;
   textures["point"].max_frame_ = 5;
}

// Get texture
Texture *Finger::get_texture() {
   if (finger_state == SHAKE) {
      return &textures["shake"];
   } else if (finger_state == POINT) {
      return &textures["point"];
   }
}

// Update function
void Finger::update() {
   // Animate based on state
   if (finger_state == SHAKE) {
      animate(&textures["shake"]);
   } else if (finger_state == POINT) {
      animate(&textures["point"]);
   }

   // Get texture and render it
   Texture *tex = get_texture();
   
   // Render finger
   render(tex);
}

// Load media function
bool Finger::load_media() {
   // Success flag
   bool success = true; 

   // Load finger point
   textures.emplace("point", Texture());
   if (!textures["point"].loadFromFile("images/miscealaneous/finger_point.png")) {
      printf("Failed to load finger_point.png\n");
      success = false;
   } else {
      // Allocate enough room
      textures["point"].clips_ = new SDL_Rect[6];
      SDL_Rect *temp = textures["point"].clips_;

      // Calculate sprite locations
      for (int i = 0; i < 6; i++) {
         temp[i].x = i * 124;
         temp[i].y = 0;
         temp[i].w = 124;
         temp[i].h = 67;
      }

      // Set curr clip
      textures["point"].curr_clip_ = &temp[0];
   }

   // Load finger shake
   textures.emplace("shake", Texture());
   if (!textures["shake"].loadFromFile("images/miscealaneous/finger_shake.png")) {
      printf("Failed to load finger_shake.png\n");
      success = false;
   } else {
      // Allocate enough room
      textures["shake"].clips_ = new SDL_Rect[8];
      SDL_Rect *temp = textures["shake"].clips_;

      // Calculate sprite locations
      for (int i = 0; i < 8; i++) {
         temp[i].x = i * 124;
         temp[i].y = 0;
         temp[i].w = 124;
         temp[i].h = 67;
      }

      // Set curr clip
      textures["shake"].curr_clip_ = &temp[0];
   }

   // Return success flag
   return success;
}