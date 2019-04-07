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
   Entity(960, 412, 150, 46, application), player_state_(STAND),
   idle_texture(this, 15), running_texture(this, 19), kick_texture(this, 15), 
   idle_jump_texture(this, 15), running_jump_texture(this, 16), arm_texture(this, 0), 
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
   const b2Vec2 center = {(92.0f - get_width()) / 2.0f * application->to_meters_ - 0.02f, 0.0f};
   box.SetAsBox(width, height, center, 0.0f);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 1.8f;
   body->CreateFixture(&fixture_def);

   // Set user data
   body->SetUserData(this);

   // Set health. TODO: set health in a better way
   health = 100;
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
   last_frame += 
      (fps_timer.getDeltaTime() / 1000.0f);
   if (last_frame > get_application()->animation_update_time_) {
      animate();
      last_frame = 0.0f;
   }

   // Update player if not frozen
   if (!freeze)
      move();

   // Adjust deltas first
   adjust_deltas();

   // Render player
   Texture *playertexture = get_texture();
   SDL_Rect *curr_clip = get_curr_clip();
   if (curr_clip) {
     // Render player
     render(playertexture, curr_clip);

     // Render arm if idle, render shooting if not
     if (!shooting) {
         if (get_player_state() == 1) {
            arm_running_texture.render(get_x() + get_width() +
               arm_delta_x, get_y() + arm_delta_y,
               arm_running_texture.curr_clip_, 0.0,
               &arm_running_texture.center_, arm_running_texture.flip_);
         } else {
            arm_texture.render(get_x() + get_width() + 
               arm_delta_x, get_y() + arm_delta_y, NULL, 0.0, 
               &arm_texture.center_, arm_texture.flip_);
         }
     } else {
        arm_shoot_texture.render(get_x() + get_width() + 
           arm_delta_shoot_x, get_y() + arm_delta_shoot_y, 
           arm_shoot_texture.curr_clip_, 0.0, 
           &arm_shoot_texture.center_, arm_shoot_texture.flip_);
     }
   }
}

// Adjust delta function
void Player::adjust_deltas() {
   if (player_state_ == STAND) {
      if (entity_direction == RIGHT) {
         arm_delta_x = 12;
         arm_delta_y = 64;
         arm_delta_shoot_x = 12;
         arm_delta_shoot_y = 51;
      } else {
         arm_delta_x = -22;
         arm_delta_y = 64;
         arm_delta_shoot_x = -75;
         arm_delta_shoot_y = 51;
      }
   } else if (player_state_ == RUN && body->GetLinearVelocity().y == 0) {
      if (entity_direction == RIGHT) {
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
      if (entity_direction == RIGHT) {
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
   } else if (player_state_ == RUN_AND_JUMP || player_state_ == JUMP) {
      // Adjust deltas
      if (entity_direction == RIGHT) {
         arm_delta_x = 10;
         arm_delta_y = 69;
         arm_delta_shoot_x = 10;
         arm_delta_shoot_y = 57;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 69;
         arm_delta_shoot_x = -75;
         arm_delta_shoot_y = 57;
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
void Player::animate(Texture *tex, int reset) {
   // Shooting animation
   if (shooting) {
      //std::cout << arm_shoot_texture.frame_ << std::endl;
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
   } else if (player_state_ == STOP && body->GetLinearVelocity().y == 0) {
      // Animate stopping
      if (running_texture.frame_ > 19) {
         running_texture.frame_ = 19;
      }
      running_texture.curr_clip_ = &running_texture.clips_[running_texture.frame_];
      ++running_texture.frame_;
   } else if (player_state_ == RUN_AND_JUMP) {
      // Animate
      if (running_jump_texture.frame_ > 16) {
         running_jump_texture.frame_ = 0;
      }
      running_jump_texture.curr_clip_ = &running_jump_texture.clips_[running_jump_texture.frame_];
      ++running_jump_texture.frame_;
   } else if (player_state_ == JUMP) {
      // Animate
      if (idle_jump_texture.frame_ > 14) {
         idle_jump_texture.frame_ = 0;
      }
      idle_jump_texture.curr_clip_ = &idle_jump_texture.clips_[idle_jump_texture.frame_];
      ++idle_jump_texture.frame_;
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
        body->GetLinearVelocity().x == 0) {
      
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
   /*
   if (player_state_ != RUN && player_state_ != RUN_AND_JUMP && player_state_ != JUMP) {
      b2Vec2 vel = {0, 0};
      body->SetLinearVelocity(vel);
   }
   */

   // Update player state
   change_player_state();

   // Shooting
   if (get_application()->current_key_states_[SDL_SCANCODE_SPACE]) {
      // Set shooting to true
      shooting = true;

      // Create eraser
      if (arm_shoot_texture.frame_ == 1)
         create_projectile(53, -7, 75, 12, 21, 1, 10, eraser_texture);
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
         player_state_ = RUN;
         b2Vec2 vel = {3.4f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      } else if (player_state_ == JUMP || player_state_ == RUN_AND_JUMP) {
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
         const b2Vec2 force = {0, 5.5f};
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

/********************* ENEMY IMPLEMENTATIONS ******************/

Enemy::Enemy(Application *application) :
   Entity(1000, 412, 92, 82, application), enemy_state_(IDLE),
   idle_texture(this, 17), shoot_texture(this, 6), poojectile_texture(this, 7),
   death_texture(this, 15), shoot_timer_(0) {

   // Setup Box2D
   // Set body type
   body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x = 1000.0f * application->to_meters_;
   float y = -412.5f * application->to_meters_;
   body_def.position.Set(x, y);
   body_def.fixedRotation = true;

   // Attach body to world
   body = get_application()->world_.CreateBody(&body_def);

   // Set box dimensions
   float width = (get_width() / 2.0f) * application->to_meters_ - 0.02f;
   float height = (get_height() / 2.0f) * application->to_meters_ - 0.02f;
   box.SetAsBox(width, height);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 1.8f;
   body->CreateFixture(&fixture_def);

   // Set user data so it can react
   body->SetUserData(this);

   // Set health
   health = 100;
}

void Enemy::update(bool freeze) {
   // Move first
   move();

   // Render enemy
   Texture *enemytexture = get_texture();
   SDL_Rect *curr_clip = get_curr_clip();
   if (curr_clip) {
     // Render player
     render(enemytexture, curr_clip);
   }
}

void Enemy::move() {
   // Check to see what direction the enemy should be facing
   if (get_application()->get_player()->get_x() <= get_x()) {
      entity_direction = LEFT;
   } else if (get_application()->get_player()->get_x() > get_x()) {
      entity_direction = RIGHT;
   }

   // Overall check to see if it's alive
   if (alive) {
      // Check to see if get_player() within bounds of enemy
      if (get_application()->get_player()->get_y() >= get_y() - get_height() &&
            get_application()->get_player()->get_y() <= get_y() + get_height()) {
         // Set state to shoot
         enemy_state_ = SHOOT;

         // Update timer
         ++shoot_timer_;

         // Shoot if timer goes off
         if (shoot_timer_ >= 50) {
            Projectile *tmp = create_projectile(15, -10, 70, 15, 24, 0, 10, poojectile_texture);
            tmp->body->SetGravityScale(0);
            shoot_timer_ = 0;
         }
      } else {
         enemy_state_ = IDLE;
      }
   } else {
      // For now, use IDLE animation
      // TODO: set state to death
      enemy_state_ = DEATH;
   }

   // Update frames
   last_frame += 
      (fps_timer.getDeltaTime() / 1000.0f);
   if (last_frame > get_application()->animation_update_time_) {
      animate();
      last_frame = 0.0f;
   }
}

void Enemy::animate(Texture *tex, int reset) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      if (idle_texture.frame_ > 17) {
         idle_texture.frame_ = 0;
      }
      idle_texture.curr_clip_ = &idle_texture.clips_[idle_texture.frame_];
      ++idle_texture.frame_;
   } else if (enemy_state_ == SHOOT) {
      if (shoot_texture.frame_ > 6) {
         shoot_texture.frame_ = 0;
      }
      shoot_texture.curr_clip_ = &shoot_texture.clips_[shoot_texture.frame_];
      ++shoot_texture.frame_;
   } else if (enemy_state_ == DEATH) {
      if (death_texture.frame_ > 15) {
         death_texture.frame_ = 15;
      }
      death_texture.curr_clip_ = &death_texture.clips_[death_texture.frame_];
      ++death_texture.frame_;
   }
}

Texture *Enemy::get_texture() {
   // Get idle texture
   if (enemy_state_ == IDLE) {
      return &idle_texture;
   }
   
   // Get shoot texture
   if (enemy_state_ == SHOOT) {
      return &shoot_texture;
   }

   // Get death texture
   if (enemy_state_ == DEATH) {
      return &death_texture;
   }
}

Enemy::~Enemy() {
   idle_texture.free();
   shoot_texture.free();
   poojectile_texture.free();
   death_texture.free();
}
