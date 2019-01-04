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
    has_jumped_(false) {}

// Rendering function for all entities
void Entity::render(Texture *texture, SDL_Rect *clip) {
    // NEED TO TAKE INTO ACCOUNT THAT BOTTOM OF IMAGE ISN'T ALLIGNED WITH FEET
    // Rendering the textures according to their centers
    set_x((100.0f * body->GetPosition().x) - (get_width() / 2.0f));
    set_y((100.0f * -body->GetPosition().y) - (get_height() / 2.0f));
    //printf("x = %d, y = %d\n", get_x(), get_y());
    texture->render(get_x(), get_y(), clip, 0.0, &texture->center_, texture->flip_);
}

// Update function for all entities. For now all it does is call move
void Entity::update() {
    move();
}

// Destructor
Entity::~Entity() {}

/******************** PLAYER IMPLEMENTATIONS ***************************/

// Initializ the player by calling it's constructor
Player::Player(Application* application) : 
    Entity(960, 412, 150, 46, application), player_state_(STAND), player_direction_(RIGHT),
    idle_texture(this), running_texture(this), kick_texture(this), running_jump_texture(this),
    arm_texture(this), arm_shoot_texture(this), arm_running_texture(this), eraser_texture(this),
    shooting(false), arm_delta_x(12), arm_delta_y(64),
    arm_delta_shoot_x(12), arm_delta_shoot_y(51) {

    // Set width and height
    set_height(150.0);
    set_width(46.0);

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
    printf("width = %f, height = %f\n", width, height);
    const b2Vec2 center = {(92.0f - get_width()) / 2.0f * application->to_meters_ - 0.02f, 0.0f};
    box.SetAsBox(width, height, center, 0.0f);

    // Set various fixture definitions and create fixture
    fixture_def.shape = &box;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.75f;
    body->CreateFixture(&fixture_def);
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

   // Return idle texture for now
   return &idle_texture;
}

// Get current clip
SDL_Rect *Player::get_curr_clip() {
   Texture *tmp = get_texture();
   return tmp->curr_clip_;
}

// Get player state
Player::STATE Player::get_player_state() {
   return player_state_;
}

// Animate based on state
void Player::animate() {
   // Shooting animation
   if (shooting) {
      if (arm_shoot_texture.frame_ > 6) {
         arm_shoot_texture.frame_ = 0;
         shooting = false;
      }
      arm_shoot_texture.curr_clip_ = &arm_shoot_texture.clips_[arm_shoot_texture.frame_];
      ++arm_shoot_texture.frame_;
   }

   // Choose animation based on what state player is in
   if (player_state_ == STAND) {
      // Animate
      if (idle_texture.frame_ > 15) {
         idle_texture.frame_ = 0;
      }
      idle_texture.curr_clip_ = &idle_texture.clips_[idle_texture.frame_];
      ++idle_texture.frame_;

      // Adjust deltas
      if (player_direction_ == RIGHT) {
         arm_delta_x = 12;
         arm_delta_y = 64;
         arm_delta_shoot_x = 12;
         arm_delta_shoot_y = 51;
      } else {
         // Adjust the deltas
         arm_delta_x = -22;
         arm_delta_y = 64;
         arm_delta_shoot_x = -75;
         arm_delta_shoot_y = 51;
      }
   } else if (player_state_ == RUN && body->GetLinearVelocity().y == 0) {
      // Animation man
      if (running_texture.frame_ > 14) {
         running_texture.frame_ = 4;
      }
      running_texture.curr_clip_ = &running_texture.clips_[running_texture.frame_];
      ++running_texture.frame_;

      // Animate arm
      if (arm_running_texture.frame_ > 3) {
         arm_running_texture.frame_ = 3;
      }
      arm_running_texture.curr_clip_ = &arm_running_texture.clips_[arm_running_texture.frame_];
      ++arm_running_texture.frame_;

      // Adjust deltas
      if (player_direction_ == RIGHT) {
         arm_delta_x = 10;
         arm_delta_y = 63;
         arm_delta_shoot_x = 12;
         arm_delta_shoot_y = 51;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 63;
         arm_delta_shoot_x = -75;
         arm_delta_shoot_y = 51;
      }
   } else if (player_state_ == STOP && body->GetLinearVelocity().y == 0) {
      // Animate stopping
      if (running_texture.frame_ > 19) {
         running_texture.frame_ = 19;
      }
      running_texture.curr_clip_ = &running_texture.clips_[running_texture.frame_];
      ++running_texture.frame_;

      // Adjust deltas
      if (player_direction_ == RIGHT) {
         arm_delta_x = 10;
         arm_delta_y = 63;
         arm_delta_shoot_x = 12;
         arm_delta_shoot_y = 51;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 63;
         arm_delta_shoot_x = -75;
         arm_delta_shoot_y = 51;
      }
   } else if (player_state_ == RUN_AND_JUMP) {
      // Animate
      if (running_jump_texture.frame_ > 16) {
         running_jump_texture.frame_ = 0;
      }
      running_jump_texture.curr_clip_ = &running_jump_texture.clips_[running_jump_texture.frame_];
      ++running_jump_texture.frame_;

      // Adjust deltas
      if (player_direction_ == RIGHT) {
         arm_delta_x = 8;
         arm_delta_y = 63;
         arm_delta_shoot_x = 8;
         arm_delta_shoot_y = 51;
      } else {
         arm_delta_x = -28;
         arm_delta_y = 63;
         arm_delta_shoot_x = -83;
         arm_delta_shoot_y = 51;
      }
   }
}

// Movement logic of the player. Done through keyboard.
void Player::move() {
   //printf("Player state = %d\n", player_state_);
   //printf("x = %f, y = %f\n", body->GetPosition().x, body->GetPosition().y);
   // Default texture
   if (body->GetLinearVelocity().x == 0 && body->GetLinearVelocity().y == 0) {
      player_state_ = STAND;
   }

   // Check for stopping
   if ((!get_application()->current_key_states_[SDL_SCANCODE_RIGHT] 
            || !get_application()->current_key_states_[SDL_SCANCODE_LEFT]) && 
         body->GetLinearVelocity().x != 0 && body->GetLinearVelocity().y == 0) {

      // Set state
      player_state_ = STOP;
   }

   // Deal with basic movement for now
   if (get_application()->current_key_states_[SDL_SCANCODE_RIGHT]) { //&& 
         //body->GetLinearVelocity().y == 0) {
      // Check for flag
      if (running_texture.has_flipped_) {
         running_texture.has_flipped_ = false;
         running_texture.flip_ = SDL_FLIP_NONE;

         // Idle texture flip
         idle_texture.has_flipped_ = false;
         idle_texture.flip_ = SDL_FLIP_NONE;

         // Running and jumping texture flip
         running_jump_texture.has_flipped_ = false;
         running_jump_texture.flip_ = SDL_FLIP_NONE;

         // Shooting arm and idle arm flip
         arm_texture.has_flipped_ = false;
         arm_texture.flip_ = SDL_FLIP_NONE;
         arm_shoot_texture.has_flipped_ = false;
         arm_shoot_texture.flip_ = SDL_FLIP_NONE;
         arm_running_texture.has_flipped_ = false;
         arm_running_texture.flip_ = SDL_FLIP_NONE;
      }
      
      // Set direction
      player_direction_ = RIGHT;

      // Set state
      player_state_ = RUN;

      // Use Box2D version for moving
      const b2Vec2 force = {3.4f, 0};
      body->ApplyForce(force, body->GetPosition(), true);
   } 

   // Player running left
   if (get_application()->current_key_states_[SDL_SCANCODE_LEFT]) { //&&
         //body->GetLinearVelocity().y == 0) {
      // Check for flag
      if (!running_texture.has_flipped_) {
         // Running texture flip
         running_texture.has_flipped_ = true;
         running_texture.flip_ = SDL_FLIP_HORIZONTAL;

         // Idle texture flip
         idle_texture.has_flipped_ = true;
         idle_texture.flip_ = SDL_FLIP_HORIZONTAL;

         // Running and jumping texture flip
         running_jump_texture.has_flipped_ = true;
         running_jump_texture.flip_ = SDL_FLIP_HORIZONTAL;

         // Shooting arm and idle arm flip
         arm_texture.has_flipped_ = true;
         arm_texture.flip_ = SDL_FLIP_HORIZONTAL;
         arm_shoot_texture.has_flipped_ = true;
         arm_shoot_texture.flip_ = SDL_FLIP_HORIZONTAL;
         arm_running_texture.has_flipped_ = true;
         arm_running_texture.flip_ = SDL_FLIP_HORIZONTAL;
      }
      
      // Set direction
      player_direction_ = LEFT;

      // Set state
      player_state_ = RUN;

      // Use Box2D version for moving
      const b2Vec2 force = {-3.4f, 0};
      body->ApplyForce(force, body->GetPosition(), true);
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

   // Mid air?
   if (body->GetLinearVelocity().x != 0 && body->GetLinearVelocity().y != 0) {
      player_state_ = RUN_AND_JUMP;
   }

   // Shooting
   if (get_application()->current_key_states_[SDL_SCANCODE_SPACE]) {
      // Set shooting to true
      shooting = true;

      // Create eraser
      create_eraser();
   }

   // Update frames
   // Change FPS
   //printf("time since last frame = %f, animation update time = %f\n",
         //get_application()->time_since_last_frame_, get_application()->animation_update_time_);
   get_application()->time_since_last_frame_ += get_application()->getFPSTimer().getDeltaTime();
   //printf("delta time = %u\n", get_application()->getFPSTimer().getDeltaTime());
   if (get_application()->time_since_last_frame_ > get_application()->animation_update_time_) {
      animate();
      get_application()->time_since_last_frame_ = 0.0f;
   }
}

// Create eraser function
void Player::create_eraser() {
   // First, create a new projectile
   Application *tmp = get_application();
   Projectile *eraser = new Projectile(get_x() + get_width() + 15 + 63, get_y() + 51, 1, 10, tmp);
   eraser->texture = eraser_texture;

   // Then set the box2d physics
   // Set width and height
   eraser->set_height(12.0);
   eraser->set_width(21.0);

   // Setup Box2D
   // Set body type
   eraser->body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x = get_x() + get_width() + 63;
   float y = get_y() + 51;
   eraser->body_def.position.Set(x, y);
   eraser->body_def.fixedRotation = true;

   // Attach body to world
   eraser->body = tmp->world_.CreateBody(&eraser->body_def);

   // Set box dimensions
   float width = (eraser->get_width() / 2.0f) * tmp->to_meters_ - 0.02f;// - 0.11f;
   float height = (eraser->get_height() / 2.0f) * tmp->to_meters_ - 0.02f;// - 0.11f;
   eraser->box.SetAsBox(width, height);

   // Set various fixture definitions and create fixture
   eraser->fixture_def.shape = &eraser->box;
   eraser->fixture_def.density = 1.0f;
   eraser->fixture_def.friction = 1.0f;
   eraser->body->CreateFixture(&eraser->fixture_def);
   
   // Now add it to the things the world needs to render
   tmp->getObjectVector()->push_back(eraser->texture);
}

// Virtual destructor
Player::~Player() {}
