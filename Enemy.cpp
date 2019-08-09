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
   Entity(x, y, height, width, application), enemy_state_(IDLE), shoot_timer_(101) {

   // Set
   start_death_ = 0;
   end_death_ = 0;

   // Projectile
   proj_ = nullptr;
}

// Create projectile
// TODO: Create a struct that holds all of the ending parameters and pass it through
//       such as to properly load each texture properly (one enemy has different sizes for textures)
Projectile* Enemy::create_projectile(int delta_x_r, int delta_x_l, int delta_y, 
      bool owner, bool damage, float force_x, float force_y, 
      const TextureData &normal, const TextureData &hit) {
   // First, create a new projectile
   Application *tmp = get_application();
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new EnemyProjectile(get_x() + get_width() + delta_x_r, get_y() + delta_y, 
            damage, force_x, force_y, normal, hit, this, tmp);
   } else {
      proj = new EnemyProjectile(get_x() + delta_x_l, get_y() + delta_y,
            damage, force_x, force_y, normal, hit, this, tmp);
   }

   // Set shot direction
   proj->shot_dir = entity_direction;

   // Return projectile reference
   return proj;
}

// Update function
void Enemy::update(bool freeze) {
   // Move first
   move();

   // The animate
   animate();

   // Render enemy
   Texture *enemytexture = get_texture();

   // Render player
   render(enemytexture);
}

Texture *Enemy::get_texture() {
   // Get idle texture
   if (enemy_state_ == IDLE) {
      return &textures["idle"];
   }
   
   // Get attack texture
   if (enemy_state_ == ATTACK) {
      return &textures["attack"];
   }

   // Get death texture
   if (enemy_state_ == DEATH) {
      return &textures["death"];
   }

   // Get turn texture
   if (enemy_state_ == TURN) {
      return &textures["turn"];
   }
}

bool Enemy::within_bounds() {
   return get_application()->get_player()->get_y() >= get_y() - get_height() &&
            get_application()->get_player()->get_y() <= get_y() + get_height();
}

Enemy::~Enemy() {}

/************** FECREEZ IMPLEMENTATIONS ********************/
Fecreez::Fecreez(int x, int y, Application *application) :
   Enemy(x, y, 92, 82, application) {

   // Set the hitboxes
   set_hitbox(x, y);

   // Set health
   health = 30;

   // Set entity direction to right
   entity_direction = LEFT;
}

// Load media function for fecreez
bool Fecreez::load_media() {
   // Flag for success
   bool success = true;

   // Load idle
   load_image(textures, this, 82, 92, 18, 1.0f / 20.0f, "idle", "images/enemies/Fecreez/fecreez_idle.png", success);

   // Load attack
   load_image(textures, this, 82, 92, 7, 1.0f / 20.0f, "attack", "images/enemies/Fecreez/fecreez_shoot.png", success);

   // Load death
   load_image(textures, this, 143, 92, 16, 1.0f / 20.0f, "death", "images/enemies/Fecreez/fecreez_death.png", success);

   // Load turn
   load_image(textures, this, 82, 92, 7, 1.0f / 20.0f, "turn", "images/enemies/Fecreez/fecreez_turn.png", success);

   // Start flipped
   textures["idle"].flip_ = SDL_FLIP_HORIZONTAL;
   textures["attack"].flip_ = SDL_FLIP_HORIZONTAL;
   textures["poojectile"].flip_ = SDL_FLIP_HORIZONTAL;
   textures["turn"].flip_ = SDL_FLIP_HORIZONTAL;

   // Return success
   return success;
}

void Fecreez::move() {
   // Overall check to see if it's alive
   if (alive) {
      // Check to see what direction the enemy should be facing
      if (enemy_state_ != DEATH && within_bounds()) {
         // Turn if direction changed
         if (get_application()->get_player()->get_x() <= get_x() 
            && entity_direction == RIGHT) {
            enemy_state_ = TURN;
            entity_direction = LEFT;
         } else if (get_application()->get_player()->get_x() > get_x() 
            && entity_direction == LEFT) {
            enemy_state_ = TURN;
            entity_direction = RIGHT;
         }
      }

      // Turn
      if (enemy_state_ == TURN) {
         // Complete texture
         if (textures["turn"].frame_ > 6) {
            if (entity_direction == RIGHT) {
               textures["turn"].flip_ = SDL_FLIP_NONE;
               textures["idle"].flip_ = SDL_FLIP_NONE;
               textures["attack"].flip_ = SDL_FLIP_NONE;
            } else if (entity_direction == LEFT) {
               textures["turn"].flip_ = SDL_FLIP_HORIZONTAL;
               textures["idle"].flip_ = SDL_FLIP_HORIZONTAL;
               textures["attack"].flip_ = SDL_FLIP_HORIZONTAL;
            }
            textures["turn"].completed_ = false;
            textures["turn"].frame_ = 0;
            enemy_state_ = IDLE;
         }
      }

      // Check to see if get_player() within bounds of enemy
      if (get_application()->get_player()->get_y() >= get_y() - get_height() &&
            get_application()->get_player()->get_y() <= get_y() + get_height()
            && enemy_state_ != TURN) {
         // Set state to attack
         if (shoot_timer_ >= 100) {
            enemy_state_ = ATTACK;
         } else if (shoot_timer_ < 100 && textures["attack"].frame_ > 6) {
            enemy_state_ = IDLE;
            textures["attack"].frame_ = 0;
         }

         // Update timer
         ++shoot_timer_;
      } else if (enemy_state_ != TURN) {
         enemy_state_ = IDLE;
         textures["attack"].frame_ = 0;
      }

      // attack
      if (enemy_state_ == ATTACK) {
         if (textures["attack"].frame_ > 6 && shoot_timer_ >= 100) {
            TextureData normal = {15, 24, 8};
            TextureData hit = {15, 24, 8};
            Projectile *tmp = create_projectile(15, -10, 70, 0, 10, 10.4f, 0.0f, normal, hit);
            tmp->body->SetGravityScale(0);
            shoot_timer_ = 0;
         }
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

// Start contact function
void Fecreez::start_contact(Element *element) {
   if (element && (element->type() == "Player" || element->type() == "Projectile")) {
      health -= 10;
      if (health <= 0) {
         alive = false;
      }
   }
}

// Animate function
void Fecreez::animate(Texture *tex, int reset, int max) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == ATTACK) {
      Element::animate(&textures["attack"], textures["attack"].reset_frame, textures["attack"].stop_frame);
   } else if (enemy_state_ == DEATH) {
      Element::animate(&textures["death"], 15);
   } else if (enemy_state_ == TURN) {
      Element::animate(&textures["turn"]);
   }
}

Fecreez::~Fecreez() {
}

//////////////////////////////////////////////////////
/********************* ARM **************************/
//////////////////////////////////////////////////////

Arm::Arm(int x, int y, int height, int width, Application *application, Rosea *rosea) :
   Enemy(x, y, height, width, application), rosea_(rosea) {}

// Callback function for arm will set rosea's state to hurt
void Arm::start_contact(Element *element) {
   if (element->type() == "Player") {
      rosea_->get_application()->get_player()->take_damage(10);
   } else if (element->type() == "Projectile") {
      if (rosea_->get_state() != ATTACK) {
         rosea_->set_state(HURT);
      }
   }
}

//////////////////////////////////////////////////////
/****************** ROSEA ENEMY *********************/
//////////////////////////////////////////////////////

Rosea::Rosea(int x, int y, Application *application) :
   Enemy(x, y, 144, 189, application), 
   arms_still(x - 46, y - 118, 78, 122, application, this),
   arms_attack(x + 30, y - 230, 387, 168, application, this), 
   hurt_counter_(0), arm_state_(0), in_bounds_(false),
   arm_heights_({{0, y - 110}, {1, y - 250}, {2, y - 325}, {3, y - 395}, 
         {4, y - 425}, {5, y - 425}, {6, y - 425}, 
         {7, y - 425}, {8, y - 425}, {9, y - 380}, {10, y - 270}, {11, y - 180}, 
         {12, y - 135}, {13, y - 110}, {14, y - 110}, {15, y - 110}}) {

   // Set hitbox for rosea body
   set_hitbox(x, y);

   // TODO: get separate elements for the arms (ie new elements and set is as dynamic body)
   //std::cout << "Before: x = " << arms_attack.get_x() << " y = " << arms_attack.get_y() << std::endl;
   arms_attack.set_hitbox(arms_attack.get_x(), arms_attack.get_y());
   arms_still.set_hitbox(arms_still.get_x() + 61, arms_still.get_y() + 39);
   //std::cout << "After: x = " << arms_attack.get_x() << " y = " << arms_attack.get_y() << std::endl;

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

   // Render arms
   if (enemy_state_ == IDLE) {
      arms_still.render(&arms_still.textures["arms_idle"]);
   } else if (enemy_state_ == HURT) {
      arms_still.render(&arms_still.textures["arms_hurt"]);
   } else if (enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      arms_attack.texture_render(&arms_attack.textures["attack"]);
   }

   // Render enemy
   render(enemytexture);
}

// Rosea move
void Rosea::move() {
   // Check for enemy_state_ hurt
   if (enemy_state_ == HURT) {
      // Set arm height to 0
      arms_attack.set_y(arm_heights_[0]);

      // Increment hurt counter
      ++hurt_counter_;

      // Check for expiration
      if (hurt_counter_ >= 50 && arms_still.textures["arms_hurt"].completed_) {
         enemy_state_ = IDLE;
         hurt_counter_ = 0;
         arms_still.textures["arms_hurt"].completed_ = false;
      }
   } else if (enemy_state_ == RETREAT) {
      arms_attack.set_y(arm_heights_[arms_attack.textures["attack"].frame_]);
      if (arms_attack.textures["attack"].frame_ > 14) {
         enemy_state_ = IDLE;
         arms_attack.textures["attack"].frame_ = 0;
         arms_attack.textures["attack"].completed_ = false;
         arms_attack.set_y(arm_heights_[14]);
      }
   }

   // Now, check to see if player is within bounds
   if (enemy_state_ != HURT) {
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
}

// Rosea animate
void Rosea::animate(Texture *tex, int reset, int max, int start) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      Element::animate(&arms_still.textures["arms_idle"]);
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      Element::animate(&arms_attack.textures["attack"], arm_state_, arm_state_);
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == HURT) {
      Element::animate(&arms_still.textures["arms_hurt"]);
      Element::animate(&textures["hurt"]);
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
void Rosea::start_contact(Element *element) {
   // Set enemy state to hurt
   if (element && element->type() == "Projectile") {
      if (enemy_state_ != ATTACK) {
         enemy_state_ = HURT;
      }
   }
}

// Check to see if player is within bounds
bool Rosea::within_bounds() {
   if (get_application()->get_player()->get_x() >= get_x() - 175 
      && get_application()->get_player()->get_x() <= get_x() + 300) {
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
Rosea::~Rosea() {}

/////////////////////////////////////////////////////
/************** MOSQUIBLER ENEMY *******************/
/////////////////////////////////////////////////////

Mosquibler::Mosquibler(int x, int y, Application *application) :
   Enemy(x, y, 89, 109, application) {

   // Set hitbox
   set_hitbox(x, y, true);
 
   // Set health
   health = 10;

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
   load_image(textures, this, 109, 89, 27, 1.0f / 20.0f, "death", "images/enemies/Mosquibler/mosquibler_death.png", success);

   // Load turn texture
   load_image(textures, this, 109, 97, 12, 1.0f / 20.0f, "turn", "images/enemies/Mosquibler/mosquibler_turn.png", success);

   // Return success
   return success;
}

// Move function
void Mosquibler::move() {
   // Check to see what direction the enemy should be facing
   if (enemy_state_ != DEATH) {
      if (get_application()->get_player()->get_x() <= get_x() && entity_direction == RIGHT) {
         entity_direction = LEFT;
         enemy_state_ = TURN;
      } else if (get_application()->get_player()->get_x() > get_x() && entity_direction == LEFT) {
         entity_direction = RIGHT;
         enemy_state_ = TURN;
      }
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
      body->SetLinearVelocity(impulse);
   }

   // In state death
   if (enemy_state_ == DEATH) {
      if (flag_ && body) {
         get_application()->world_.DestroyBody(body);
         body = nullptr;
         flag_ = false;
      }

      if (textures["death"].frame_ > 10 && is_alive() && start_death_ < 7) {
         start_death_ = 6;
         end_death_ = 10;
      } 
      
      if (textures["death"].frame_ > 26 && start_death_ >= 12) {
         alive = false;
         start_death_ = 26;
         end_death_ = 26;
      }
   }
}

// Animate function
void Mosquibler::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["fly"]);
   } else if (enemy_state_ == TURN) {
      Element::animate(&textures["turn"]);
   } else if (enemy_state_ == DEATH) {
      Element::animate(&textures["death"], start_death_, end_death_);
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
void Mosquibler::start_contact(Element *element) {
   if (element && (element->type() == "Player" || element->type() == "Projectile") && enemy_state_ != DEATH) {
      // TODO: loop falling animation
      enemy_state_ = DEATH;
      start_death_ = 0;
      end_death_ = 10;
   } if (element && enemy_state_ == DEATH && element->type() == "Platform") {
      // TODO: play the death part instead
      flag_ = true;
      start_death_ = 12;
      end_death_ = 26;
   }
}

// Destructor
Mosquibler::~Mosquibler() {}

///////////////////////////////////////////////////
/************* FRUIG ENEMY ***********************/
///////////////////////////////////////////////////

// Constructor
Fruig::Fruig(int x, int y, Application *application) :
   Enemy(x, y, 140, 79, application) {

   // Set hitbox
   set_hitbox(x, y);

   // Set health
   health = 10;

   // Set initial dir
   entity_direction = RIGHT;

   // Set enemy state
   enemy_state_ = IDLE;
}

// Load media function
bool Fruig::load_media() {
   bool success = true;

   // Load idle
   load_image(textures, this, 79, 140, 20, 1.0f / 20.0f, "idle", "images/enemies/Fruig/idle.png", success);

   // Load death
   load_image(textures, this, 85, 136, 20, 1.0f / 20.0f, "death", "images/enemies/Fruig/death.png", success);

   // Return success
   return success;
}

// Mpve function
void Fruig::move() {
   // Check death state first
   if (enemy_state_ == DEATH) {
      // Destroy body
      if (body) {
         get_application()->world_.DestroyBody(body);
         body = nullptr;
      }

      // Set animation
      if (textures["death"].frame_ >= 14) {
         start_death_ = 14;
         end_death_ = 19;
      }
   }

   // Check if state is idle?
   if (enemy_state_ == IDLE) {
      // Let goop fall if it reaches the correct point in the animation.
      if (textures["idle"].frame_ == 8 && shoot_timer_ > 20) {
         TextureData normal = {9, 10, 5};
         TextureData hit = {9, 65, 9};
         proj_ = create_projectile(-30, 0, 170, 10, 10, 0.0f, 0.0f, normal, hit);
         shoot_timer_ = 0;
      }

      // Update shoot timer
      shoot_timer_++;

      // Check for death of proj
      if (proj_ && !proj_->shift && proj_->get_state() == Object::DEAD) {
         proj_->sub_x(24);
         proj_->shift = true;
      }
   }
}

// Animate function
void Fruig::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == DEATH) {
      Element::animate(&textures["death"], start_death_, end_death_);
   }
}

// Get contact function
void Fruig::start_contact(Element *element) {
   if ((element->type() == "Player" || element->type() == "Projectile")) {
      health -= 10;
      if (health <= 0) {
         enemy_state_ = DEATH;
         start_death_ = 14;
         end_death_ = 19;
      }
   }
}

///////////////////////////////////////////////////
/************* FLEET ENEMY ***********************/
///////////////////////////////////////////////////

// Constructor
Fleet::Fleet(int x, int y, Application *application) :
   Enemy(x, y, 46, 53, application) {
   
   // Set the hitbox
   set_hitbox(x, y, true);

   // Set health
   health = 30;

   // Set initial dir
   entity_direction = RIGHT;

   // Set enemy state
   enemy_state_ = IDLE;
}

// Load media function
bool Fleet::load_media() {
   bool success = true;

   // Load idle
   load_image(textures, this, 63, 87, 16, 1.0f / 20.0f, "idle", "images/enemies/Fleet/idle.png", success);

   // Load death
   load_image(textures, this, 63, 87, 11, 1.0f / 20.0f, "turn", "images/enemies/Fleet/turn.png", success);

   // Return success
   return success;
}

// Move function
void Fleet::move() {
   if (alive) {
      // Check for physical turn
      if ((get_application()->get_player()->get_x() < get_x() && entity_direction == RIGHT) 
         || (get_application()->get_player()->get_x() > get_x() && entity_direction == LEFT)) {
         
         // Set state to turn
         enemy_state_ = TURN;
      }

      // Check for turn state
      if (enemy_state_ == TURN) {
         // Complete texture
         if (textures["turn"].frame_ > 10) {
            if (entity_direction == RIGHT) {
               textures["turn"].flip_ = SDL_FLIP_NONE;
               textures["idle"].flip_ = SDL_FLIP_NONE;
            } else if (entity_direction == LEFT) {
               textures["turn"].flip_ = SDL_FLIP_HORIZONTAL;
               textures["idle"].flip_ = SDL_FLIP_HORIZONTAL;
            }
            textures["turn"].completed_ = false;
            textures["turn"].frame_ = 0;
            enemy_state_ = IDLE;
         }
      }

      // Idle is him just hopping around towards the player
      if (enemy_state_ == IDLE) {
         // Check to see if in bounds
         if (within_bounds()) {
            // Magnitude of impulse
            float magnitude = sqrt(pow((get_application()->get_player()->body->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                                    + pow((get_application()->get_player()->body->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

            // Get a vector towards the player and apply as impulse
            const b2Vec2 impulse = {(get_application()->get_player()->body->GetPosition().x - body->GetPosition().x) / magnitude * 0.90f, 
                           (get_application()->get_player()->body->GetPosition().y - body->GetPosition().y) / magnitude * 0.90f};
            
            // Apply impulse
            body->SetLinearVelocity(impulse);
         }
      }
   }
}

// Animate function
void Fleet::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == TURN) {
      Element::animate(&textures["turn"]);
   }
}

// Start contact function
void Fleet::start_contact(Element *element) {
   if (element && (element->type() == "Player" || element->type() == "Projectile")) {
      health -= 10;
      if (health <= 0) {
         alive = false;
      }
   }
}