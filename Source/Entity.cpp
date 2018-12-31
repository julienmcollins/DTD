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
    Entity(960, 412, 150, 46, application), player_state_(STAND), player_direction_(NEUTRAL),
    idle_texture_(this), running_texture_(this), kick_texture_(this), running_jump_texture_(this),
    arm_texture_(this) {

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
   if (player_state_ == RUN || player_state_ == STOP) {
      return &running_texture_;
   }

   // Return idle texture for now
   return &idle_texture_;
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
   // Animate arm next to the player if it's not shooting
   /*
   if (player_state_ != SHOOT) {
      printf("x = %d, y = %d\n", get_x(), get_y());
      arm_texture_.render(get_x() + get_width() + 12, get_y() + 62, NULL, 0.0, 
            &arm_texture_.center_, arm_texture_.flip_);
   }
   */

   // Choose animation based on what state player is in
   if (player_state_ == STAND) {
      if (idle_texture_.frame_ > 15) {
         idle_texture_.frame_ = 0;
      }
      idle_texture_.curr_clip_ = &idle_texture_.clips_[idle_texture_.frame_];
      ++idle_texture_.frame_;
   } else if (player_state_ == RUN && body->GetLinearVelocity().y == 0) {
      if (running_texture_.frame_ > 14) {
         running_texture_.frame_ = 4;
      }
      running_texture_.curr_clip_ = &running_texture_.clips_[running_texture_.frame_];
      ++running_texture_.frame_;
   } else if (player_state_ == STOP && body->GetLinearVelocity().y == 0) {
      if (running_texture_.frame_ > 19) {
         running_texture_.frame_ = 19;
      }
      running_texture_.curr_clip_ = &running_texture_.clips_[running_texture_.frame_];
      ++running_texture_.frame_;
   } else if (player_state_ == RUN_AND_JUMP) {
      std::cout << "RUNNING AND JUMPING" << std::endl;
      if (running_jump_texture_.frame_ > 16) {
         running_jump_texture_.frame_ = 0;
      }
      running_jump_texture_.curr_clip_ = &running_jump_texture_.clips_[running_jump_texture_.frame_];
      ++running_jump_texture_.frame_;
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
         body->GetLinearVelocity().x != 0) {

      // Set state
      player_state_ = STOP;
   }

   // Deal with basic movement for now
   if (get_application()->current_key_states_[SDL_SCANCODE_RIGHT]) {
      // Check for flag
      if (running_texture_.has_flipped_) {
         running_texture_.has_flipped_ = false;
         running_texture_.flip_ = SDL_FLIP_NONE;

         // Idle texture flip
         idle_texture_.has_flipped_ = false;
         idle_texture_.flip_ = SDL_FLIP_NONE;
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
   if (get_application()->current_key_states_[SDL_SCANCODE_LEFT]) {
      // Check for flag
      if (!running_texture_.has_flipped_) {
         // Running texture flip
         running_texture_.has_flipped_ = true;
         running_texture_.flip_ = SDL_FLIP_HORIZONTAL;

         // Idle texture flip
         idle_texture_.has_flipped_ = true;
         idle_texture_.flip_ = SDL_FLIP_HORIZONTAL;
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
            std::cout << "In run and jump" << std::endl;
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
   } if (get_application()->current_key_states_[SDL_SCANCODE_DOWN]) {
     // Need to revisit this
     //add_y(5);
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

// Virtual destructor
Player::~Player() {}
