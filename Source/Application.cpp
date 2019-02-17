//
//  Application.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <Box2D/Box2D.h>
#include <vector>
#include <unordered_map>
#include <string>

#include "Application.h"
#include "Entity.h"
#include "Global.h"

//#define NUM_BLOCKS 11

// Constructs application
Application::Application() : SCREEN_WIDTH(1920.0f), SCREEN_HEIGHT(1080.0f), 
   SCREEN_FPS(60), SCREEN_TICKS_PER_FRAME(1000 / SCREEN_FPS), mainWindow(NULL), 
   current_key_states_(NULL), player(this), enemy(this), mouseButtonPressed(false), quit(false), 
   countedFrames(0), lv1_flag(true), game_flag_(MAIN_SCREEN), world_(gravity_), to_meters_(0.01f), 
   to_pixels_(100.0f), debugDraw(this), test(0),
   timeStep_(1.0f / 60.0f), velocityIterations_(6), positionIterations_(2), animation_speed_(20.0f), 
   animation_update_time_(1.0f / animation_speed_), time_since_last_frame_(0.0f), finger_(NULL, 0),
   item_(0), title_screen_(NULL, 0), background_(NULL, 0) {
    
    //Initialize SDL
    if (init()) {
        // Get screen dimensions
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        SCREEN_WIDTH = DM.w;
        SCREEN_HEIGHT = DM.h - 55;
        
        // Creates window
        mainWindow = SDL_CreateWindow("Doodle 'Till Death", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        
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
        gravity_ = {0.0f, -9.81f};
        world_.SetGravity(gravity_);
        
        // Set up debug drawer
        //world_.SetDebugDraw(&debugDraw);
        //debugDraw.AppendFlags( b2Draw::e_shapeBit );
        //debugDraw.AppendFlags( b2Draw::e_aabbBit );
        //debugDraw.AppendFlags( b2Draw::e_centerOfMassBit );

        // Set contact listener
        world_.SetContactListener(&contact_listener_);
    }

   // Start counting frames per second
   fpsTimer.start();
}

// Checks initialization of SDL functions
bool Application::init() {
    
    bool success = true;
    
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    } else {
        //Initialize PNG loading
      int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
            success = false;
        }
    }
    return success;
}

// Loads images and other media
bool Application::loadMedia() {
    
   bool success = true;
   
   // Load player idle
   if (!player.idle_texture.loadFromFile("images/player/idle_na.png")) {
       printf("Failed to load Texture image!\n");
       success = false;
   } else {
      // Allocat enough room for the clips
      player.idle_texture.clips_ = new SDL_Rect[16];
      SDL_Rect *temp = player.idle_texture.clips_;

      // Calculate locations
      for (int i = 0; i < 16; i++) {
         temp[i].x = i * 92;
         temp[i].y = 0;
         temp[i].w = 92;
         temp[i].h = 150;
      }
   }

   // Load player running
   if (!player.running_texture.loadFromFile("images/player/running_na.png")) {
       printf("Failed to load Texture image!\n");
       success = false;
   } else {
      // Allocat enough room for the clips
      player.running_texture.clips_ = new SDL_Rect[20];
      SDL_Rect *temp = player.running_texture.clips_;

      // Calculate locations
      for (int i = 0; i < 20; i++) {
         temp[i].x = i * 92;
         temp[i].y = 0;
         temp[i].w = 92;
         temp[i].h = 150;
      }
   }

   // Load player kicking
   if (!player.kick_texture.loadFromFile("images/player/kick.png")) {
       printf("Failed to load Texture image!\n");
       success = false;
   } else {
      // Allocat enough room for the clips
      player.kick_texture.clips_ = new SDL_Rect[16];
      SDL_Rect *temp = player.kick_texture.clips_;

      // Calculate locations
      for (int i = 0; i < 16; i++) {
         temp[i].x = i * 75;
         temp[i].y = 0;
         temp[i].w = 75;
         temp[i].h = 150;
      }
   }

   // Load jump and run
   if (!player.running_jump_texture.loadFromFile("images/player/running_jump_na.png")) {
      printf("Failed to load Running Jump image!\n");
      success = false;
   } else {
      // Allocate enough room for the clips
      player.running_jump_texture.clips_ = new SDL_Rect[17];
      SDL_Rect *temp = player.running_jump_texture.clips_;

      // Calculate the locations
      for (int i = 0; i < 17; i++) {
         temp[i].x = i * 92;
         temp[i].y = 0;
         temp[i].w = 92;
         temp[i].h = 150;
      }
   }

   // Turn animation width 52 --> turns from facing right to left

   // Load arm
   if (!player.arm_texture.loadFromFile("images/player/idle_arm_na.png")) {
      printf("Failed to load Arm image!\n");
      success = false;
   }

   // Load shooting arm
   if (!player.arm_shoot_texture.loadFromFile("images/player/arm.png")) {
      printf("Failed to load arm shooting texture!\n");
      success = false;
   } else {
     // Allocate enough room for the clips
     player.arm_shoot_texture.clips_ = new SDL_Rect[9];
     SDL_Rect *temp = player.arm_shoot_texture.clips_;

     // Calculate the locations
     for (int i = 0; i < 9; i++) {
        temp[i].x = i * 63;
        temp[i].y = 0;
        temp[i].w = 63;
        temp[i].h = 49;
     }
   }

   // Running arm
   if (!player.arm_running_texture.loadFromFile("images/player/running_arm.png")) {
      printf("Failed to load arm shooting texture!\n");
      success = false;
   } else {
      // Allocate enough room for the clips
      player.arm_running_texture.clips_ = new SDL_Rect[4];
      SDL_Rect *temp = player.arm_running_texture.clips_;

      // Calculate the locations
      for (int i = 0; i < 4; i++) {
         temp[i].x = i * 10;
         temp[i].y = 0;
         temp[i].w = 10;
         temp[i].h = 33;
      }
   }

   // Eraser
   if (!player.eraser_texture.loadFromFile("images/player/eraser.png")) {
      printf("Failed to load eraser texture!\n");
      success = false;
   } else {
      // Allocate one image for it
      player.eraser_texture.clips_ = new SDL_Rect[1];
      SDL_Rect *temp = player.eraser_texture.clips_;
      temp[0].x = 0; temp[0].y = 0; temp[0].w = 21; temp[0].h = 12;
   }

   /******** ENEMY **********/
   if (!enemy.idle_texture.loadFromFile("images/enemies/fecreez_idle.png")) {
      printf("Failed to load feecreez idle texture!\n");
      success = false;
   } else {
      // Allocate enough room
      enemy.idle_texture.clips_ = new SDL_Rect[18];
      SDL_Rect *temp = enemy.idle_texture.clips_;

      // Calculate sprite locations
      for (int i = 0; i < 18; i++) {
         temp[i].x = i * 82;
         temp[i].y = 0;
         temp[i].w = 82;
         temp[i].h = 92;
      }
   }

   if (!enemy.shoot_texture.loadFromFile("images/enemies/fecreez_shoot.png")) {
      printf("Failed to load feecreez shoot texture!\n");
      success = false;
   } else {
      // Allocate enough room
      enemy.shoot_texture.clips_ = new SDL_Rect[7];
      SDL_Rect *temp = enemy.shoot_texture.clips_;

      // Calculate sprite locations
      for (int i = 0; i < 7; i++) {
         temp[i].x = i * 82;
         temp[i].y = 0;
         temp[i].w = 82;
         temp[i].h = 92;
      }
   }

   if (!enemy.poojectile_texture.loadFromFile("images/enemies/poojectile.png")) {
      printf("Failed to load poojectile texture!\n");
      success = false;
   } else {
      // Allocate enough room
      enemy.poojectile_texture.clips_ = new SDL_Rect[8];
      SDL_Rect *temp = enemy.poojectile_texture.clips_;

      // Calculate sprite locations
      for (int i = 0; i < 8; i++) {
         temp[i].x = i * 24;
         temp[i].y = 0;
         temp[i].w = 24;
         temp[i].h = 15;
      }
   }
   /**************************/

   // Load finger
   if (!finger_.loadFromFile("images/miscealaneous/finger.png")) {
      printf("Failed to load finger.png\n");
      success = false;
   } else {
      // Display finger
      finger_.set_x(750);
      finger_.set_y(680);
   }

   // Load title screen
   if (!title_screen_.loadFromFile("images/miscealaneous/titlescreen.png")) {
      printf("Failed to load titlescreen.png\n");
      success = false;
   }
      
   // Return state
   return success;
}

// Load level 1 media
bool Application::loadMediaLvl1() {
   // Flag for quitting
   bool success = true;

   // Load background 
   if (!background_.loadFromFile("images/levels/lv1bg.png")) {
       printf("Failed to load Texture image!\n");
       success = false;
   }
   
   // Load ground
   if (!ground->texture.loadFromFile("images/miscealaneous/floor.png")) {
       printf("Failed to load thinstrip.png\n");
       success = false;
   } else {
      // Set the heights and widths of the stuff
      ground->set_height(ground->texture.getHeight());
      ground->set_width(ground->texture.getWidth());

      // Setup the platform
      ground->setup();
   }

   // Load platform
   for (int i = 0; i < NUM_BLOCKS; i++) {
      std::string name ("images/levels/lv1pf");
      name += std::to_string(i + 1) + ".png";
      if (!platforms[i]->texture.loadFromFile(name)) {
          printf("Failed to load platforms\n");
          success = false;
      } else {
         // Set height and width
         platforms[i]->set_height(platforms[i]->texture.getHeight());
         platforms[i]->set_width(platforms[i]->texture.getWidth());

         // Setup
         platforms[i]->setup();
      }
   }

   // Return success
   return success;
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
      if (game_flag_ == MAIN_SCREEN) {
         main_screen();
      } else if (game_flag_ == PLAYGROUND) {
         playground();
      }
   }
}

// MAIN SCREEN FUNCTION
void Application::main_screen() {
   // Get current keyboard states
   current_key_states_ = SDL_GetKeyboardState(NULL);

   // Handle events on queue
   while (SDL_PollEvent( &e )) {
      //User requests quit
      if(e.type == SDL_QUIT) {
          quit = true;
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
   title_screen_.render(0, 0);

   // Animate picking
   if (finger_.get_y() == OPTIONS && current_key_states_[SDL_SCANCODE_UP]) {
      finger_.set_y(START);
      usleep(200000);
   } else if (finger_.get_y() == EGGS && current_key_states_[SDL_SCANCODE_UP]) {
      finger_.set_y(OPTIONS);
      usleep(200000);
   } else if (finger_.get_y() == START && current_key_states_[SDL_SCANCODE_DOWN]) {
      finger_.set_y(OPTIONS);
      usleep(200000);
   } else if (finger_.get_y() == OPTIONS && current_key_states_[SDL_SCANCODE_DOWN]) {
      finger_.set_y(EGGS);
      usleep(200000);
   }

   // Render
   finger_.render(finger_.get_x(), finger_.get_y());

   // Check enter key state
   if (current_key_states_[SDL_SCANCODE_RETURN]) {
      if (finger_.get_y() == START) {
         game_flag_ = PLAYGROUND;
      }
   }

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

// PLAYGROUND FUNCTION
void Application::playground() {
   // First setup level 1
   if (lv1_flag) {
      setup_lv1();
      lv1_flag = false;
   }

   // Clear screen as the first things that's done?
   SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
   SDL_RenderClear(renderer);

   // Update world timer
   world_.Step(timeStep_, velocityIterations_, positionIterations_);

   // Start cap timer
   capTimer.start();

   // Get current keyboard states
   current_key_states_ = SDL_GetKeyboardState(NULL);

   // Handle events on queue
   while (SDL_PollEvent( &e )) {
      //User requests quit
      if(e.type == SDL_QUIT) {
          quit = true;
      }
   }
      
   // Calculate and correct fps
   float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
   if( avgFPS > 2000000 ) {
      avgFPS = 0;
   }

   // DEBUG DRAW
   //world_.DrawDebugData();

   // Render the background
   background_.render(0, -55);

   // ITERATE THROUGH THE SPRITES AND DRAW THEM
   for (std::vector<Element *>::iterator it = sprites_.begin(); it != sprites_.end();) {
      // Check to see if it's allocated
      if (*it) {
         // Check if it's alive or not
         if (!(*it)->is_alive()) {
            it = sprites_.erase(it);
         } else {
            (*it)->update();
            ++it;
         }
      } else {
         it = sprites_.erase(it);
      }
   }

   // ITERATE THROUGH THE PROJECTILES AND DRAW THEM
   for (std::vector<Projectile *>::iterator it = projectiles_.begin(); it != projectiles_.end();) {
      // Check to see if it's still allocated
      if (*it) {
         // Check if it's alive or not
         if (!(*it)->is_alive()) {
            delete (*it);
            it = projectiles_.erase(it);
         } else {
            (*it)->update();
            ++it;
         }
      } else {
         it = projectiles_.erase(it);
      }
   }

   /* For poop enemy shooting and turning: If you're on the left side of him, turn to left, 
    * if on right side of him, turn to right. For shooting, if center of character within 
    * certain height length of model, shoot. */

   // Update player
   player.update();

   /* 
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

// Setup level 1
void Application::setup_lv1() {
   // Set the platforms up
   ground = new Platform(960, 1050, this);
   
   // Do level platform 1-6
   platforms[0] = new Platform(950, 305 - 55, this);
   platforms[1] = new Platform(300, 505 - 55, this);
   platforms[2] = new Platform(1630, 505 - 55, this);
   platforms[3] = new Platform(1045, 705 - 55, this);
   platforms[4] = new Platform(550, 905 - 55, this);
   platforms[5] = new Platform(1510, 900 - 55, this);

   // Load media for level 1
   if (loadMediaLvl1() == false) {
      quit = true;
      return;
   }

   // Push back ground
   sprites_.push_back(ground);

   // Push back platforms 1-6
   sprites_.push_back(platforms[0]);
   sprites_.push_back(platforms[1]);
   sprites_.push_back(platforms[2]);
   sprites_.push_back(platforms[3]);
   sprites_.push_back(platforms[4]);
   sprites_.push_back(platforms[5]);

   // Push back enemy
   sprites_.push_back(&enemy);
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
    //Free loaded image
    player.idle_texture.free();
    player.running_texture.free();
    player.kick_texture.free();
    player.running_jump_texture.free();
    background_.free();

    // Delete platforms
    for (int i = 0; i < NUM_BLOCKS; i++) {
       platforms[i]->texture.free();
       delete platforms[i];
    }

    //Destroy window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);
    mainWindow = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}
