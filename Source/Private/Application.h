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
#include <SDL2/SDL_mixer.h>
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
      Finger();

      // Get texture
      virtual Texture *get_texture();

      // Update function
      virtual void update();

      // FINGER STATE
      enum FINGER_STATE {
         POINT,
         SHAKE   
      };  

      // Load media
      virtual bool load_media();

      // Finger state
      FINGER_STATE finger_state;

      // Updating flag
      bool updating;
};

class Application {
   public:
      // Get instance
      static Application& get_instance() {
         static Application instance;
         return instance;
      }

      // Initialize SDL
      bool init();
   
      // Load Media
      bool loadMedia();

      // Paths to media
      static const std::string sprite_path;
      static const std::string audio_path;
   
      // Update the application
      void update();
      void update_projectiles();

      /*********** SDL WINDOW STUFF ***************/

      // The main window of the application
      SDL_Window* mainWindow;
      
      // Load textures (now redundant)
      SDL_Texture* loadTexture(std::string path);
      
      // Application keyboard state
      const Uint8* current_key_states_;

      // Handles events
      SDL_Event e;

      /*********************************************/

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
      SDL_Rect r[15];

      // Animation speed
      float animation_speed_;
      float animation_update_time_;
      float time_since_last_frame_;
      int test;

      // SET QUIT TO TRUE
      void set_quit() {
         quit = true;
      }

      // GAME STATE
      enum GAME_STATE {
         SETUP,
         PLAY
      };

      // Levels
      enum FOREST {
         FOREST1 = 1,
         FOREST2 = 2,
         FOREST3 = 3,
         FOREST4 = 4,
         FOREST5 = 5,
         FOREST6 = 6,
         FOREST7 = 7,
         FOREST8 = 8,
         FOREST9 = 9,
         FORESTBOSS = 10,
         TEMP = 11
      };

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
         return player;
      }

      /********** LEVEL **************/
      Level *level;

      Level *get_level() {
         return level;
      }

      /******* DEATH TIMER ***********/
      Timer death_timer_;

      // Set game flag
      void set_game_flag(GAME_STATE new_flag) {
         game_flag_ = new_flag;
      }

      // Get and Set level flag
      FOREST get_level_flag() {
         return level_flag_;
      }
      void set_level_flag(FOREST new_level) {
         level_flag_ = new_level;
      }

      // Destructrs the application
      ~Application();
   
   private:
      // Initialize the application
      Application();

      // // Singleton stuff
      // Application(Application const&);
      // void operator=(Application const&);

      // Screen Dimensions
      int SCREEN_WIDTH;
      int SCREEN_HEIGHT;

      // Frame rate capping    
      const int SCREEN_FPS;
      const int SCREEN_TICKS_PER_FRAME;

      // Testing flag
      int pause;

      // FPS timers
      Timer fpsTimer;
      Timer capTimer;

      // Viewport
      SDL_Rect viewport;
   
      // Player object
      Player *player;

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
         THANKS,
         PLAYGROUND
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

      // ANIMATE FUNCTION
      void animate(const float &fps, Element* element, 
            Texture *texture, Timer &timer, float &last_frame);

      // MAIN SCREEN FUNCTION
      void main_screen();

      // GAMEOVER FUNCTION
      void gameover_screen();

      // Finger
      Finger *finger_;
      bool point_;
      int item_;

      // Clicked flag
      bool clicked;

      /**** MAIN MENU ********/
      Element menu_background_;
      Element menu_title_;
      Element menu_items_;
      Element world_items_;
      Element ruler_;
      Platform *menu_platform_;
      Platform *invisible_wall_;
      /***********************/

      // String for background
      std::string final_background_;

      /***** GAMEOVER *********/
      Element gameover_screen_;
      /************************/

      /***** THANKS ***********/
      Element thanks_screen_;
      /************************/

      /****** MUSIC **********/
      Mix_Music *music; 

      // Setup main menu
      void setup_menu();
      bool menu_flag;

      // PLAYGROUND FUNCTION
      void playground();

      // GAME FLAG
      APP_STATE app_flag_;
      MENU_SCREENS menu_screen_;
      FOREST level_flag_;
      GAME_STATE game_flag_;

      // VECTORS (Might be able to combine the two, since the objects update themselves)
      std::vector<Platform *> platforms;
      std::vector<Element *> sprites_;        
      std::vector<Projectile *> projectiles_;
      std::unordered_map<int, Texture> background;
      /*****************************************************/
   
      // Quit flag for application
      bool quit;

   public:
      // More singleton stuff
      Application(Application const&) = delete;
      void operator=(Application const&) = delete;
};

#endif /* Application_h */
