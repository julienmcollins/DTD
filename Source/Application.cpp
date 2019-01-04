//
//  Application.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <Box2D/Box2D.h>
#include <vector>

#include "Application.h"
#include "Entity.h"
#include "Global.h"

//#define NUM_BLOCKS 11

// Constructs application
Application::Application() : SCREEN_WIDTH(1920.0f), SCREEN_HEIGHT(1080.0f), 
   SCREEN_FPS(20), SCREEN_TICKS_PER_FRAME(1000 / SCREEN_FPS), mainWindow(NULL), 
   current_key_states_(NULL), player(this), mouseButtonPressed(false), quit(false), 
   countedFrames(0), game_flag_(PLAYGROUND), world_(gravity_), to_meters_(0.01f), 
   to_pixels_(100.0f), debugDraw(this), test(0),
   timeStep_(1.0f / 60.0f), velocityIterations_(6), positionIterations_(2), animation_speed_(20.0f), 
   animation_update_time_(1.0f / animation_speed_), time_since_last_frame_(0.0f) {
    
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
        world_.SetDebugDraw(&debugDraw);
        //debugDraw.AppendFlags( b2Draw::e_shapeBit );
        debugDraw.AppendFlags( b2Draw::e_aabbBit );
        //debugDraw.AppendFlags( b2Draw::e_centerOfMassBit );

        // Set background
        background = new Object(0, 0, 0, 0, this);
        background->set_width(0);
        background->set_height(0);

        // Set the platforms up
        ground = new Platform(0, SCREEN_HEIGHT - 50, this);//SCREEN_HEIGHT - 10);
        
        // Create platforms
        for (int i = 0; i < NUM_BLOCKS; i++) {
           platforms_[i] = new Platform(i * 150, i * 150, this);
        }
    }
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
    if (!player.idle_texture.loadFromFile("images/idle_na.png")) {
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
    if (!player.running_texture.loadFromFile("images/running_na.png")) {
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
    if (!player.kick_texture.loadFromFile("images/kick.png")) {
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
    if (!player.running_jump_texture.loadFromFile("images/running_jump_na.png")) {
       printf("Failed to load Running Jump image!\n");
       success = false;
    } else {
       // Allocate enough room for the clips
       player.running_jump_texture.clips_ = new SDL_Rect[17];
       SDL_Rect *temp = player.running_jump_texture.clips_;

       // Calculate the locations
       for (int i = 0; i < 17; i++) {
          temp[i].x = i * 80;
          temp[i].y = 0;
          temp[i].w = 80;
          temp[i].h = 150;
       }
    }

    // Turn animation width 52 --> turns from facing right to left

    // Load arm
    if (!player.arm_texture.loadFromFile("images/idle_arm_na.png")) {
       printf("Failed to load Arm image!\n");
       success = false;
    }

    // Load shooting arm
    if (!player.arm_shoot_texture.loadFromFile("images/arm.png")) {
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
    if (!player.arm_running_texture.loadFromFile("images/running_arm.png")) {
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
    if (!player.eraser_texture.loadFromFile("images/eraser.png")) {
       printf("Failed to load eraser texture!\n");
       success = false;
    }

    // Load background 
    if (!background->texture.loadFromFile("images/whitebackground.jpg")) {
        printf("Failed to load Texture image!\n");
        success = false;
    } else {
       sprites_.push_back(background);
    }
    
    // Load ground
    if (!ground->texture.loadFromFile("images/floor.png")) {
        printf("Failed to load thinstrip.png\n");
        success = false;
    } else {
        float x_ground = 19.20f / 2.0f;
        float y_ground = 10.55f - 0.6f; 
        ground->body_def.position.Set(x_ground, -y_ground);
        ground->body = world_.CreateBody(&ground->body_def);
        ground->box.SetAsBox(9.6f - 0.01f, 0.25f - 0.01f);
        ground->body->CreateFixture(&ground->box, 0.0f);
        //x_ground = (ground->body->GetPosition().x - 19.20f / 2.0f) * to_pixels_;
        //y_ground = -(ground->body->GetPosition().y + 0.25f) * to_pixels_; 
        //ground->set_x(x_ground);
        //ground->set_y(y_ground);
        ground->set_height(ground->texture.getHeight());
        ground->set_width(ground->texture.getWidth());
        sprites_.push_back(ground);
    }

    // Load platform
    for (int i = 0; i < NUM_BLOCKS; i++) {
       if (!platforms_[i]->texture.loadFromFile("images/block1.png")) {
           printf("Failed to load block1.png\n");
           success = false;
       } else {
           platforms_[i]->body_def.position.Set(i * 1.50f + 0.5f, -(i * 1.50f) - 0.5f);
           platforms_[i]->body = world_.CreateBody(&platforms_[i]->body_def);
           platforms_[i]->box.SetAsBox(0.5f - 0.01f, 0.5f - 0.01f);
           platforms_[i]->body->CreateFixture(&platforms_[i]->box, 0.0f);
           //float x_block = ((platforms_[i]->body->GetPosition().x - 0.5f) * to_pixels_);
           //float y_block = (-(platforms_[i]->body->GetPosition().y + 0.5f) * to_pixels_);
           //platforms_[i]->set_x(x_block);
           //platforms_[i]->set_y(y_block);
           //printf("x = %f, y = %f\n", (100 * platforms_[i]->body->GetPosition().x) - (platforms_[i]->get_width() / 2),
           //      platforms_[i]->body->GetPosition().y);
           //printf("x = %d, y = %d\n", platforms_[i]->get_x(), platforms_[i]->get_y());
           platforms_[i]->set_height(100);
           platforms_[i]->set_width(100);
           printf("width = %d\n", platforms_[i]->texture.getWidth());
           sprites_.push_back(platforms_[i]);
       }
    }

    // Return state
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

    // Start counting frames per second
    fpsTimer.start();

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

   // Clear screen
   SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
   SDL_RenderClear(renderer);
}

// PLAYGROUND FUNCTION
void Application::playground() {
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

   // ITERATE THROUGH THE SPRITES AND DRAW THEM
   for (std::vector<Element *>::iterator it = sprites_.begin(); it != sprites_.end(); ++it) {
      (*it)->texture.render((*it)->get_x(), (*it)->get_y());
      //printf("x = %d, y = %d\n", (*it)->get_x(), (*it)->get_y());
   }

   /* For poop enemy shooting and turning: If you're on the left side of him, turn to left, 
    * if on right side of him, turn to right. For shooting, if center of character within 
    * certain height length of model, shoot. */

   // Update player
   player.update();

   // Render player
   Texture *playertexture = player.get_texture();
   SDL_Rect *curr_clip = player.get_curr_clip();
   if (curr_clip) {
     // Render player
     player.render(playertexture, curr_clip);

     // Render arm if idle, render shooting if not
     if (!player.shooting) {
         if (player.get_player_state() == 1) {
            player.arm_running_texture.render(player.get_x() + player.get_width() +
               player.arm_delta_x, player.get_y() + player.arm_delta_y,
               player.arm_running_texture.curr_clip_, 0.0,
               &player.arm_running_texture.center_, player.arm_running_texture.flip_);
         } else {
            player.arm_texture.render(player.get_x() + player.get_width() + 
               player.arm_delta_x, player.get_y() + player.arm_delta_y, NULL, 0.0, 
               &player.arm_texture.center_, player.arm_texture.flip_);
         }
     } else {
         player.arm_shoot_texture.render(player.get_x() + player.get_width() + 
            player.arm_delta_shoot_x, player.get_y() + player.arm_delta_shoot_y, 
            player.arm_shoot_texture.curr_clip_, 0.0, 
            &player.arm_shoot_texture.center_, player.arm_shoot_texture.flip_);
     }
   }

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
    background->texture.free();

    // Delete platforms
    for (int i = 0; i < NUM_BLOCKS; i++) {
       platforms_[i]->texture.free();
       delete platforms_[i];
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
