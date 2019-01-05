//
//  Application.h
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#ifndef Application_h
#define Application_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <Box2D/Box2D.h>
#include <vector>

#include "Element.h"
#include "Texture.h"
#include "Entity.h"
#include "Timer.h"
#include "Object.h"
#include "DebugDraw.h"

#define NUM_BLOCKS 6

// For returning the screen position
typedef struct {
   int x, y;
} Screen;

class Application {
    public:
        // Initialize the application
        Application();
    
        // Initialize SDL
        bool init();
    
        // Load Media
        bool loadMedia();
    
        // Update the application
        void update();

        // The main window of the application
        SDL_Window* mainWindow;
        
        // Load textures (now redundant)
        SDL_Texture* loadTexture(std::string path);
        
        // Application keyboard state
        const Uint8* current_key_states_;

        // Handles events
        SDL_Event e;

        // Set viewport
        void setViewport();
        
        // Flag for mouse button pressed
        bool mouseButtonPressed;

        // Get screen height and width
        const int get_height();
        const int get_width();

        // Get ground
        Texture get_ground();
        
        // Public world object so that other entities can access it
        b2World world_;

        // Need a scaling factor since Box2D doesn't work with pixels
        float to_pixels_;
        float to_meters_;

        // DebugDraw
        DebugDraw debugDraw;
        SDL_Rect r[7];

        // Animation speed
        float animation_speed_;
        float animation_update_time_;
        float time_since_last_frame_;
        int test;

        // Get FPS timer
        Timer getFPSTimer() const {
           return fpsTimer;
        }

        // Return the vector
        std::vector<Element *> *getObjectVector() {
           return &sprites_;
        }

        // Destructrs the application
        ~Application();
    
    private:
        // Screen Dimensions
        int SCREEN_WIDTH;
        int SCREEN_HEIGHT;

        // Frame rate capping    
        const int SCREEN_FPS;
        const int SCREEN_TICKS_PER_FRAME;

        // FPS timers
        Timer fpsTimer;
        Timer capTimer;

        // Viewport
        SDL_Rect viewport;
    
        // Player object
        Player player;

        // Background texture
        Object *background;

        /***** Box2D Variables *****/
        b2Vec2 gravity_;

        // Timestep for the engine
        float32 timeStep_;
        int32 velocityIterations_;
        int32 positionIterations_;

        /***************************/

        // Ground
        Platform* ground;

        // Platforms
        Platform* platforms[NUM_BLOCKS];

        /********* APPLICATION STATE -- CRITICAL *************/
        // Fps counter
        int countedFrames;

        // Two flags for now, update as levels increase
        enum APP_STATE {
           MAIN_SCREEN,
           PLAYGROUND
        };

        // MAIN SCREEN FUNCTION
        void main_screen();

        // PLAYGROUND FUNCTION
        void playground();

        // Setup level 1
        void setup_lv1();
        bool lv1_flag;

        // GAME FLAG
        APP_STATE game_flag_;

        // TEXTURE VECTOR
        std::vector<Element *> sprites_;        
        /*****************************************************/
    
        // Quit flag for application
        bool quit;
};

#endif /* Application_h */
