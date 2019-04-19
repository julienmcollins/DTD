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

class Level;
class Enemy;

// For returning the screen position
typedef struct {
   int x, y;
} Coords;

// Finger class because it's easier
class Finger : public Element {
   public:
      // Constructor
      Finger(Application *application);

      // Get texture
      virtual Texture *get_texture();

      // Update function
      virtual void update();

      // FINGER STATE
      enum FINGER_STATE {
         POINT,
         SHAKE   
      };  

      // Finger state
      FINGER_STATE finger_state;
};

class Application {
   public:
      // Initialize the application
      Application();
   
      // Initialize SDL
      bool init();
   
      // Load Media
      bool loadMedia();

      // Load Level media
      //bool loadMediaLvl1();
      bool loadMediaLvl3();

      // Load media for enemy
      bool loadMediaEnemy(Enemy *enemy);
   
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

      // SET QUIT TO TRUE
      void set_quit() {
         quit = true;
      }

      // SET COMPLETED LEVEL
      void set_completed_level() {
         completed_level_ = true;
      }

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

      /********** LEVEL **************/
      Level *level;

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
      Enemy *enemy;

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

      /********* APPLICATION STATE -- CRITICAL *************/
      // Fps counter
      int countedFrames;

      // Two flags for now, update as levels increase
      enum APP_STATE {
         MAIN_SCREEN,
         GAMEOVER_SCREEN,
         PLAYGROUND
      };

      // Levels
      enum LEVELS {
         LEVEL11,
         LEVEL12,
         LEVEL13,
         LEVEL14,
         LEVEL15   
      };

      // Menu Items
      enum MENU {
         START = 665,
         OPTIONS = 780,
         EGGS = 880,
         WORLD1 = 645,
         WORLD2 = 715,
         WORLD3 = 800,
         WORLD4 = 865,
         WORLD5 = 925
      };

      // Menu
      enum MENU_SCREENS {
         FIRST,
         SECOND,
         THIRD
      };

      // GAME STATE
      enum GAME_STATE {
         SETUP,
         PLAY
      };

      // ANIMATE FUNCTION
      void animate(const float &fps, Element* element, 
            Texture *texture, Timer &timer, float &last_frame);

      // MAIN SCREEN FUNCTION
      void main_screen();

      // GAMEOVER FUNCTION
      void gameover_screen();

      // Finger
      Finger finger_;
      bool point_;
      int item_;

      // Clicked flag
      bool clicked;

      /**** MAIN MENU ********/
      Element menu_background_;
      Element menu_title_;
      Element menu_items_;
      Element world_items_;
      Platform *menu_platform_;
      /***********************/

      // Gameove texture
      Texture gameover_screen_;

      // Setup main menu
      void setup_menu();
      bool menu_flag;

      // PLAYGROUND FUNCTION
      void playground();

      // GAME FLAG
      APP_STATE app_flag_;
      MENU_SCREENS menu_screen_;
      LEVELS level_flag_;
      GAME_STATE game_flag_;
      bool completed_level_;

      // VECTORS (Might be able to combine the two, since the objects update themselves)
      std::vector<Platform *> platforms;
      std::vector<Element *> sprites_;        
      std::vector<Projectile *> projectiles_;
      std::unordered_map<int, Texture> background;
      /*****************************************************/
   
      // Quit flag for application
      bool quit;
};

#endif /* Application_h */
