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

#include "OpenGLIncludes.h"

#include <Box2D/Box2D.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <memory>

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/DebugDraw.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Element.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Object.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Timers/FPSTimer.h"
#include "QuiteGoodMachine/Source/GameManager/Private/ContactListener.h"
#include "QuiteGoodMachine/Source/GameManager/Private/KeyHandler.h"

#include "QuiteGoodMachine/Source/HUD/Private/HUD.h"
#include "QuiteGoodMachine/Source/HUD/Private/HUDElement.h"

#define NUM_BLOCKS 6

class Level;
class Enemy;
class DevelZoom;
class Notebook;
class Animation;

// For returning the screen position
typedef struct {
   int x, y;
} Coords;

// Finger class because it's easier
class Finger : public Element {
   public:
      // Constructor
      Finger();

      // Update function
      virtual void Update();

      // FINGER STATE
      enum FINGER_STATE {
         POINT,
         SHAKE   
      };  

      // Load media
      virtual bool LoadMedia();

      // Get animation from state
      virtual Animation *GetAnimationFromState();

      // Finger state
      FINGER_STATE finger_state;

      // Updating flag
      bool updating;
};

class Application {
   public:
      // Get instance
      static Application& GetInstance() {
         static Application instance;
         return instance;
      }

      // Initialize external libraries
      bool Init();

      // Load Media
      bool LoadMedia();

      // Paths to media
      static std::string sprite_path;
      static std::string audio_path;
   
      // Update the application
      void Update();
      void update_projectiles();

      // Render cloud layer
      void RenderCloudLayer();

      // Draw everything
      void Draw();

      /*********** SDL WINDOW STUFF ***************/

      // The main window of the application
      SDL_Window* main_window;
            
      // Application keyboard state
      const Uint8* current_key_states_;

      // Handles events
      SDL_Event e;

      KeyHandler key_handler;

      /*********************************************/

      /************* OPENGL STUFF ******************/
      // Initialze OpenGL
      bool InitOpenGL();
      /*********************************************/
      
      // Flag for mouse button pressed
      bool mouseButtonPressed;

      // Get screen height and width
      const float get_height();
      const float get_width();
      
      // Public world object so that other entities can access it
      b2World world_;

      // Need a scaling factor since Box2D doesn't work with pixels
      float to_pixels_;
      float to_meters_;

      // DebugDraw
      DebugDraw debugDraw;
      GLFloatRect r[15];

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
      FPSTimer* getFPSTimer() {
         return &fpsTimer;
      }

      // Return the vector
      std::vector<Element *> *getObjectVector() {
         return &sprites_;
      }

      std::vector<Projectile *> *getProjectileVector() {
         return &projectiles_;
      }

      std::shared_ptr<Player> get_player() {
         return player;
      }

      /********** LEVEL **************/
      Level *level;

      Level *get_level() {
         return level;
      }

      /******* DEATH TIMER ***********/
      FPSTimer death_timer_;

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

      // Screen Dimensions
      float SCREEN_WIDTH;
      float SCREEN_HEIGHT;

      // Destructrs the application
      ~Application();
   
   private:
      // Initialize the application
      Application();

      //OpenGL context
      SDL_GLContext gl_context_;
      unsigned int gl_program_id_;

      // Frame rate capping    
      const int SCREEN_FPS;
      const int SCREEN_TICKS_PER_FRAME;

      // Testing flag
      int pause;

      // FPS timers
      FPSTimer fpsTimer;
      FPSTimer capTimer;

      // Viewport
      GLFloatRect viewport;
   
      // Player object
      std::shared_ptr<Player> player;

      // HUD
      std::unique_ptr<HUD> hud;

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
   
      // Zoom
      DevelZoom *zoom_;

      /***** NOTEBOOK *************/
      Element notebook_background_;
      Notebook *notebook_;

      /***** CLOUD LAYER **********/
      Element cloud_layer_;
      float cloud_pos_;
      FPSTimer cloud_timer_;
      float cloud_last_frame_;
      float cloud_fps_;

      // Quit flag for application
      bool quit;

   public:
      // More singleton stuff
      Application(Application const&) = delete;
      void operator=(Application const&) = delete;
};

#endif /* Application_h */
