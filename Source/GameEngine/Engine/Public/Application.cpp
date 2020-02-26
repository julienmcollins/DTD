//
//  Application.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include "Source/GameEngine/Engine/Private/Application.h"
#include "Source/GameEngine/Engine/Private/Level.h"
#include "Source/GameEngine/Engine/Private/Notebook.h"

#include "Source/GameEngine/ObjectManager/Private/Entity.h"
#include "Source/GameEngine/ObjectManager/Private/Global.h"
#include "Source/GameEngine/ObjectManager/Private/Enemy.h"

#include "Source/GameEngine/RenderingEngine/Private/RenderingEngine.h"
#include "Source/GameEngine/RenderingEngine/Private/Texture.h"

#include "Source/GameEngine/Development/Private/DevelZoom.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <Box2D/Box2D.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cmath>
#include <tuple>

#define DEVELOPER_MODE 0

//Shader loading utility programs
void printProgramLog( unsigned int program );
void printShaderLog( unsigned int shader );

// Initialize paths
std::string Application::sprite_path = "Media/Sprites/";
std::string Application::audio_path = "Media/Audio/";

using namespace std;

void drawLines() {
   float i = 0.025f;
   float j = 0.03f;
   float vertices[] = {
      -i, 0.0f, i,  0.0f,
       0.0f, j, 0.0f, -j
   };
   unsigned int VBO, VAO;
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0); 

   // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
   // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
   glBindVertexArray(0);

   RenderingEngine::GetInstance().GetShaderReference("color")->Use()->SetVector3f("color", glm::vec3(1.0f, 0.0f, 0.0f));
   glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
   glDrawArrays(GL_LINES, 0, 8);
}

void drawTriangle() {
   float i = 0.5f * (1920.0f / 2.0f);
   float j = 0.5f * (1080.0f / 2.0f);
   float vertices[] = {
      -i, -j, // left  
       i, -j,  // right 
       0.0f, j  // top   
   }; 

   unsigned int VBO, VAO;
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0); 

   // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
   // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
   glBindVertexArray(0);

   ShaderProgram *shader = RenderingEngine::GetInstance().GetShaderReference("basic_color")->Use();
   glm::mat4 model = glm::mat4(1.0f);
   shader->SetMatrix4("model", model);
   glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
   glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawHitBoxes(GLFloatRect *r) {
   // Set to wireframe
   glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

   // Static variables for these particular shapes
   static GLuint debug_vbo = 0;
   static GLuint debug_ibo = 0;
   static GLuint debug_vao = 0;
   float debug_vertices[] = {
      // positionscoords
      1.0f,  1.0f,
      1.0f, -1.0f,
     -1.0f, -1.0f,
     -1.0f,  1.0f
   };
   GLuint debug_indices[] = {
      3, 1, 2,
      3, 0, 1
   };

   glGenVertexArrays(1, &debug_vao);
   glGenBuffers(1, &debug_vbo);
   glGenBuffers(1, &debug_ibo);
   glBindVertexArray(debug_vao);
   glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(debug_vertices), debug_vertices, GL_DYNAMIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debug_ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(debug_indices), debug_indices, GL_DYNAMIC_DRAW);
   
   // Position attribute
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
   // glEnableVertexAttribArray(1);

   // Disable
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

   // std::cout << debug_vbo << " " << debug_ibo << " " << debug_vao << std::endl;

   for (int i = 0; i < 15; i++) {
      GLFloatRect m;
      m.w = r[i].w / 1.920f;
      m.h = r[i].h / 1.080f;
      m.x = r[i].x;
      m.y = (r[i].y - m.h);

      // std::cout << m.w << " " << m.h << " " << m.x << " " << m.y << std::endl;

      ShaderProgram *shader = RenderingEngine::GetInstance().GetShaderReference("hitbox")->Use();
      float vertices[] = {
        -m.w, -m.h,
         m.w, -m.h,
         m.w,  m.h,
        -m.w,  m.h
      };
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(m.x + m.w / 2.0f, m.y + m.h / 2.0f, 0.0f));
      shader->SetMatrix4("model", model);
      shader->SetVector3f("color", glm::vec3(1.0f, 0.0f, 0.0f));
      glBindVertexArray(debug_vao);
      glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
   }

   // Set to normal
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

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
   ruler_(200, 722, 200, 50),
   gameover_screen_(0, 0, 1920, 1080),
   thanks_screen_(0, 0, 1920, 1080),
   notebook_background_(0, 0, 1920, 1080),
   cloud_layer_(0, 0, 5760, 1080) {
    
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

   // Create zoom object
   #if DEVELOPER_MODE == 1
   zoom_ = new DevelZoom();
   #endif

   // Create notebook
   notebook_ = new Notebook();

   // Timer for cloud drift
   cloud_pos_ = 0.0f;
   cloud_last_frame_ = 0.0f;
   cloud_fps_ = 1.0f / 10.0f;
   cloud_timer_.Start();

   // Start counting frames per second
   fpsTimer.Start();
}

// Initialize OpenGL
bool Application::InitOpenGL() {
   // Load shaders, and viewport
   // TODO: For zoom, create a function that changes this and sets it for the shader
   glm::mat4 projection = glm::ortho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

   // Load texture shader
   RenderingEngine::GetInstance().LoadShader("texture_shader");
   RenderingEngine::GetInstance().LoadApplicationPerspective("texture_shader", projection);

   // Load basic shader
   RenderingEngine::GetInstance().LoadShader("color");

   // Load basic perspective
   RenderingEngine::GetInstance().LoadShader("hitbox");
   RenderingEngine::GetInstance().GetShaderReference("hitbox")->Use()->SetMatrix4("projection", projection);

   // Screen shader
   RenderingEngine::GetInstance().LoadShader("zoom_shader");
   // RenderingEngine::GetInstance().LoadApplicationPerspective("zoom_shader", projection);

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
bool Application::LoadMedia() {
   // Start success = true 
   bool success = true;

   // String
   std::string s;

   /********* MENU BACKGROUNDS **********/
   s = sprite_path + "Menu/menu_background_sheet.png";
   menu_background_.sprite_sheet = RenderingEngine::GetInstance().LoadTexture("menu_background_sheet", s.c_str());
   menu_background_.animations.emplace("forest", new Animation(menu_background_.sprite_sheet, "forest", 1920.0f, 1080.0f, 0.0f, 3, 1.0f / 4.0f));
   menu_background_.animations.emplace("cloud", new Animation(menu_background_.sprite_sheet, "cloud", 1920.0f, 1080.0f, 1080.0f, 3, 1.0f / 4.0f));
   RenderingEngine::GetInstance().LoadResources(&menu_background_);

   /************* TITLES **************/
   s = sprite_path + "Menu/menu_title_sheet.png";
   menu_title_.sprite_sheet = RenderingEngine::GetInstance().LoadTexture("menu_title_sheet", s.c_str());
   menu_title_.animations.emplace("title", new Animation(menu_title_.sprite_sheet, "title", 646.0f, 513.0f, 704.0f, 3, 1.0f / 4.0f));
   menu_title_.animations.emplace("menu", new Animation(menu_title_.sprite_sheet, "menu", 323.0f, 372.0f, 332.0f, 3, 1.0f / 4.0f));
   menu_title_.animations.emplace("world", new Animation(menu_title_.sprite_sheet, "world", 193.0f, 332.0f, 0.0f, 3, 1.0f / 4.0f));
   RenderingEngine::GetInstance().LoadResources(&menu_title_);

   /************ GAMEOVER **************/
   s = sprite_path + "Miscealaneous/gameover.png";
   gameover_screen_.sprite_sheet = RenderingEngine::GetInstance().LoadTexture("gameover", s.c_str());
   gameover_screen_.animations.emplace("gameover", new Animation(gameover_screen_.sprite_sheet, "gameover", 1920.0f, 1080.0f, 0.0f, 3, 1.0f / 3.0f));
   RenderingEngine::GetInstance().LoadResources(&gameover_screen_);
      
   /************ NOTEBOOK **************/
   s = sprite_path + "Miscealaneous/notebookwhite.png";
   notebook_background_.sprite_sheet = RenderingEngine::GetInstance().LoadTexture("notebook", s.c_str());
   notebook_background_.animations.emplace("notebook", new Animation(notebook_background_.sprite_sheet, "notebook", 1920.0f, 1080.0f, 0.0f, 1, 1.0f / 3.0f));
   RenderingEngine::GetInstance().LoadResources(&notebook_background_);

   /************* CLOUD LAYER **********/
   s = sprite_path + "Miscealaneous/clouds.png";
   cloud_layer_.sprite_sheet = RenderingEngine::GetInstance().LoadTexture("clouds", s.c_str());
   cloud_layer_.animations.emplace("clouds", new Animation(cloud_layer_.sprite_sheet, "clouds", 7680.0f, 1080.0f, 0.0f, 2, 1.0f / 3.0f));
   RenderingEngine::GetInstance().LoadResources(&cloud_layer_);

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
   menu_platform_ = new Platform(960, 925, 1920, 10);
   menu_platform_->setup();

   // Setup invisible wall
   invisible_wall_ = new Platform(0, 1055, 10, 1000);
   invisible_wall_->setup();

   // Load and play music
   music = Mix_LoadMUS("sounds/hobbits.mp3");
   //Mix_PlayMusic(music, 1);

   // Set menu screen to first screen
   menu_screen_ = FIRST;
}

void Application::Draw() {
   /******** UPDATE THE LEVEL *************/
   // Do this as the last thing (testing)
   Level::GetInstance().Update();
   /***************************************/

   // ITERATE THROUGH THE PROJECTILES AND DRAW THEM
   update_projectiles();
      
   // Update player
   if (player->is_alive()) {
      // Update player
      player->Update();

      // Render the hit markers
      for (int i = 0; i < player->hit_markers.size(); i++) {
         player->hit_markers[i]->Update();
      }

      // Check for completed level and that player as walked to the edge of the screen
      if (Level::GetInstance().completed) {
         // Clear level board
         Level::GetInstance().ClearBoard();

         // Change mode to setup
         game_flag_ = SETUP;

         // Increment level_flag_
         int inc = static_cast<int>(level_flag_);
         level_flag_ = static_cast<FOREST>(inc + 1);
         std::cout << level_flag_ << std::endl;
      }
   } else {
      app_flag_ = GAMEOVER_SCREEN;
      delete player;
      Level::GetInstance().ClearBoard();
      menu_flag = true;
   }

   /*
   // DEBUG DRAW
   world_.DrawDebugData();
   */

   // drawLines();
}

// Updates the screen
void Application::Update() {
   // Game loop
   while (!quit) {
      // Clear screen at every instance
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear( GL_COLOR_BUFFER_BIT );

      // Update world timer
      world_.Step(timeStep_, velocityIterations_, positionIterations_);
      world_.ClearForces();

      // Start cap timer
      capTimer.Start();

      // Calculate and correct fps
      float avgFPS = countedFrames / ( fpsTimer.GetTicks() / 1000.f );
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
         capTimer.Start();

         // Calculate and correct fps
         float avgFPS = countedFrames / ( fpsTimer.GetTicks() / 1000.f );
         if( avgFPS > 2000000 ) {
            avgFPS = 0;
         }

         // Update the screen
         ++countedFrames;

         // If frame finished early
         int frameTicks = capTimer.GetTicks();
         if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
         }
      }

      // Modulate FPS
      ++countedFrames;

      // If frame finished early
      int frameTicks = capTimer.GetTicks();
      if (frameTicks < SCREEN_TICKS_PER_FRAME) {
         // Wait remaining time
         SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
      }

      // Swap the windows (Render)
      SDL_GL_SwapWindow( main_window );
   }
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
   capTimer.Start();

   // Calculate and correct fps
   float avgFPS = countedFrames / ( fpsTimer.GetTicks() / 1000.f );
   if( avgFPS > 2000000 ) {
      avgFPS = 0;
   }

   // Draw title screen
   gameover_screen_.sprite_sheet->Animate(gameover_screen_.GetAnimationByName("gameover"));
   gameover_screen_.sprite_sheet->Render(0.0f, 0.0f, 0.0f, gameover_screen_.GetAnimationByName("gameover"));

   // Update the screen
   // SDL_RenderPresent(renderer);
   ++countedFrames;

   // If frame finished early
   int frameTicks = capTimer.GetTicks();
   if (frameTicks < SCREEN_TICKS_PER_FRAME) {
      // Wait remaining time
      SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
   }
}

void Application::RenderCloudLayer() {
   // Increment cloud timer
   if ((cloud_pos_ / 5.0f) < -7680.0f) {
      cloud_pos_ = 0.0f;
   } else {
      cloud_pos_--;
   }

   // render based on drift
   cloud_layer_.sprite_sheet->Animate(cloud_layer_.GetAnimationByName("clouds"));
   cloud_layer_.sprite_sheet->Render(cloud_pos_ / 5.0f, 0.0f, 0.0f, cloud_layer_.GetAnimationByName("clouds"));
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
         } 
         if (finger_ && finger_->updating) {
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

   // Render background
   notebook_background_.sprite_sheet->Render(0.0f, 0.0f, 0.0f, notebook_background_.GetAnimationByName("notebook"));

   // Set framebuffer
   notebook_->SetToFrameBuffer();

   // Render cloud layer
   RenderCloudLayer();

   /* ANIMATION FOR TITLE SCREEN */
   if (finger_) {
      // std::cout << "Here 1\n";
      if (finger_->get_y() == START || finger_->get_y() == WORLD1) {
         menu_background_.sprite_sheet->Animate(menu_background_.GetAnimationByName("forest"));
         menu_background_.sprite_sheet->Render(0.0f, 0.0f, 0.0f, menu_background_.GetAnimationByName("forest"));
      } else if (finger_->get_y() == WORLD2) {
         menu_background_.sprite_sheet->Animate(menu_background_.GetAnimationByName("cloud"));
         menu_background_.sprite_sheet->Render(0.0f, 0.0f, 0.0f, menu_background_.GetAnimationByName("cloud"));
      }
   }

   // Animate title
   menu_title_.sprite_sheet->Animate(menu_title_.GetAnimationByName("title"));
   menu_title_.sprite_sheet->Render(640.0f, 70.0f, 0.0f, menu_title_.GetAnimationByName("title"));

   // Animate menu items
   if (menu_screen_ == FIRST) {
      // Animate menu items
      menu_title_.sprite_sheet->Animate(menu_title_.GetAnimationByName("menu"));
      menu_title_.sprite_sheet->Render(800.0f, 650.0f, 0.0f, menu_title_.GetAnimationByName("menu"));

      // Animate player
      player->Update(true);

      // Update finger
      finger_->Update();
   } else if (menu_screen_ == SECOND) {
      // Animate world items
      menu_title_.sprite_sheet->Animate(menu_title_.GetAnimationByName("world"));
      menu_title_.sprite_sheet->Render(850.0f, 650.0f, 0.0f, menu_title_.GetAnimationByName("world"));

      // Animate player
      player->Update(true);

      // Update finger
      finger_->Update();
   } else if (menu_screen_ == THIRD) {
      // Animate final background
      // Update player for real (and projectiles)
      player->Update();
      update_projectiles();

      // Check to see if player has reached the edge
      if (player->get_x() >= 1890) {
         // Set correct app flags
         app_flag_ = PLAYGROUND;
         level_flag_ = FORESTBOSS;
         game_flag_ = SETUP;
         delete menu_platform_;
         delete invisible_wall_;
         //Mix_FreeMusic(music);
                  
         // Delete finger_
         delete finger_;
         finger_ = nullptr;
      }
   }

   // Check enter key state
   if (finger_ && finger_->finger_state == Finger::POINT) {
      if (finger_->get_y() == START) {
         if (finger_->AnimationCompleted("point")) {
            menu_screen_ = SECOND;
            finger_->finger_state = Finger::SHAKE;
            finger_->set_y(WORLD1);
            finger_->set_x(720);
         }
      } else if (finger_->get_y() == WORLD1) {
         if (finger_->AnimationCompleted("point")) {
            // Set to final menu screen
            menu_screen_ = THIRD;

            // Tell finger not to update
            finger_->updating = false;
         }
      }
   }
   // world_.DrawDebugData();
   // drawHitBoxes(r);
   notebook_->Render();
}

// UPDATE PROJECTILES
void Application::update_projectiles() {
   // ITERATE THROUGH THE PROJECTILES AND DRAW THEM
   for (std::vector<Projectile *>::iterator it = projectiles_.begin(); it != projectiles_.end();) {
      // Check to see if it's still allocated
      if (*it) {
         // Check if it's alive or not
         if (!(*it)->is_alive()) {
            delete (*it);
            it = projectiles_.erase(it);
         } else {
            //std::cout << "NO, in here" << std::endl;
            (*it)->Update();
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
      Level::GetInstance().SetBoard(path, level_flag_);
      game_flag_ = PLAY;
   }

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

   // Render background
   notebook_background_.sprite_sheet->Render(0.0f, 0.0f, 0.0f, notebook_background_.GetAnimationByName("notebook"));

   // Draw normally
   notebook_->SetToFrameBuffer();
   RenderCloudLayer();
   Draw();
   notebook_->Render();

#if DEVELOPER_MODE == 1
   // Draw everything
   zoom_->SetToFrameBuffer();
   Draw();
   zoom_->Render();
#endif

}

// Get the height
const float Application::get_height() {
    return SCREEN_HEIGHT;
}

// Get the width
const float Application::get_width() {
    return SCREEN_WIDTH;
}

// Destructs application
Application::~Application() {
   // Clear the screen
   SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
   SDL_RenderClear(renderer);
   
   // Stop fps timer
   fpsTimer.Stop();
   capTimer.Stop();

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

Animation *Finger::GetAnimationFromState() {
   if (finger_state == SHAKE) {
      return GetAnimationByName("shake");
   }
   
   if (finger_state == POINT) {
      return GetAnimationByName("point");
   }
}

// Update function
void Finger::Update() {
   // Animate based on state
   if (finger_state == SHAKE) {
      sprite_sheet->Animate(GetAnimationByName("shake"));
   } else if (finger_state == POINT) {
      sprite_sheet->Animate(GetAnimationByName("point"));
   }

   // Render
   sprite_sheet->Render(get_anim_x(), get_anim_y(), 0.0f, GetAnimationFromState());
}

// Load media function
bool Finger::LoadMedia() {
   // // Success flag
   bool success = true; 

   // Load the file
   std::string finger_path = Application::sprite_path + "Menu/finger_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("finger_sheet", finger_path.c_str());

   // Insantiate animations
   animations.emplace("shake", new Animation(sprite_sheet, "shake", 124.0f, 68.0f, 68.0f, 8, 1.0f / 20.0f));
   animations.emplace("point", new Animation(sprite_sheet, "point", 124.0f, 68.0f, 0.0f, 6, 1.0f / 20.0f));

   // Load resources
   RenderingEngine::GetInstance().LoadResources(this);

   // Return success flag
   return success;
}