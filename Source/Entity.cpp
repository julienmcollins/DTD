#include <stdio.h>
#include <cmath>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <Box2D/Box2D.h>

#include "Entity.h"
#include "Object.h"
#include "Texture.h"
#include "Application.h"
#include "Timer.h"

/*************************** ENTITY IMPLEMENTATIONS ******************************/

// Entity constructor which will provide basic establishment for all entities
Entity::Entity(int x_pos, int y_pos, double height, double width, Application* application) : 
   Element(x_pos, y_pos, height, width, application),
   has_jumped_(false), health(0) {
}

// Update function for all entities. For now all it does is call move
void Entity::update(bool freeze) {
   move();
}

// Get direction
int Entity::get_dir() const {
   return entity_direction;
}

// Create projectile
Projectile* Entity::create_projectile(int delta_x_r, int delta_x_l, int delta_y, int height, int width,
     bool owner, bool damage, Texture texture) {
   // First, create a new projectile
   Application *tmp = get_application();
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new Projectile(get_x() + get_width() + delta_x_r, get_y() + delta_y, 
            height, width, owner, damage, this, tmp);
   } else {
      proj = new Projectile(get_x() + delta_x_l, get_y() + delta_y, height, width, owner,
            damage, this, tmp);
   }

   // Set texture
   proj->texture = texture;

   // Set shot direction
   proj->shot_dir = entity_direction;

   // Return projectile reference
   return proj;
}

// Destructor
Entity::~Entity() {}

/******************** PLAYER IMPLEMENTATIONS ***************************/

// Initializ the player by calling it's constructor
Player::Player(Application* application) : 
   // The new sprite is going to be 37 wide (the character itself)
   // TODO: Load in new smaller sprite sheet
   Entity(960, 412, 104, 37, application), player_state_(STAND),
   idle_texture(this, 4), running_texture(this, 20), kick_texture(this, 15), 
   idle_jump_texture(this, 14), running_jump_texture(this, 16), arm_texture(this, 4), 
   arm_shoot_texture(this, 8), arm_running_texture(this, 3), eraser_texture(this, 0),
   shooting(false), arm_delta_x(12), arm_delta_y(64),
   arm_delta_shoot_x(12), arm_delta_shoot_y(51) {

   // Set entity direction
   entity_direction = RIGHT;

   // Setup Box2D
   // Set body type
   body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x = 600.0f * application->to_meters_;
   float y = -412.5f * application->to_meters_;
   body_def.position.Set(x, y);
   body_def.fixedRotation = true;

   // Attach body to world
   body = get_application()->world_.CreateBody(&body_def);

   // Set box dimensions
   float width = (get_width() / 2.0f) * application->to_meters_ - 0.02f;// - 0.11f;
   float height = (get_height() / 2.0f) * application->to_meters_ - 0.02f;// - 0.11f;
   //printf("width = %d, height = %d\n", get_width(), get_height());
   const b2Vec2 center = {(62.0f - get_width()) / 2.0f * application->to_meters_, 
                          4.0f * application->to_meters_};
   box.SetAsBox(width, height, center, 0.0f);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 0.0f;
   body->CreateFixture(&fixture_def);

   // Set user data
   body->SetUserData(this);

   // Set health. TODO: set health in a better way
   health = 100;

   // TEMPORARY SOLUTION
   arm_shoot_texture.completed_ = true;

   // Set fps
   idle_texture.fps = 1.0f / 4.0f;
   running_texture.fps = 1.0f / 20.0f;
   running_jump_texture.fps = 1.0f / 20.0f;
   idle_jump_texture.fps = 1.0f / 20.0f;
   arm_texture.fps = 1.0f / 4.0f;
   arm_shoot_texture.fps = 1.0f / 20.0f;
   arm_running_texture.fps = 1.0f / 20.0f;
}

// Get texture based on state
Texture *Player::get_texture() {
   // Return run or stop texture (aka running texture)
   if (player_state_ == RUN || player_state_ == STOP) {
      return &running_texture;
   }

   // Return run and jump texture
   if (player_state_ == RUN_AND_JUMP) {
      return &running_jump_texture;
   }

   // Return jump texture
   if (player_state_ == JUMP) {
      return &idle_jump_texture;
   }

   // Return idle texture for now
   return &idle_texture;
}

// Get player state
Player::STATE Player::get_player_state() {
   return player_state_;
}

// Update function
void Player::update(bool freeze) {
   //std::cout << "State: " << player_state_ << " (0: STAND, 1: RUN, 2: JUMP, 3: STOP, 4: CROUCH, 5: RUN_AND_JUMP)" << std::endl;
   // Update frames
   /*
   last_frame += 
      (fps_timer.getDeltaTime() / 1000.0f);
   if (last_frame > get_application()->animation_update_time_) {
      animate();
      last_frame = 0.0f;
   }
   */
   animate();

   // Update player if not frozen
   if (!freeze)
      move();

   // Adjust deltas first
   adjust_deltas();

   // Render player
   Texture *playertexture = get_texture();

   // Render arm if idle, render shooting if not
   if (!shooting) {
       if (get_player_state() == 1) {
          arm_running_texture.render(get_x() + get_width() +
             arm_delta_x, get_y() + arm_delta_y,
             arm_running_texture.curr_clip_, 0.0,
             &arm_running_texture.center_, arm_running_texture.flip_);
       } else {
          //std::cout << arm_texture.max_frame_ << std::endl;
          arm_texture.render(get_x() + get_width() + 
             arm_delta_x, get_y() + arm_delta_y, arm_texture.curr_clip_, 0.0, 
             &arm_texture.center_, arm_texture.flip_);
       }
   } else {
      arm_shoot_texture.render(get_x() + get_width() + 
         arm_delta_shoot_x, get_y() + arm_delta_shoot_y, 
         arm_shoot_texture.curr_clip_, 0.0, 
         &arm_shoot_texture.center_, arm_shoot_texture.flip_);
   }

   // Render player
   render(playertexture);
}

// Adjust delta function
void Player::adjust_deltas() {
   if (player_state_ == STAND) {
      if (entity_direction == RIGHT) {
         arm_delta_x = 1;
         arm_delta_y = 43;
         arm_delta_shoot_x = 2;
         arm_delta_shoot_y = 34;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 43;
         arm_delta_shoot_x = -58;
         arm_delta_shoot_y = 34;
      }
   } else if (player_state_ == RUN && body->GetLinearVelocity().y == 0) {
      if (entity_direction == RIGHT) {
         arm_delta_x = 0;
         arm_delta_y = 43;
         arm_delta_shoot_x = 2;
         arm_delta_shoot_y = 34;
      } else {
         arm_delta_x = -19;
         arm_delta_y = 43;
         arm_delta_shoot_x = -58;
         arm_delta_shoot_y = 34;
      }
   } else if (player_state_ == STOP && body->GetLinearVelocity().y == 0) {
      if (entity_direction == RIGHT) {
         arm_delta_x = 2;
         arm_delta_y = 43;
         arm_delta_shoot_x = 2;
         arm_delta_shoot_y = 34;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 43;
         arm_delta_shoot_x = -58;
         arm_delta_shoot_y = 34;
      }
   } else if (player_state_ == RUN_AND_JUMP || player_state_ == JUMP) {
      // Adjust deltas
      if (entity_direction == RIGHT) {
         arm_delta_x = 1;
         arm_delta_y = 42;
         arm_delta_shoot_x = 2;
         arm_delta_shoot_y = 34;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 42;
         arm_delta_shoot_x = -58;
         arm_delta_shoot_y = 34;
      }
   } /*else if (player_state_ == JUMP) {
      // Adjust deltas
      if (entity_direction == RIGHT) {
         arm_delta_x = 12;
         arm_delta_y = 64;
         arm_delta_shoot_x = 10;
         arm_delta_shoot_y = 57;
      } else {
         arm_delta_x = -22;
         arm_delta_y = 64;
         arm_delta_shoot_x = -75;
         arm_delta_shoot_y = 57;
      }
   }*/
}

// Animate based on state
void Player::animate(Texture *tex, int reset, int max, int start) {
   // Shooting animation
   if (shooting) {
      //std::cout << arm_shoot_texture.frame_ << std::endl;
      if (arm_shoot_texture.frame_ > 6) {
         arm_shoot_texture.frame_ = 0;
         shooting = false;
         arm_shoot_texture.completed_ = true;
      }
      arm_shoot_texture.curr_clip_ = &arm_shoot_texture.clips_[arm_shoot_texture.frame_];
      ++arm_shoot_texture.frame_;
   } else {
      Element::animate(&arm_texture, 0);
   }

   // Choose animation based on what state player is in
   if (player_state_ == STAND) {
      Element::animate(&idle_texture, 0, 4);
   } else if (player_state_ == RUN && body->GetLinearVelocity().y == 0) {
      Element::animate(&running_texture, 4, 15);
      Element::animate(&arm_running_texture, 3);
   } else if (player_state_ == STOP && body->GetLinearVelocity().y == 0) {
      Element::animate(&running_texture, 20);
   } else if (player_state_ == RUN_AND_JUMP) {
      Element::animate(&running_jump_texture);
   } else if (player_state_ == JUMP) {
      Element::animate(&idle_jump_texture);
   }
}

// Change player state
void Player::change_player_state() {
   // Special stop state
   if ((!get_application()->current_key_states_[SDL_SCANCODE_RIGHT] 
            && !get_application()->current_key_states_[SDL_SCANCODE_LEFT]) && 
         body->GetLinearVelocity().x != 0 && body->GetLinearVelocity().y == 0) {

      // Set state
      player_state_ = STOP;

      // Return
      return;
   }

   // Special fall state, TODO: add falling animation
   if ((get_application()->current_key_states_[SDL_SCANCODE_RIGHT] ||
            get_application()->current_key_states_[SDL_SCANCODE_LEFT]) &&
        body->GetLinearVelocity().x == 0 && body->GetLinearVelocity().y != 0) {
      
      // Set state
      player_state_ = JUMP;

      // Return
      return;
   }

   // Check for non-zero y-vel
   if (body->GetLinearVelocity().y != 0) {
      // Run and jump or just jump
      if (body->GetLinearVelocity().x != 0) {
         player_state_ = RUN_AND_JUMP;
      } else {
         player_state_ = JUMP;
      }
   } else if (body->GetLinearVelocity().x != 0) {
      player_state_ = RUN;
   } else {
      player_state_ = STAND;
   }
}

// Movement logic of the player. Done through keyboard.
void Player::move() {

   // If not running or running and jumping, then set linear velocity to 0
   if (player_state_ != RUN && player_state_ != RUN_AND_JUMP && player_state_ != JUMP) {
      b2Vec2 vel = {0, body->GetLinearVelocity().y};
      body->SetLinearVelocity(vel);
      player_state_ = STOP;
   }

   // Update player state
   change_player_state();

   // Shooting
   if (get_application()->current_key_states_[SDL_SCANCODE_SPACE]) {
      // Set shooting to true
      shooting = true;

      // Create eraser
      if (arm_shoot_texture.completed_) {
         create_projectile(38, -12, 41, 12, 21, 1, 10, eraser_texture);
         arm_shoot_texture.completed_ = false;
      }
   }

   // Player running left
   if (get_application()->current_key_states_[SDL_SCANCODE_LEFT]) {
      // Check for flag
      if (entity_direction == RIGHT) {
         // Running texture flip
         running_texture.has_flipped_ = true;
         running_texture.flip_ = SDL_FLIP_HORIZONTAL;

         // Idle texture flip
         idle_texture.has_flipped_ = true;
         idle_texture.flip_ = SDL_FLIP_HORIZONTAL;

         // Running and jumping texture flip
         running_jump_texture.has_flipped_ = true;
         running_jump_texture.flip_ = SDL_FLIP_HORIZONTAL;

         // Jump texture flip
         idle_jump_texture.has_flipped_ = true;
         idle_jump_texture.flip_ = SDL_FLIP_HORIZONTAL;

         // Shooting arm and idle arm flip
         arm_texture.has_flipped_ = true;
         arm_texture.flip_ = SDL_FLIP_HORIZONTAL;
         arm_shoot_texture.has_flipped_ = true;
         arm_shoot_texture.flip_ = SDL_FLIP_HORIZONTAL;
         arm_running_texture.has_flipped_ = true;
         arm_running_texture.flip_ = SDL_FLIP_HORIZONTAL;
      }
      
      // Set direction
      entity_direction = LEFT;

      // Check for midair
      if (player_state_ == RUN || player_state_ == STAND || player_state_ == STOP) {
         player_state_ = RUN;
         b2Vec2 vel = {-3.4f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      } else if (player_state_ == JUMP || player_state_ == RUN_AND_JUMP) {
         has_jumped_ = true;
         if (body->GetLinearVelocity().x > -4.0f) {
            const b2Vec2 force = {-5.4f, 0};
            body->ApplyForce(force, body->GetPosition(), true);
         }
      }
   } 

   // Deal with basic movement for now
   if (get_application()->current_key_states_[SDL_SCANCODE_RIGHT]) {
      // Check for flag
      if (entity_direction == LEFT) {
         running_texture.has_flipped_ = false;
         running_texture.flip_ = SDL_FLIP_NONE;

         // Idle texture flip
         idle_texture.has_flipped_ = false;
         idle_texture.flip_ = SDL_FLIP_NONE;

         // Running and jumping texture flip
         running_jump_texture.has_flipped_ = false;
         running_jump_texture.flip_ = SDL_FLIP_NONE;

         // Jump texture flip
         idle_jump_texture.has_flipped_ = false;
         idle_jump_texture.flip_ = SDL_FLIP_NONE;

         // Shooting arm and idle arm flip
         arm_texture.has_flipped_ = false;
         arm_texture.flip_ = SDL_FLIP_NONE;
         arm_shoot_texture.has_flipped_ = false;
         arm_shoot_texture.flip_ = SDL_FLIP_NONE;
         arm_running_texture.has_flipped_ = false;
         arm_running_texture.flip_ = SDL_FLIP_NONE;
      }
      
      // Set direction
      entity_direction = RIGHT;

      // Set state
      // Set to jump and run if not on the ground
      if (player_state_ == RUN || player_state_ == STAND || player_state_ == STOP) {
         //std::cout << player_state_ << std::endl;
         //std::cout << body->GetLinearVelocity().y << std::endl;
         player_state_ = RUN;
         b2Vec2 vel = {3.4f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      } else if (player_state_ == JUMP || player_state_ == RUN_AND_JUMP) {
         //std::cout << "Hello?" << std::endl;
         has_jumped_ = true;
         if (body->GetLinearVelocity().x < 4.0f) {
            const b2Vec2 force = {5.4f, 0};
            body->ApplyForce(force, body->GetPosition(), true);
         }
      }
   }
   
   // Player jumping
   if (get_application()->current_key_states_[SDL_SCANCODE_UP]) {
      if (!has_jumped_) {
         // Set state
         if (player_state_ == RUN) {
            player_state_ = RUN_AND_JUMP;
         } else {
            player_state_ = JUMP;
         }

         // Apply an impulse
         const b2Vec2 force = {0, 3.5f};
         body->ApplyLinearImpulse(force, body->GetPosition(), true);

         // Set the flags
         has_jumped_ = true;
      } else {
         if (body->GetLinearVelocity().y == 0) {
            has_jumped_ = false;
         }
      }
   } 
   
   if (get_application()->current_key_states_[SDL_SCANCODE_DOWN]) {
     // Need to revisit this
     //add_y(5);
   }
}

// Start contact function
void Player::start_contact(Element *element) {
   if (element && (element->type() == "Projectile" || element->is_enemy())) {
      health -= 10;
      if (health <= 0) {
         alive = false;
      }
   }
}

// Load media function
bool Player::load_media() {
   // Temp flag
   bool success = true;

   // Load player idle
   if (!idle_texture.loadFromFile("images/player/new/idle_no_arm.png")) {
       printf("Failed to load idle_no_arm.png!\n");
       success = false;
   } else {
      // Allocat enough room for the clips
      idle_texture.clips_ = new SDL_Rect[5];
      SDL_Rect *temp = idle_texture.clips_;

      // Calculate locations
      for (int i = 0; i < 5; i++) {
         temp[i].x = i * 62;
         temp[i].y = 0;
         temp[i].w = 62;
         temp[i].h = 104;
      }

      // Set curr clip
      idle_texture.curr_clip_ = &temp[0];
   }

   // Load player jumping idly
   if (!idle_jump_texture.loadFromFile("images/player/new/idle_jump_no_arm.png")) {
      printf("Failed to load idle_jump_texture!\n");
      success = false;
   } else {
      // Allocate enough room for the clips
      idle_jump_texture.clips_ = new SDL_Rect[15];
      SDL_Rect *temp = idle_jump_texture.clips_;

      // Calculate sprites
      for (int i = 0; i < 15; i++) {
         temp[i].x = i * 62;
         temp[i].y = 0;
         temp[i].w = 62;
         temp[i].h = 104;
      }

      // Set curr clip
      idle_jump_texture.curr_clip_ = &temp[0];
   }

   // Load player running
   if (!running_texture.loadFromFile("images/player/new/running_no_arm.png")) {
       printf("Failed to load running_no_arm.png image!\n");
       success = false;
   } else {
      // Allocat enough room for the clips
      running_texture.clips_ = new SDL_Rect[21];
      SDL_Rect *temp = running_texture.clips_;

      // Calculate locations
      for (int i = 0; i < 21; i++) {
         temp[i].x = i * 62;
         temp[i].y = 0;
         temp[i].w = 62;
         temp[i].h = 104;
      }

      // Set curr clip
      running_texture.curr_clip_ = &temp[0];
   }

   // Load player kicking
   if (!kick_texture.loadFromFile("images/player/kick.png")) {
       printf("Failed to load Texture image!\n");
       success = false;
   } else {
      // Allocat enough room for the clips
      kick_texture.clips_ = new SDL_Rect[16];
      SDL_Rect *temp = kick_texture.clips_;

      // Calculate locations
      for (int i = 0; i < 16; i++) {
         temp[i].x = i * 75;
         temp[i].y = 0;
         temp[i].w = 75;
         temp[i].h = 150;
      }

      // Set curr clip
      kick_texture.curr_clip_ = &temp[0];
   }

   // Load jump and run
   if (!running_jump_texture.loadFromFile("images/player/new/running_jump_no_arm.png")) {
      printf("Failed to load Running Jump image!\n");
      success = false;
   } else {
      // Allocate enough room for the clips
      running_jump_texture.clips_ = new SDL_Rect[17];
      SDL_Rect *temp = running_jump_texture.clips_;

      // Calculate the locations
      for (int i = 0; i < 17; i++) {
         temp[i].x = i * 62;
         temp[i].y = 0;
         temp[i].w = 62;
         temp[i].h = 104;
      }

      // Set curr clip
      running_jump_texture.curr_clip_ = &temp[0];
   }

   // Turn animation width 52 --> turns from facing right to left

   // Load arm
   if (!arm_texture.loadFromFile("images/player/new/idle_arm.png")) {
      printf("Failed to load Arm image!\n");
      success = false;
   } else {
      // Allocate enough room
      arm_texture.clips_ = new SDL_Rect[5];
      SDL_Rect *temp = arm_texture.clips_;

      // Calculate the locations
      for (int i = 0; i < 5; i++) {
         temp[i].x = i * 7;
         temp[i].y = 0;
         temp[i].w = 7;
         temp[i].h = 24;
      }

      // Set curr clip
      arm_texture.curr_clip_ = &temp[0];
   }

   // Load shooting arm
   if (!arm_shoot_texture.loadFromFile("images/player/new/arm_throw.png")) {
      printf("Failed to load arm shooting texture!\n");
      success = false;
   } else {
     // Allocate enough room for the clips
     arm_shoot_texture.clips_ = new SDL_Rect[9];
     SDL_Rect *temp = arm_shoot_texture.clips_;

     // Calculate the locations
     for (int i = 0; i < 9; i++) {
        temp[i].x = i * 44;
        temp[i].y = 0;
        temp[i].w = 44;
        temp[i].h = 33;
     }

     // Set curr_clip
     arm_shoot_texture.curr_clip_ = &temp[0];
   }

   // Running arm
   if (!arm_running_texture.loadFromFile("images/player/new/running_arm.png")) {
      printf("Failed to load running arm texture!\n");
      success = false;
   } else {
      // Allocate enough room for the clips
      arm_running_texture.clips_ = new SDL_Rect[4];
      SDL_Rect *temp = arm_running_texture.clips_;

      // Calculate the locations
      for (int i = 0; i < 4; i++) {
         temp[i].x = i * 7;
         temp[i].y = 0;
         temp[i].w = 7;
         temp[i].h = 22;
      }

      // Set curr clip
      arm_running_texture.curr_clip_ = &temp[0];
   }

   // Eraser
   if (!eraser_texture.loadFromFile("images/player/eraser.png")) {
      printf("Failed to load eraser texture!\n");
      success = false;
   } else {
      // Allocate one image for it
      eraser_texture.clips_ = new SDL_Rect[1];
      SDL_Rect *temp = eraser_texture.clips_;
      temp[0].x = 0; temp[0].y = 0; temp[0].w = 21; temp[0].h = 12;

      // Set curr clip
      eraser_texture.curr_clip_ = &temp[0];
   }

   // Return success
   return success;
}

// Virtual destructor
Player::~Player() {
   idle_texture.free();
   running_texture.free();
   kick_texture.free();
   running_jump_texture.free();
   idle_jump_texture.free();
   arm_texture.free();
   arm_shoot_texture.free();
   arm_running_texture.free();
   eraser_texture.free();
}

