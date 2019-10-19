//
//  Application.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include "Source/GameEngine/Private/Application.h"
#include "Source/GameEngine/Private/Level.h"

#include "Source/ObjectManager/Private/Entity.h"
#include "Source/ObjectManager/Private/Global.h"
#include "Source/ObjectManager/Private/Enemy.h"

#include "Source/RenderingEngine/Private/RenderingEngine.h"
#include "Source/RenderingEngine/Private/Texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <Box2D/Box2D.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cmath>
#include <tuple>

//Shader loading utility programs
void printProgramLog( unsigned int program );
void printShaderLog( unsigned int shader );

// Initialize paths
const std::string Application::sprite_path = "Media/Sprites/";
const std::string Application::audio_path = "Media/Audio/";

// Constructs application
Application::Application() : SCREEN_WIDTH(1920.0f), SCREEN_HEIGHT(1080.0f), 
   SCREEN_FPS(60), SCREEN_TICKS_PER_FRAME(1000 / SCREEN_FPS), pause(-1), main_window(NULL), 
   current_key_states_(NULL), mouseButtonPressed(false), quit(false), 
   countedFrames(0), menu_flag(true),
   app_flag_(MAIN_SCREEN),
   menu_screen_(FIRST),
   game_flag_(SETUP),
   world_(gravity_), to_meters_(0.01f), to_pixels_(100.0f), test(0),
   timeStep_(1.0f / 60.0f), velocityIterations_(10), positionIterations_(10), animation_speed_(20.0f), 
   animation_update_time_(1.0f / animation_speed_), time_since_last_frame_(0.0f), 
   menu_background_(0, 0, 1080, 1920),
   menu_title_(640, 70, 513, 646),
   menu_items_(800, 650, 299, 321),
   world_items_(850, 650, 332, 193),
   ruler_(200, 722, 200, 50),
   gameover_screen_(0, 0, 1080, 1920),
   thanks_screen_(0, 0, 1080, 1920) {
    
   //Initialize SDL
   if (!Init()) {
      std::cout << "Application::Application() - Failed to initialize application\n";
      exit(0);
   }

   //Initialize OpenGL
   if( !InitOpenGL() )
   {
      printf( "Unable to initialize OpenGL!\n" );
      exit(0);
   }

   // Set gravity up
   gravity_ = {0.0f, -11.81f};
   world_.SetGravity(gravity_);

   // Set up debug drawer
   world_.SetDebugDraw(&debugDraw);

   //debugDraw.AppendFlags( b2Draw::e_shapeBit );
   debugDraw.AppendFlags( b2Draw::e_aabbBit );
   //debugDraw.AppendFlags( b2Draw::e_centerOfMassBit );

   // Set contact listener
   world_.SetContactListener(&contact_listener_);

   // Start counting frames per second
   fpsTimer.start();
}

// Initialize OpenGL
bool Application::InitOpenGL() {
   // Load shaders, and viewport
   RenderingEngine::get_instance().LoadShader("texture_shader");

   // Load perspective matrix
   glm::mat4 projection = glm::ortho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);
   RenderingEngine::get_instance().LoadApplicationPerspective(projection);

   // Blending?
   glEnable(GL_TEXTURE_2D);
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   return true;
}

// Checks initialization of SDL functions
bool Application::Init() {
   // Success flag 
   bool success = true;
    
   // Open audio first
   Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);

   //Initialize SDL
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
      printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
      success = false;
   } else {
      //Use OpenGL 3.3 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

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

   // Now initialize window
   SDL_DisplayMode DM;
   SDL_GetCurrentDisplayMode(0, &DM);
   SCREEN_WIDTH = DM.w;
   SCREEN_HEIGHT = DM.h;
   
   // Creates window
   main_window = SDL_CreateWindow("Doodle 'Till Death", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
   if (main_window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
      success = false;
   }

   //Create context
   gl_context_ = SDL_GL_CreateContext( main_window );
   if (gl_context_ == NULL) {
      printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
      success = false;
   }

   //Initialize GLEW
   glewExperimental = GL_TRUE; 
   GLenum glewError = glewInit();
   if (glewError != GLEW_OK) {
      printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
      success = false;
   }

   //Use Vsync
   if (SDL_GL_SetSwapInterval(1) < 0) {
      printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
      success = false;
   }

   return success;
}

void printProgramLog(unsigned int program) {
	//Make sure name is shader
	if( glIsProgram( program ) )
	{
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		
		//Get info string length
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );
		
		//Allocate string
		char* infoLog = new char[ maxLength ];
		
		//Get info log
		glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
			printf( "%s\n", infoLog );
		}
		
		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf( "Name %d is not a program\n", program );
	}
}

void printShaderLog(unsigned int shader) {
	//Make sure name is shader
	if( glIsShader( shader ) )
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		
		//Get info string length
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
		
		//Allocate string
		char* infoLog = new char[ maxLength ];
		
		//Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
			printf( "%s\n", infoLog );
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf( "Name %d is not a shader\n", shader );
	}
}

// Loads images and other media
// TODO: Move LoadMedia function to constructor of each object --> like level
bool Application::LoadMedia() {
   // Start success = true 
   bool success = true;

   /************** MAIN MENU STUFF *********************************/
   // Instantiate data
   std::vector<TextureData> menu_data;
   menu_data.push_back(TextureData(3, 1.0f / 4.0f, "cloud", sprite_path + "Miscealaneous/cloudscreen.png"));
   menu_data.push_back(TextureData(3, 1.0f / 4.0f, "forest", sprite_path + "Miscealaneous/forestscreen.png"));

   // Load resources
   success = RenderingEngine::get_instance().LoadResources(&menu_background_, menu_data);

   // Load title
   std::string path = sprite_path + "Miscealaneous/title.png";
   menu_title_.texture.frame_num = 3;
   if (!menu_title_.texture.LoadFromFile(path.c_str(), true)) {
      printf("Failed to load title.png\n");
      success = false;
   } else {
      menu_title_.texture.clips_ = new GLFloatRect[3];
      GLFloatRect *temp = menu_title_.texture.clips_;

      for (int i = 0; i < 3; i++) {
         temp[i].x = i * 646.0f / 1938.0f;
         temp[i].y = 0;
         temp[i].w = 646.0f / 1938.0f;
         temp[i].h = 513.0f / 513.0f;
      }

      // Set curr clip
      menu_title_.texture.curr_clip_ = &temp[0];
   }

   // Load menu
   path = sprite_path + "Miscealaneous/menu.png";
   menu_items_.texture.frame_num = 3;
   if (!menu_items_.texture.LoadFromFile(path.c_str(), true)) {
      printf("Failed to load menu.png\n");
      success = false;
   } else {
      // Allocate room
      menu_items_.texture.clips_ = new GLFloatRect[3];
      GLFloatRect *temp = menu_items_.texture.clips_;

      // Allocate enough room
      for (int i = 0; i < 3; i++) {
         temp[i].x = i * 321.0f / 963.0f;
         temp[i].y = 0;
         temp[i].w = 321.0f / 963.0f;
         temp[i].h = 299.0f / 299.0f;
      }

      // Set curr clip
      menu_items_.texture.curr_clip_ = &temp[0];
   }

   // Load world items
   path = sprite_path + "Miscealaneous/worlds.png";
   world_items_.texture.frame_num = 3;
   if (!world_items_.texture.LoadFromFile(path.c_str(), true)) {
      printf("Failed to load worlds.png\n");
      success = false;
   } else {
      // Allocate room
      world_items_.texture.clips_ = new GLFloatRect[3];
      GLFloatRect *temp = world_items_.texture.clips_;

      // Allocate enough room
      for (int i = 0; i < 3; i++) {
         temp[i].x = i * 193.0f / 579.0f;
         temp[i].y = 0;
         temp[i].w = 193.0f / 579.0f;
         temp[i].h = 332.0f / 332.0f;
      }

      // Set curr clip
      world_items_.texture.curr_clip_ = &temp[0];
   }

   // Gameover screen
   path = sprite_path + "Miscealaneous/gameover.png";
   gameover_screen_.texture.frame_num = 3;
   if (!gameover_screen_.texture.LoadFromFile(path.c_str(), true)) {
      printf("Failed to load gameover.png\n");
      success = false;
   } else {
      // Allocate room
      gameover_screen_.texture.clips_ = new GLFloatRect[3];
      GLFloatRect *temp = gameover_screen_.texture.clips_;

      // Allocate enough room
      for (int i = 0; i < 3; i++) {
         temp[i].x = i * 1920.0f / 5760.0f;
         temp[i].y = 0;
         temp[i].w = 1920.0f / 5760.0f;
         temp[i].h = 1080.0f / 1080.0f;
      }

      // Set curr clip
      gameover_screen_.texture.curr_clip_ = &temp[0];

      // Set fps
      gameover_screen_.texture.fps = 1.0f / 3.0f;
      gameover_screen_.texture.max_frame_ = 2;
   }

   // Load thanks
   path = sprite_path + "Miscealaneous/test.png";
   if (!thanks_screen_.texture.LoadFromFile(path.c_str(), true)) {
      printf("Failed to load thanks.png\n");
      success = false;
   }

   path = sprite_path + "Miscealaneous/ruler_200.png";
   if (!ruler_.texture.LoadFromFile(path.c_str(), true)) {
      printf("Failed to load ruler.png\n");
      success = false;
   }
      
   // Return state
   return success;
}

// Setup main menu
void Application::setup_menu() {
   // Create player
   player = new Player();
   if (player->LoadMedia() == false) {
      quit = true;
   }

   // Create finger
   finger_ = new Finger();
   if (finger_->LoadMedia() == false) {
      quit = true;
   }
   point_ = false;
   item_ = false;
   clicked = false;

   // Set player location
   player->set_x(50);
   player->set_y(782);

   // Setup platform
   menu_platform_ = new Platform(960, 925, 10, 1920);
   menu_platform_->setup();

   // Setup invisible wall
   invisible_wall_ = new Platform(0, 1055, 1000, 10);
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

// Updates the screen
void Application::update() {
   // Game loop
   while (!quit) {
      // Clear screen at every instance
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear( GL_COLOR_BUFFER_BIT );

      // Update world timer
      world_.Step(timeStep_, velocityIterations_, positionIterations_);
      world_.ClearForces();

      // Start cap timer
      capTimer.start();

      // Calculate and correct fps
      float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
      if( avgFPS > 2000000 ) {
         avgFPS = 0;
      }
      
      // Update levels independently
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

         // Calculate and correct fps
         float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
         if( avgFPS > 2000000 ) {
            avgFPS = 0;
         }

         // Draw title screen
         thanks_screen_.Render(&thanks_screen_.texture);

         // Update the screen
         ++countedFrames;

         // If frame finished early
         int frameTicks = capTimer.getTicks();
         if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
         }
      }

      // Modulate FPS
      ++countedFrames;

      // If frame finished early
      int frameTicks = capTimer.getTicks();
      if (frameTicks < SCREEN_TICKS_PER_FRAME) {
         // Wait remaining time
         SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
      }

      // Swap the windows (Render)
      SDL_GL_SwapWindow( main_window );
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
   texture->Render(element->get_tex_x(), element->get_tex_y(), texture->curr_clip_);
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
   // SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
   // SDL_RenderClear(renderer);

   // Calculate and correct fps
   float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
   if( avgFPS > 2000000 ) {
      avgFPS = 0;
   }

   // Draw title screen
   animate(gameover_screen_.texture.fps, &gameover_screen_, &gameover_screen_.texture, 
      gameover_screen_.texture.fps_timer, gameover_screen_.texture.last_frame);

   // Update the screen
   // SDL_RenderPresent(renderer);
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

   /* ANIMATION FOR TITLE SCREEN */
   // Animate background
   if (finger_) {
      // std::cout << "Here 1\n";
      if (finger_->get_y() == START || finger_->get_y() == WORLD1) {
         // std::cout << "Here 2\n";
         animate(menu_background_.textures["forest"]->fps, &menu_background_, menu_background_.textures["forest"], 
            menu_background_.textures["forest"]->fps_timer, menu_background_.textures["forest"]->last_frame);
      } else if (finger_->get_y() == WORLD2) {
         // std::cout << "Here 3\n";
         animate(menu_background_.textures["cloud"]->fps, &menu_background_, menu_background_.textures["cloud"], 
            menu_background_.textures["cloud"]->fps_timer, menu_background_.textures["cloud"]->last_frame);
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
         level_flag_ = FOREST1;
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
         if (finger_->textures["point"]->completed_) {
            menu_screen_ = SECOND;
            finger_->finger_state = Finger::SHAKE;
            finger_->set_y(WORLD1);
            finger_->set_x(720);
            finger_->textures["point"]->completed_ = false;
         }
      } else if (finger_->get_y() == WORLD1) {
         if (finger_->textures["point"]->completed_) {
            // Set to final menu screen
            menu_screen_ = THIRD;

            // Tell finger not to update
            finger_->updating = false;
         }
      }
   }
   // DEBUG DRAW
   // world_.DrawDebugData();

   // for (int i = 0; i < 15; i++) {
   //    GLFloatRect m;
   //    m.w = r[i].w;
   //    m.h = r[i].h;
   //    m.x = r[i].x;
   //    m.y = r[i].y - m.h;

   //    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
   //    SDL_RenderDrawRect(renderer, &m);
   // }
  
   // Update the screen
   // SDL_RenderPresent(renderer);

   //SDL_GL_SwapWindow(main_window);
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
      std::string path1(sprite_path + "Levels/Forest/board");
      std::string path2("/format");
      std::string path = path1 + std::to_string(static_cast<int>(level_flag_)) + path2;
      level = new Level(path, level_flag_);
      game_flag_ = PLAY;
   }

   // // Clear screen as the first things that's done?
   // SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
   // SDL_RenderClear(renderer);

   // // Update world timer
   // world_.Step(timeStep_, velocityIterations_, positionIterations_);
   // world_.ClearForces();

   // // Start cap timer
   // capTimer.start();
      
   // // Calculate and correct fps
   // float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
   // if( avgFPS > 2000000 ) {
   //    avgFPS = 0;
   // }

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

   // DEBUG DRAW
   // world_.DrawDebugData();

   // for (int i = 0; i < 15; i++) {
   //    GLFloatRect m;
   //    m.w = r[i].w;
   //    m.h = r[i].h;
   //    m.x = r[i].x;
   //    m.y = r[i].y - m.h;

   //    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
   //    SDL_RenderDrawRect(renderer, &m);
   // }

   // SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
   // SDL_RenderDrawLine(renderer, 1300, 454, 1500, 454);
   // SDL_RenderDrawLine(renderer, 1400, 354, 1400, 554);

   // // Update the screen
   // SDL_RenderPresent(renderer);
   // ++countedFrames;

   // // If frame finished early
   // int frameTicks = capTimer.getTicks();
   // if (frameTicks < SCREEN_TICKS_PER_FRAME) {
   //    // Wait remaining time
   //    SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
   // }
}

// Get the height
const float Application::get_height() {
    return SCREEN_HEIGHT;
}

// Get the width
const float Application::get_width() {
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
   SDL_DestroyWindow(main_window);
   main_window = NULL;
   renderer = NULL;
   
   //Quit SDL subsystems
   IMG_Quit();
   SDL_Quit();
}

/********** FINGER ***************/

// Constructor
Finger::Finger() : 
   Element(700, 665, 67, 124), finger_state(SHAKE), updating(true) {}

// Get texture
Texture *Finger::get_texture() {
   if (finger_state == SHAKE) {
      return textures["shake"];
   }
   
   if (finger_state == POINT) {
      return textures["point"];
   }
}

// Update function
void Finger::update() {
   // Animate based on state
   if (finger_state == SHAKE) {
      animate(textures["shake"]);
   } else if (finger_state == POINT) {
      animate(textures["point"]);
   }

   // Get texture and Render it
   Texture *tex = get_texture();
   
   // Render finger
   Render(tex);
}

// Load media function
bool Finger::LoadMedia() {
   // Success flag
   bool success = true; 

   // Load data
   // anim_data["shake"] = TextureData(8, 1.0f / 20.0f, "", "");
   // anim_data["point"] = TextureData(6, 1.0f / 20.0f, "", "");


   std::vector<TextureData> data;
   data.push_back(TextureData(8, 1.0f / 20.0f, "shake", Application::sprite_path + "Miscealaneous/finger_shake.png"));
   data.push_back(TextureData(6, 1.0f / 20.0f, "point", Application::sprite_path + "Miscealaneous/finger_point.png"));

   // Load resources
   success = RenderingEngine::get_instance().LoadResources(this, data);

   // // Load finger point
   // textures.emplace("point", Texture());
   // if (!textures["point"]->LoadFromFile(path.c_str(), true)) {
   //    printf("Failed to load finger_point.png\n");
   //    success = false;
   // } else {
   //    // Allocate enough room
   //    textures["point"]->clips_ = new GLFloatRect[6];
   //    GLFloatRect *temp = textures["point"].clips_;

   //    // Calculate sprite locations
   //    for (int i = 0; i < 6; i++) {
   //       temp[i].x = i * 124;
   //       temp[i].y = 0;
   //       temp[i].w = 124;
   //       temp[i].h = 67;
   //    }

   //    // Set curr clip
   //    textures["point"].curr_clip_ = &temp[0];
   // }

   // // Load finger shake
   // textures.emplace("shake", Texture());
   // if (!textures["shake"].LoadFromFile(path.c_str(), true)) {
   //    printf("Failed to load finger_shake.png\n");
   //    success = false;
   // } else {
   //    // Allocate enough room
   //    textures["shake"].clips_ = new GLFloatRect[8];
   //    GLFloatRect *temp = textures["shake"].clips_;

   //    // Calculate sprite locations
   //    for (int i = 0; i < 8; i++) {
   //       temp[i].x = i * 124;
   //       temp[i].y = 0;
   //       temp[i].w = 124;
   //       temp[i].h = 67;
   //    }

   //    // Set curr clip
   //    textures["shake"].curr_clip_ = &temp[0];
   // }

   // Return success flag
   return success;
}