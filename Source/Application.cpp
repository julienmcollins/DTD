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
Application::Application() : SCREEN_WIDTH(1920.0f), SCREEN_HEIGHT(1080.0f), SCREEN_FPS(60), SCREEN_TICKS_PER_FRAME(1000 / SCREEN_FPS), mainWindow(NULL), current_key_states_(NULL), player_(this), mouseButtonPressed(false), quit(false), world_(gravity_), to_meters_(1.0/100.0), to_pixels_(100.0), timeStep_(1.0f / 60.0f), velocityIterations_(6), positionIterations_(2) {
    
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

        // Set the platforms up
        ground = new Platform(0, SCREEN_HEIGHT - 50);//SCREEN_HEIGHT - 10);
        ground->BodyDef_.position.Set(0.0f,
                     (SCREEN_HEIGHT - (SCREEN_HEIGHT - 50.0f)) * to_meters_);
                     //(SCREEN_HEIGHT - (SCREEN_HEIGHT - 4 * (0.05f * 2 * to_pixels_))) * to_meters_);
        ground->Body_ = world_.CreateBody(&ground->BodyDef_);
        ground->Box_.SetAsBox(19.2f, 0.25f);
        ground->Body_->CreateFixture(&ground->Box_, 0.0f);
        
        // Create platforms
        for (int i = 0; i < NUM_BLOCKS; i++) {
            platforms[i] = new Platform(150 * i, 150 * i);
            //ground->BodyDef_.position.Set(0.0f, -0.2f);
            //ground->Body_ = world_.CreateBody(&ground->BodyDef_);
            //ground->Box_.SetAsBox((100.0 / 100.0) / 2, (100.0 / 100.0) / 2);
            //ground->Body_->CreateFixture(&ground->Box_, 0.0f);

        }

        // Test last platform
        platforms[NUM_BLOCKS - 1]->
           BodyDef_.position.Set((150 * (NUM_BLOCKS - 1)) * to_meters_,
                                 (SCREEN_HEIGHT - (150.0 * (NUM_BLOCKS - 1)) - 25.0f) * to_meters_);
        platforms[NUM_BLOCKS - 1]->Body_ = world_.CreateBody(&platforms[NUM_BLOCKS - 1]->BodyDef_);
        platforms[NUM_BLOCKS - 1]->Box_.SetAsBox(0.5f, 0.5f);
        platforms[NUM_BLOCKS - 1]->Body_->CreateFixture(&platforms[NUM_BLOCKS - 1]->Box_, 0.0f);
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
    if (!ground->texture_.loadFromFile("images/floor.png")) {
        printf("Failed to load thinstrip.png\n");
        success = false;
    }

    // Load platform
    for (int i = 0; i < NUM_BLOCKS; i++) {
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
        b2Vec2 velocity = player_.body_->GetLinearVelocity();
        float32 angle = player_.body_->GetAngle();
        printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
        //printf("%d %d\n", player_.get_x(), player_.get_y());

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
            
        // Update player
        player_.update();

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
        ground->texture_.render(0, SCREEN_HEIGHT - 50);

        // Render the texture
        player_.render();

        // Render blocks
        for (int i = 0; i < NUM_BLOCKS; i++) {
            //std::cout << blocks[i].get_x() << " " << blocks[i].get_y() << std::endl;
            platforms[i]->texture_.render(150 * i, 150 * i);
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
    return ground->texture_;
}

// Destructs application
Application::~Application() {
    //Free loaded image
    player_.texture_.free();
    background.free();

    // Delete platforms
    for (int i = 0; i < NUM_BLOCKS; i++) {
       platforms[i]->texture_.free();
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
