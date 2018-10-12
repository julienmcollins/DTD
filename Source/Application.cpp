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

#include "Application.h"
#include "Entity.h"
#include "Global.h"

//#define NUM_BLOCKS 11

// Constructs application
Application::Application() : SCREEN_WIDTH(1660), SCREEN_HEIGHT(920), SCREEN_FPS(60), SCREEN_TICKS_PER_FRAME(1000 / SCREEN_FPS), mainWindow(NULL), current_key_states_(NULL), player_(this), mouseButtonPressed(false), quit(false), world_(gravity_), timeStep_(1.0f / 60.0f), velocityIterations_(6), positionIterations_(2) {
    
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

        // Create ground
        gravity_ = {0.0f, -9.81f};
        world_.SetGravity(gravity_);
        platforms[0] = new Platform(0, SCREEN_HEIGHT - 10);
        groundBodyDef_.position.Set(0.0f, -10.0f);
        groundBody_ = world_.CreateBody(&groundBodyDef_);
        groundBox_.SetAsBox(50.0f, 10.0f);
        groundBody_->CreateFixture(&groundBox_, 0.0f);
        
        // Create platforms
        for (int i = 1; i < NUM_BLOCKS; i++) {
            platforms[i] = new Platform(150 * (i - 1), 150 * (i - 1));
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
    
    // Load player
    if (!player_.texture_.loadFromFile("images/man.png")) {
        printf("Failed to load Texture image!\n");
        success = false;
    }

    // Load background 
    if (!background.loadFromFile("images/whitebackground.jpg")) {
        printf("Failed to load Texture image!\n");
        success = false;
    }
    
    // Load ground
    if (!platforms[0]->texture_.loadFromFile("images/thinstrip.png")) {
        printf("Failed to load thinstrip.png\n");
        success = false;
    }

    // Load platform
    for (int i = 1; i < NUM_BLOCKS; i++) {
        if (!platforms[i]->texture_.loadFromFile("images/block1.png")) {
            printf("Failed to load block1.png\n");
            success = false;
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
    int countedFrames = 0;
    fpsTimer.start();

    // Game loop
    while(!quit) {
        // Update world timer
        world_.Step(timeStep_, velocityIterations_, positionIterations_);
        b2Vec2 position = player_.body_->GetPosition();
        float32 angle = player_.body_->GetAngle();
        printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);

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

        // Check collisions all the time
        for (int i = 0; i < NUM_BLOCKS; i++) {
            //check_collision(&player_, platforms[i]);
        }
            
        // Update player
        player_.update();

        // Do gravity
        player_.get_state();

        // Calculate and correct fps
        float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
        if( avgFPS > 2000000 ) {
            avgFPS = 0;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        
        /* Eventually add sprite animations. It'll have to be based on if a character has appeared and stuff like that. Figure it out later. */
        
        // Render background image
        background.render(0, 0);
    
        // Render thin strip
        platforms[0]->texture_.render(0, SCREEN_HEIGHT - 10);

        // Render the texture
        player_.render();

        // Render blocks
        for (int i = 1; i < NUM_BLOCKS; i++) {
            //std::cout << blocks[i].get_x() << " " << blocks[i].get_y() << std::endl;
            platforms[i]->texture_.render(150 * (i - 1), 150 * (i - 1));
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

        // Collide
        for (int i = 0; i < NUM_BLOCKS; i++) {
            //check_collision(&player_, platforms[i]);
        }
    }
}

// check collision between entity and platform
/*
void Application::check_collision(Entity* entity, Platform* platform) {
    // Platform metrics
    int obj_left, obj_right, obj_top, obj_bottom;

    // Entity matrics
    int entity_left, entity_right, entity_top, entity_bottom;
    int entity_next_left, entity_next_right, entity_next_top, entity_next_bottom;

    // Set the objects metrics
    obj_left = platform->get_x();
    obj_right = platform->get_x() + platform->get_width(); 
    obj_top = platform->get_y();
    obj_bottom = platform->get_y() + platform->get_height();

    // Set the entity's metrics
    entity_left = entity->get_x() + 20;
    entity_right = entity->get_x() + entity->get_width() - 20;
    entity_top = entity->get_y() + 15;
    entity_bottom = entity->get_y() + entity->get_height() - 18;

    // Set entity's next metrics
    entity_next_left = entity->get_next_x() + 15;
    entity_next_right = entity->get_next_x() + entity->get_width() - 15;
    entity_next_top = entity->get_next_y() + 15;
    entity_next_bottom = entity->get_next_y() + entity->get_height();
    
    //std::cout << "platform_right = " << obj_right << " platform_left = " << obj_left << " entity_left = " << entity_left << " entity_right = " << entity_right << " entity_top = " << entity_top << " entity_bottom = " << entity_bottom << " obj_top = " << obj_top << " obj_bottom = " << obj_bottom << std::endl;

    // Another option is to have a collision detection box around objects so that anytime
    // an entity enters that area, it gets ready for a collision (could be 2 pixels from
    // either side)

    // If its within the x coords
    bool within_x = (entity_left > obj_left && entity_left < obj_right) ||
                    (entity_right > obj_left && entity_right < obj_right);
    bool collided_with_top = (entity_bottom >= obj_top && entity_top <= obj_top);
   *|| (entity_next_bottom >= obj_top && entity_next_top <= obj_top);*
    bool collided_with_bottom = (entity_top <= obj_bottom && entity_bottom >= obj_bottom);
                 *|| (entity_next_top <= obj_bottom && entity_next_bottom >= obj_bottom);*

    // If its within the y coords
    bool within_y = (entity_bottom < obj_bottom && entity_bottom > obj_top) ||
                    (entity_top < obj_bottom && entity_top > obj_top) || 
                    (entity_bottom >= obj_bottom && entity_top <= obj_top);
    bool collided_with_right = (entity_left <= obj_right && entity_right >= obj_right);
    bool collided_with_left = (entity_right >= obj_left && entity_left <= obj_left);

    // Check collisions
    if (within_y && (collided_with_right || collided_with_left) && (entity->get_center_x() > obj_right || entity->get_center_x() < obj_left)) {
        //std::cout << "In within y " << obj_right << std::endl;
        if (collided_with_left) {
            entity->collision_sides[1] = true;
            //std::cout << "Collided with left side of platform----------------\n";
        } else if (collided_with_right) {
            entity->collision_sides[0] = true;
            //std::cout << "Collided with right side of platform--------------\n";
        }

        // Set platform collision
        entity->collided_platform_ = platform;
    } 
    else if (within_x && (collided_with_top || collided_with_bottom) && (entity->get_center_y() > obj_bottom || entity->get_center_y() < obj_top)) { 
        //std::cout << "In within x " << obj_right << " " << entity_top << std::endl;
        if (collided_with_top) {
            entity->collision_sides[3] = true;
            //std::cout << "Collided with top of platform-----------------------\n";
        }
        else if (collided_with_bottom) {
            entity->collision_sides[2] = true;
            //std::cout << "Collided with bottom of platform--------------------\n";
        }

        // Set platform collision
        entity->collided_platform_ = platform;
    }
    
    // No collision
    if (!entity->collision_sides[0] && !entity->collision_sides[1] && !entity->collision_sides[2] && !entity->collision_sides[3]) {
        //std::cout << "No collision\n";
        entity->collision_sides[4] = true;
    }
}
*/

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
    return platforms[0]->texture_;
}

// Destructs application
Application::~Application() {
    //Free loaded image
    player_.texture_.free();
    background.free();

    //Destroy window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);
    mainWindow = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}
