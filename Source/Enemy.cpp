#include <stdio.h>
#include <cmath>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <Box2D/Box2D.h>

#include "Enemy.h"
#include "Entity.h"
#include "Object.h"
#include "Texture.h"
#include "Application.h"

/********************* ENEMY IMPLEMENTATIONS ******************/

Enemy::Enemy(int x, int y, int height, int width, Application *application) :
   Entity(x, y, height, width, application), enemy_state_(IDLE), shoot_timer_(0) {}

Enemy::~Enemy() {}

/************** FECREEZ IMPLEMENTATIONS ********************/
Fecreez::Fecreez(int x, int y, Application *application) :
   Enemy(x, y, 92, 82, application), shift_(false) {

   // Set the hitboxes
   set_hitbox(x, y);

   // Set health
   health = 100;
}

// Load media function for fecreez
bool Fecreez::load_media() {
   // Flag for success
   bool success = true;

   // Load idle
   load_image(textures, this, 82, 92, 18, 1.0f / 20.0f, "idle", "images/enemies/Fecreez/fecreez_idle.png", success);

   // Load shoot
   load_image(textures, this, 82, 92, 7, 1.0f / 20.0f, "shoot", "images/enemies/Fecreez/fereez_shoot.png", success);

   // Load poojectile
   load_image(textures, this, 24, 15, 8, 1.0f / 20.0f, "poojectile", "images/enemies/Fecreez/poojectile.png", success);

   // Load death
   load_image(textures, this, 143, 92, 16, 1.0f / 20.0f, "death", "images/enemies/Fecreez/fecreez_death.png", success);

   // Return success
   return success;
}

void Fecreez::update(bool freeze) {
   // Move first
   move();

   // The animate
   animate();

   // Render enemy
   Texture *enemytexture = get_texture();

   // Render player
   render(enemytexture);
}

void Fecreez::move() {
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
         enemy_state_ = ATTACK;

         // Update timer
         ++shoot_timer_;

         // Shoot if timer goes off
         if (shoot_timer_ >= 50) {
            Projectile *tmp = create_projectile(15, -10, 70, 15, 24, 0, 10, textures["poojectile"]);
            tmp->body->SetGravityScale(0);
            shoot_timer_ = 0;
         }
      } else {
         enemy_state_ = IDLE;
      }
   } else {
      // Set state to death
      enemy_state_ = DEATH;

      // TODO: find better solution
      if (!shift_) {
         sub_x(31);
         shift_ = true;
      }
   }

   // Update frames
   animate();
}

// Animate function
void Fecreez::animate(Texture *tex, int reset, int max) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == ATTACK) {
      Element::animate(&textures["shoot"]);
   } else if (enemy_state_ == DEATH) {
      Element::animate(&textures["death"], 15);
   }
}

// Get texture function
Texture *Fecreez::get_texture() {
   // Get idle texture
   if (enemy_state_ == IDLE) {
      return &textures["idle"];
   }
   
   // Get shoot texture
   if (enemy_state_ == ATTACK) {
      return &textures["shoot"];
   }

   // Get death texture
   if (enemy_state_ == DEATH) {
      return &textures["death"];
   }
}

Fecreez::~Fecreez() {
   textures["idle"].free();
   textures["shoot"].free();
   textures["poojectile"].free();
   textures["death"].free();
}

/********************* ROSEA ENEMY ******************/
Rosea::Rosea(int x, int y, Application *application) :
   Enemy(x, y, 144, 189, application), 
   arms_still(x - 46, y - 118, 78, 122, application),
   arms_attack(x + 30, y - 230, 387, 168, application), hurt_counter_(0), arm_state_(0), in_bounds_(false),
   arm_heights_({{0, y - 110}, {1, y - 250}, {2, y - 325}, {3, y - 395}, 
         {4, y - 425}, {5, y - 425}, {6, y - 425}, 
         {7, y - 425}, {8, y - 425}, {9, y - 380}, {10, y - 270}, {11, y - 180}, 
         {12, y - 135}, {13, y - 110}, {14, y - 110}, {15, y - 110}}) {

   // Set hitbox for rosea body
   set_hitbox(x, y);

   // TODO: get separate elements for the arms (ie new elements and set is as dynamic body)
   arms_attack.set_hitbox(arms_attack.get_x(), arms_attack.get_y());

   // Set texture location
   arms_attack.textures["attack"].set_x(arms_attack.get_x());
   arms_attack.textures["attack"].set_y(arms_attack.get_y());

   // Set initial arm postion
   arms_attack.set_y(arm_heights_[0]);

   // Set health
   health = 100;

   // Set enemy state
   enemy_state_ = IDLE;
}

// Load media for rosea
bool Rosea::load_media() {
   // Flag for success
   bool success = true;

   // Load idle texture
   load_image(textures, this, 189, 144, 15, 1.0f / 20.0f, "idle", "images/enemies/Rosea/rosea_idle.png", success);

   // Load hurt texture
   load_image(textures, this, 189, 144, 15, 1.0f / 20.0f, "hurt", "images/enemies/Rosea/rosea_hurt.png", success);

   // Load arms_idle texture
   load_image(arms_still.textures, this, 112, 78, 15, 1.0f/ 20.0f, "arms_idle", "images/enemies/Rosea/arms_idle.png", success);

   // Load arms_hurt texture
   load_image(arms_still.textures, this, 112, 78, 15, 1.0f/ 20.0f, "arms_hurt", "images/enemies/Rosea/arms_hurt.png", success);

   // Load arms_hurt texture
   load_image(arms_attack.textures, this, 122, 387, 15, 1.0f / 20.0f, "attack", "images/enemies/Rosea/arms_attack.png", success);

   // Return success
   return success;
}

// Rosea update
void Rosea::update(bool freeze) {
   // Move first
   move();

   // The animate
   animate();

   // Render enemy
   Texture *enemytexture = get_texture();

   // Render enemy
   render(enemytexture);

   // Render arms
   if (enemy_state_ == IDLE) {
      arms_still.render(&arms_still.textures["arms_idle"]);
   } else if (enemy_state_ == HURT) {
      arms_still.render(&arms_still.textures["arms_hurt"]);
   } else if (enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      arms_attack.texture_render(&arms_attack.textures["attack"]);
   }
}

// Rosea move
void Rosea::move() {
   // Check for enemy_state_ hurt
   if (enemy_state_ == HURT) {
      // Increment hurt counter
      ++hurt_counter_;

      // Check for expiration
      if (hurt_counter_ >= 50 && arms_still.textures["arms_hurt"].completed_) {
         enemy_state_ = IDLE;
         hurt_counter_ = 0;
         arms_still.textures["arms_hurt"].completed_ = false;
      }
   }

   if (enemy_state_ == RETREAT) {
      arms_attack.set_y(arm_heights_[arms_attack.textures["attack"].frame_]);
      if (arms_attack.textures["attack"].frame_ > 14) {
         enemy_state_ = IDLE;
         arms_attack.textures["attack"].frame_ = 0;
         arms_attack.textures["attack"].completed_ = false;
         arms_attack.set_y(arm_heights_[14]);
      }
   }

   // Now, check to see if player is within bounds
   if (within_bounds()) {
      // Set in bounds to true
      if (!in_bounds_) {
         arm_state_ = 7;
         in_bounds_ = true;
      }

      // Set state to attack
      enemy_state_ = ATTACK;

      // Deactivate main body
      body->SetActive(false);

      // Temp var
      int temp = (arms_attack.textures["attack"].frame_ < arm_state_) ? 
                     arms_attack.textures["attack"].frame_ : arm_state_;

      // Sets hitbox to position of arm
      arms_attack.set_y(arm_heights_[temp]);
   } else {
      // Check to make sure enemy is done attacking
      if (in_bounds_) {
         enemy_state_ = RETREAT;
         arm_state_ = 0;
         in_bounds_ = false;
      }
   }
}

// Rosea animate
void Rosea::animate(Texture *tex, int reset, int max, int start) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
      Element::animate(&arms_still.textures["arms_idle"]);
   } else if (enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      Element::animate(&textures["idle"]);
      Element::animate(&arms_attack.textures["attack"], arm_state_, arm_state_);
   } else if (enemy_state_ == HURT) {
      Element::animate(&textures["hurt"]);
      Element::animate(&arms_still.textures["arms_hurt"]);
   }
}

// Rosea get texture
Texture* Rosea::get_texture() {
   // Get idle texture
   if (enemy_state_ == IDLE || enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      return &textures["idle"];
   }
   
   // Get hurt texture for main body
   if (enemy_state_ == HURT) {
      return &textures["hurt"];
   }
}

// Get contact
void Rosea::start_contact() {
   // Set enemy state to hurt
   enemy_state_ = HURT;
}

// Check to see if player is within bounds
bool Rosea::within_bounds() {
   if (get_application()->get_player()->get_x() >= get_x() - 200 
      && get_application()->get_player()->get_x() <= get_x() + 200) {
      return true;
   }
   return false;
}

// Checks to see what side player is on
bool Rosea::left_bound() {
   return (get_application()->get_player()->get_x() >= get_x() - 200) && 
            (get_application()->get_player()->get_x() < get_x());
}
bool Rosea::right_bound() {
   return (get_application()->get_player()->get_x() <= get_x() + 200) &&
            (get_application()->get_player()->get_x() > get_x());
}

// Checks to see if player is leaving bounds
bool Rosea::is_leaving_bounds() {
   if (within_bounds()) {
      if (get_application()->get_player()->get_x() + 
            get_application()->get_player()->body->GetLinearVelocity().x < get_x() - 200) {
         return true;
      }
      if (get_application()->get_player()->get_x() +
            get_application()->get_player()->body->GetLinearVelocity().x > get_x() + 200) {
         return true;
      }
   }
   return false;
}

// Rosea destructor
Rosea::~Rosea() {
   textures["idle"].free();
   textures["hurt"].free();
   arms_still.textures["arms_idle"].free();
   arms_still.textures["arms_hurt"].free();
   arms_attack.textures["arms_attack"].free();
}

/************** MOSQUIBLER ENEMY *******************/
Mosquibler::Mosquibler(int x, int y, Application *application) :
   Enemy(x, y, 97, 109, application) {

   // Set hitbox
   set_hitbox(x, y, true);
 
   // Set health
   health = 100;

   // Set initial dir
   entity_direction = LEFT;

   // Set enemy state
   enemy_state_ = IDLE;  
}

// Load media function
bool Mosquibler::load_media() {
   // Flag for success
   bool success = true;

   // Load fly for mosquibler
   load_image(textures, this, 109, 97, 12, 1.0f / 20.0f, "fly", "images/enemies/Mosquibler/mosquibler_fly.png", success);

   // Load death for mosquibler
   load_image(textures, this, 109, 97, 28, 1.0f / 20.0f, "death", "images/enemies/Mosquibler/mosquibler_death.png", success);

   // Load turn texture
   load_image(textures, this, 109, 97, 12, 1.0f / 20.0f, "turn", "images/enemies/Mosquibler/mosquibler_turn.png", success);

   // Return success
   return success;
}

// Update
void Mosquibler::update(bool freeze) {
   // Move first
   move();

   // The animate
   animate();

   // Render enemy
   Texture *enemytexture = get_texture();

   // Render enemy
   render(enemytexture);

   // Render arms
   if (enemy_state_ == IDLE) {
      render(&textures["fly"]);
   } else if (enemy_state_ == TURN) {
      render(&textures["turn"]);
   } else if (enemy_state_ == DEATH) {
      render(&textures["death"]);
   } 
}

// Move function
void Mosquibler::move() {
   // Check to see what direction the enemy should be facing
   if (get_application()->get_player()->get_x() <= get_x() && entity_direction == RIGHT) {
      entity_direction = LEFT;
      enemy_state_ = TURN;
   } else if (get_application()->get_player()->get_x() > get_x() && entity_direction == LEFT) {
      entity_direction = RIGHT;
      enemy_state_ = TURN;
   }

   // Check to see if has collided
   if (!is_alive()) {
      enemy_state_ = DEATH;
   }

   // Turn around
   if (enemy_state_ == TURN) {
      // Magnitude of impulse
      float magnitude = sqrt(pow((get_application()->get_player()->body->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                              + pow((get_application()->get_player()->body->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

      // Get a vector towards the player and apply as impulse
      const b2Vec2 impulse = {(get_application()->get_player()->body->GetPosition().x - body->GetPosition().x) / magnitude * 0.90f, 
                       (get_application()->get_player()->body->GetPosition().y - body->GetPosition().y) / magnitude * 0.90f};
      
      // Apply impulse
      body->SetLinearVelocity(impulse);

      // Complete texture
      if (textures["turn"].frame_ > 11) {
         if (entity_direction == RIGHT) {
            textures["fly"].flip_ = SDL_FLIP_HORIZONTAL;
            textures["turn"].flip_ = SDL_FLIP_HORIZONTAL;
         } else if (entity_direction == LEFT) {
            textures["fly"].flip_ = SDL_FLIP_NONE;
            textures["turn"].flip_ = SDL_FLIP_NONE;
         }
         textures["turn"].completed_ = false;
         textures["turn"].frame_ = 0;
         enemy_state_ = IDLE;
      }
   }

   // Fly towards player
   if (enemy_state_ == IDLE) {
      // Magnitude of impulse
      float magnitude = sqrt(pow((get_application()->get_player()->body->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                              + pow((get_application()->get_player()->body->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

      // Get a vector towards the player and apply as impulse
      const b2Vec2 impulse = {(get_application()->get_player()->body->GetPosition().x - body->GetPosition().x) / magnitude * 0.90f, 
                       (get_application()->get_player()->body->GetPosition().y - body->GetPosition().y) / magnitude * 0.90f};
      
      // Apply impulse
      //body->ApplyLinearImpulseToCenter(impulse, true);
      body->SetLinearVelocity(impulse);

      //std::cout << "x: " << get_application()->get_player()->body->GetPosition().x - body->GetPosition().x << " y: " << get_application()->get_player()->body->GetPosition().y - body->GetPosition().y << std::endl;
   }
}

// Animate function
void Mosquibler::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["fly"]);
   } else if (enemy_state_ == TURN) {
      Element::animate(&textures["turn"]);
   } else if (enemy_state_ == DEATH) {
      Element::animate(&textures["death"]);
   }
}

// Get texture function
Texture* Mosquibler::get_texture() {
   // Get idle texture
   if (enemy_state_ == IDLE) {
      return &textures["fly"];
   }

   // Get turn texture
   if (enemy_state_ == TURN) {
      return &textures["turn"];
   }

   // Get death texture
   if (enemy_state_ == DEATH) {
      return &textures["death"];
   }
}

// Start contact function
void Mosquibler::start_contact() {
   alive = false;
}

// Destructor
Mosquibler::~Mosquibler() {
   textures["fly"].free();
   textures["death"].free();
   textures["turn"].free();
}