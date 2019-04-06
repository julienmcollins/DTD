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
#include <unordered_map>

#include "Element.h"
#include "Texture.h"
#include "Entity.h"
#include "Timer.h"
#include "Object.h"
#include "DebugDraw.h"
#include "ContactListener.h"

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

        // Load Level 1
        bool loadMediaLvl1();
    
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
        Timer* getFPSTimer() {
           return &fpsTimer;
        }

        // Return the vector
        std::vector<Element *> *getObjectVector() {
           return &sprites_;
        }

        std::vector<Projectile *> *getProjectileVector() {
           return &projectiles_;
        }

        Player* get_player() {
           return &player;
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

        // Enemy object
        Enemy enemy;

        /***** Box2D Variables *****/
        b2Vec2 gravity_;

        // Timestep for the engine
        float32 timeStep_;
        int32 velocityIterations_;
        int32 positionIterations_;

        // Contact listener instance
        ContactListener contact_listener_;
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
           GAMEOVER_SCREEN,
           PLAYGROUND
        };

        // Menu Items
        enum MENU {
           START = 665,
           OPTIONS = 780,
           EGGS = 880
        };

        // ANIMATE FUNCTION
        void animate(const float &fps, Element* element, 
              Texture *texture, Timer &timer, float &last_frame);

        // MAIN SCREEN FUNCTION
        void main_screen();

        // GAMEOVER FUNCTION
        void gameover_screen();

        // Finger
        /*
        Texture finger_point_;
        Texture finger_shake_;
        Timer finger_point_timer_;
        Timer finger_shake_timer_;
        float finger_last_frame_;
        */
        Element finger_;
        int item_;

        // Clicked flag
        bool clicked;

        // Background
        /*
        Texture title_screen_;
        Texture title_;
        Texture menu_;
        float menu_last_frame_;
        float background_last_frame_;
        float title_last_frame_;
        Timer background_timer_;
        Timer menu_timer_;
        Timer title_timer_;
        */
        Element menu_background_;
        Element menu_title_;
        Element menu_items_;

        // Gameove texture
        Texture gameover_screen_;

        // Background texture
        Texture background_;

        // Setup main menu
        void setup_menu();
        bool menu_flag;

        // PLAYGROUND FUNCTION
        void playground();

        // Setup level 1
        void setup_lv1();
        bool lv1_flag;

        // GAME FLAG
        APP_STATE game_flag_;

        // TEXTURE VECTORS (Might be able to combine the two, since the objects update themselves)
        std::vector<Element *> sprites_;        
        std::vector<Projectile *> projectiles_;
        /*****************************************************/
    
        // Quit flag for application
        bool quit;
};

#endif /* Application_h */
