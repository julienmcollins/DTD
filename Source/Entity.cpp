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
   shooting(false), arm_delta_x(12), arm_delta_y(64),
   arm_delta_shoot_x(12), arm_delta_shoot_y(51), prev_pos_x_(0.0f), prev_pos_y_(0.0f) {

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

   // TODO: ADD FIXTURES TO THIS AS SENSORS
   left_sensor_ = new Sensor(0.8f, 0.2f, this, -0.05f);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 0.0f;
   fixture_def.userData = this;
   body->CreateFixture(&fixture_def);

   // Set user data
   body->SetUserData(this);

   // Set health. TODO: set health in a better way
   health = 100;

   // TEMPORARY SOLUTION
   textures["arm_throw"].completed_ = true;

   // Set in contact = false
   in_contact = false;
}

// Get texture based on state
Texture *Player::get_texture() {
   // Return run or stop texture (aka running texture)
   if (player_state_ == RUN || player_state_ == STOP) {
      return &textures["running"];
   }

   // Return run and jump texture
   if (player_state_ == RUN_AND_JUMP) {
      return &textures["running_jump"];
   }

   // Return jump texture
   if (player_state_ == JUMP) {
      return &textures["jump"];
   }

   // Return push texture
   if (player_state_ == PUSH) {
      return &textures["push"];
   }

   // Return jump and push texture
   if (player_state_ == JUMP_AND_PUSH) {
      return &textures["jump_push"];
   }

   // Return idle texture for now
   return &textures["idle"];
}

// Get player state
Player::STATE Player::get_player_state() {
   return player_state_;
}

// Update function
void Player::update(bool freeze) {
   //std::cout << "State: " << player_state_ << " (0: STAND, 1: RUN, 2: JUMP, 3: STOP, 4: RUN_AND_JUMP, 5: PUSH)" << std::endl;
   animate();

   // Update player if not frozen
   if (!freeze)
      move();

   // Adjust deltas first
   adjust_deltas();

   // Render player
   Texture *playertexture = get_texture();

   // Render arm if idle, render shooting if not
   if (player_state_ != PUSH && player_state_ != JUMP_AND_PUSH) {
      if (!shooting) {
         if (get_player_state() == 1) {
            textures["running_arm"].render(get_x() + get_width() +
               arm_delta_x, get_y() + arm_delta_y,
               textures["running_arm"].curr_clip_, 0.0,
               &textures["running_arm"].center_, textures["running_arm"].flip_);
         } else {
            //std::cout << textures["idle_arm"].max_frame_ << std::endl;
            textures["idle_arm"].render(get_x() + get_width() + 
               arm_delta_x, get_y() + arm_delta_y, textures["idle_arm"].curr_clip_, 0.0, 
               &textures["idle_arm"].center_, textures["idle_arm"].flip_);
         }
      } else {
         textures["arm_throw"].render(get_x() + get_width() + 
            arm_delta_shoot_x, get_y() + arm_delta_shoot_y, 
            textures["arm_throw"].curr_clip_, 0.0, 
            &textures["arm_throw"].center_, textures["arm_throw"].flip_);
      }
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
      textures["arm_throw"].last_frame += 
      (textures["arm_throw"].fps_timer.getDeltaTime() / 1000.0f);
      if (textures["arm_throw"].last_frame > get_application()->animation_update_time_) {
         if (textures["arm_throw"].frame_ > 6) {
            textures["arm_throw"].frame_ = 0;
            shooting = false;
            textures["arm_throw"].completed_ = true;
         }
         textures["arm_throw"].curr_clip_ = &textures["arm_throw"].clips_[textures["arm_throw"].frame_];
         ++textures["arm_throw"].frame_;
         textures["arm_throw"].last_frame = 0.0f;
      }
      Element::animate(&textures["eraser"]);
   } else {
      Element::animate(&textures["idle_arm"], 0);
   }

   // Choose animation based on what state player is in
   if (player_state_ == STAND) {
      Element::animate(&textures["idle"], 0, 4);
   } else if (player_state_ == RUN && body->GetLinearVelocity().y == 0) {
      Element::animate(&textures["running"], 4, 15);
      Element::animate(&textures["running_arm"], 3);
   } else if (player_state_ == STOP && body->GetLinearVelocity().y == 0) {
      Element::animate(&textures["running"], 20);
   } else if (player_state_ == RUN_AND_JUMP) {
      Element::animate(&textures["running_jump"]);
   } else if (player_state_ == JUMP) {
      Element::animate(&textures["jump"]);
   } else if (player_state_ == PUSH) {
      Element::animate(&textures["push"]);
   } else if (player_state_ == JUMP_AND_PUSH) {
      Element::animate(&textures["jump_push"]);
   }
}

// Change player state
void Player::change_player_state() {
   // Key touches
   bool right = get_application()->current_key_states_[SDL_SCANCODE_RIGHT];
   bool left = get_application()->current_key_states_[SDL_SCANCODE_LEFT];
   bool up = get_application()->current_key_states_[SDL_SCANCODE_UP];

   // Special push state
   if (in_contact) {
      if ((left || right) && (up || body->GetLinearVelocity().y != 0)) {
         player_state_ = JUMP_AND_PUSH;
      } else if (!left && !right && (up || body->GetLinearVelocity().y != 0)) {
         player_state_ = JUMP;
      } else if (entity_direction == LEFT && left) {
         player_state_ = PUSH;
      } else if (entity_direction == RIGHT && right) {
         player_state_ = PUSH;
      } else {
         player_state_ = STAND;
      }
      return;
   }

   // Special stand state
   if ((!right && !left) && 
      body->GetLinearVelocity().x == 0 && 
      body->GetLinearVelocity().y == 0) {

      // Set state to stand
      player_state_ = STAND;

      // Return
      return;
   }

   // Special stop state
   if ((!right && !left) && 
      body->GetLinearVelocity().x != 0 && 
      body->GetLinearVelocity().y == 0 && 
      player_state_ != PUSH) {

      // Set state
      player_state_ = STOP;

      // Return
      return;
   }

   // Special fall state, TODO: add falling animation
   if ((right || left) && 
      body->GetLinearVelocity().x == 0 && 
      body->GetLinearVelocity().y != 0) {
      
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
   if (player_state_ != RUN && player_state_ != RUN_AND_JUMP && player_state_ != JUMP && player_state_ != PUSH) {
      b2Vec2 vel = {0, body->GetLinearVelocity().y};
      body->SetLinearVelocity(vel);
      player_state_ = STOP;
   }

   // Shooting
   if (get_application()->current_key_states_[SDL_SCANCODE_SPACE]) {
      // Set shooting to true
      shooting = true;

      // Create eraser
      if (textures["arm_throw"].completed_) {
         create_projectile(38, -12, 41, 12, 21, 1, 10, textures["eraser"]);
         textures["arm_throw"].completed_ = false;
      }
   }

   // Player running left
   if (get_application()->current_key_states_[SDL_SCANCODE_LEFT]) {
      // Check for flag
      if (entity_direction == RIGHT) {
         // Running texture flip
         textures["running"].has_flipped_ = true;
         textures["running"].flip_ = SDL_FLIP_HORIZONTAL;

         // Idle texture flip
         textures["idle"].has_flipped_ = true;
         textures["idle"].flip_ = SDL_FLIP_HORIZONTAL;

         // Running and jumping texture flip
         textures["running_jump"].has_flipped_ = true;
         textures["running_jump"].flip_ = SDL_FLIP_HORIZONTAL;

         // Jump texture flip
         textures["jump"].has_flipped_ = true;
         textures["jump"].flip_ = SDL_FLIP_HORIZONTAL;

         // Shooting arm and idle arm flip
         textures["idle_arm"].has_flipped_ = true;
         textures["idle_arm"].flip_ = SDL_FLIP_HORIZONTAL;
         textures["arm_throw"].has_flipped_ = true;
         textures["arm_throw"].flip_ = SDL_FLIP_HORIZONTAL;
         textures["running_arm"].has_flipped_ = true;
         textures["running_arm"].flip_ = SDL_FLIP_HORIZONTAL;

         // Pushing texture
         textures["push"].has_flipped_ = true;
         textures["push"].flip_ = SDL_FLIP_HORIZONTAL;
         textures["jump_push"].has_flipped_ = true;
         textures["jump_push"].flip_ = SDL_FLIP_HORIZONTAL;
      }

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
      } else if (player_state_ == PUSH && entity_direction == RIGHT) {
         player_state_ = RUN;
         in_contact = false;
      }
            
      // Set direction
      entity_direction = LEFT;
   } 

   // Deal with basic movement for now
   if (get_application()->current_key_states_[SDL_SCANCODE_RIGHT]) {
      // Check for flag
      if (entity_direction == LEFT) {
         textures["running"].has_flipped_ = false;
         textures["running"].flip_ = SDL_FLIP_NONE;

         // Idle texture flip
         textures["idle"].has_flipped_ = false;
         textures["idle"].flip_ = SDL_FLIP_NONE;

         // Running and jumping texture flip
         textures["running_jump"].has_flipped_ = false;
         textures["running_jump"].flip_ = SDL_FLIP_NONE;

         // Jump texture flip
         textures["jump"].has_flipped_ = false;
         textures["jump"].flip_ = SDL_FLIP_NONE;

         // Shooting arm and idle arm flip
         textures["idle_arm"].has_flipped_ = false;
         textures["idle_arm"].flip_ = SDL_FLIP_NONE;
         textures["arm_throw"].has_flipped_ = false;
         textures["arm_throw"].flip_ = SDL_FLIP_NONE;
         textures["running_arm"].has_flipped_ = false;
         textures["running_arm"].flip_ = SDL_FLIP_NONE;

         // Pushing texture
         textures["push"].has_flipped_ = false;
         textures["push"].flip_ = SDL_FLIP_NONE;
         textures["jump_push"].has_flipped_ = false;
         textures["jump_push"].flip_ = SDL_FLIP_NONE;
      }   

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
      } else if (player_state_ == PUSH && entity_direction == LEFT) {
         player_state_ = RUN;
         in_contact = false;
      }
            
      // Set direction
      entity_direction = RIGHT;
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
         const b2Vec2 force = {0, 2.2f};
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

   // Update player state
   change_player_state();

   // Set previous position
   prev_pos_x_ = body->GetPosition().x; 
   prev_pos_y_ = body->GetPosition().y;
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
   load_image(textures, this, 62, 104, 5, 1.0f / 4.0f, "idle", "images/player/idle_no_arm.png", success);

   // Load player jumping idly
   load_image(textures, this, 62, 104, 15, 1.0f / 20.0f, "jump", "images/player/idle_jump_no_arm.png", success);

   // Load player running
   load_image(textures, this, 62, 104, 21, 1.0f / 20.0f, "running", "images/player/running_no_arm.png", success);

   // Load jump and run
   load_image(textures, this, 62, 104, 17, 1.0f / 20.0f, "running_jump", "images/player/running_jump_no_arm.png", success);

   // Load arm
   load_image(textures, this, 7, 24, 5, 1.0f / 20.0f, "idle_arm", "images/player/idle_arm.png", success);

   // Load shooting arm
   load_image(textures, this, 44, 33, 9, 1.0f / 20.0f, "arm_throw", "images/player/arm_throw.png", success);

   // Load Running arm
   load_image(textures, this, 7, 22, 4, 1.0f / 20.0f, "running_arm", "images/player/running_arm.png", success);

   // load Eraser
   load_image(textures, this, 21, 12, 3, 1.0f / 20.0f, "eraser", "images/player/eraser.png", success);

   // Load pushing animation
   load_image(textures, this, 62, 104, 16, 1.0f / 20.0f, "push", "images/player/push.png", success);

   // Load jump and push
   load_image(textures, this, 62, 104, 8, 1.0f / 20.0f, "jump_push", "images/player/jump_push.png", success);

   // Return success
   return success;
}

// Create projectile
Projectile* Player::create_projectile(int delta_x_r, int delta_x_l, int delta_y, int height, int width,
     bool owner, bool damage, Texture texture) {
   // First, create a new projectile
   Application *tmp = get_application();
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new Eraser(get_x() + get_width() + delta_x_r, get_y() + delta_y, 
            height, width, this, tmp);
   } else {
      proj = new Eraser(get_x() + delta_x_l, get_y() + delta_y, height, width, this, tmp);
   }

   // Set shot direction
   proj->shot_dir = entity_direction;

   // Return projectile reference
   return proj;
}

// Virtual destructor
Player::~Player() {
   textures["idle"].free();
   textures["running"].free();
   textures["running_jump"].free();
   textures["jump"].free();
   textures["idle_arm"].free();
   textures["arm_throw"].free();
   textures["running_arm"].free();
   textures["eraser"].free();
}

/*************** SENSOR CLASS *************************/
Sensor::Sensor(int height, int width, Player *entity, float center_x) :
   Element(0, 0, height, width, NULL), entity_(entity) {

   // Create box shape
   box.SetAsBox(width, height, {center_x, 0}, 0.0f);

   // Create fixture
   fixture_def.shape = &box;
   fixture_def.density = 0.0f;
   //fixture_def.isSensor = true;
   fixture_def.userData = this;

   // Attach fixture
   entity->body->CreateFixture(&fixture_def);
}

// Start contact function
void Sensor::start_contact(Element *element) {
   if (element->type() == "Platform") {
      entity_->in_contact = true;
      //std::cout << "SUCCESSFULLY HIT SENSOR" << std::endl;
      //entity_->set_player_state(entity_->PUSH);
   }
}

// End contact function
void Sensor::end_contact() {
   entity_->in_contact = false;
   //entity_->set_player_state(entity_->RUN);
   //std::cout << "ENDED CONTACT" << std::endl;
}