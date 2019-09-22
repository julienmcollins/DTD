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
#include "Level.h"
#include "Global.h"

/********************* ENEMY IMPLEMENTATIONS ******************/

Enemy::Enemy(int x, int y, int height, int width, Application *application) :
   Entity(x, y, height, width, application), enemy_state_(IDLE), shoot_timer_(101) {

   // Set
   start_death_ = 0;
   end_death_ = 0;

   // Projectile
   proj_ = nullptr;
}

// Get enemy state
Enemy::STATE Enemy::get_enemy_state() const {
   return enemy_state_;
}

// Create projectile
// TODO: Create a struct that holds all of the ending parameters and pass it through
//       such as to properly load each texture (one enemy has different sizes for textures)
// TODO: Do it based on get_x() instead of get_tex_x() (will need to change consts)
Projectile* Enemy::create_projectile(int delta_x_r, int delta_x_l, int delta_y, 
      bool owner, bool damage, float force_x, float force_y, 
      const TextureData &normal, const TextureData &hit) {
   // First, create a new projectile
   Application *tmp = get_application();
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new EnemyProjectile(get_tex_x() + get_width() + delta_x_r, get_tex_y() + delta_y, 
            damage, force_x, force_y, normal, hit, this, tmp);
   } else {
      proj = new EnemyProjectile(get_tex_x() + delta_x_l, get_tex_y() + delta_y,
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
   load_image(82, 92, 18, 1.0f / 20.0f, "idle", "images/enemies/Fecreez/fecreez_idle.png", success);

   // Load attack
   load_image(82, 92, 9, 1.0f / 20.0f, "attack", "images/enemies/Fecreez/fecreez_shoot.png", success);

   // Load death
   load_image(143, 92, 16, 1.0f / 20.0f, "death", "images/enemies/Fecreez/fecreez_death.png", success);

   // Load turn
   load_image(82, 92, 7, 1.0f / 20.0f, "turn", "images/enemies/Fecreez/fecreez_turn.png", success);

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
   if (enemy_state_ == DEATH) {
      // TODO: find better solution
      if (!shift_) {
         sub_tex_x(-7);
         shift_ = true;
      }
   }

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
      && enemy_state_ != TURN && enemy_state_ != DEATH) {
      if (shoot_timer_ >= 100) {
         enemy_state_ = ATTACK;
      } else if (shoot_timer_ < 100 && textures["attack"].frame_ > 8) {
         enemy_state_ = IDLE;
         textures["attack"].frame_ = 0;
      }
      ++shoot_timer_;
   } else if (enemy_state_ != TURN && enemy_state_ != DEATH && enemy_state_ != ATTACK) {
      enemy_state_ = IDLE;
   }

   // attack
   if (enemy_state_ == ATTACK) {
      if (textures["attack"].frame_ > 4 && shoot_timer_ >= 100) {
         TextureData normal(24, 15, 8);
         TextureData hit(24, 15, 8);
         Projectile *tmp = create_projectile(15, -10, 70, 0, 10, 10.4f, 0.0f, normal, hit);
         tmp->body->SetGravityScale(0);
         shoot_timer_ = 0;
      }
      if (textures["attack"].frame_ > 8) {
         enemy_state_ = IDLE;
         textures["attack"].frame_ = 0;
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
         //alive = false;
         enemy_state_ = DEATH;
         set_collision(CAT_PLATFORM);
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

Rosea::Rosea(int x, int y, float angle, Application *application) :
   Enemy(x, y, 144, 189, application), 
   arms_still(x - 46, y - 118, 78, 122, application, this),
   arms_attack(x + 5, y - 230, 387, 122, application, this), 
   hurt_counter_(0), arm_state_(0), in_bounds_(false), angle_(angle),
   arm_heights_({{0, y - 110}, {1, y - 250}, {2, y - 325}, {3, y - 395}, 
         {4, y - 425}, {5, y - 425}, {6, y - 425}, 
         {7, y - 425}, {8, y - 425}, {9, y - 380}, {10, y - 270}, {11, y - 180}, 
         {12, y - 135}, {13, y - 110}, {14, y - 110}, {15, y - 110}}),
   arm_widths_({{0, x - 425}, {1, x - 395}, {2, x - 325}, {3, x - 250}, 
         {4, x - 80}, {5, x - 80}, {6, x - 80}, 
         {7, x - 80}, {8, x - 80}, {9, x - 180}, {10, x - 270}, {11, x - 380}, 
         {12, x - 400}, {13, x - 425}, {14, x - 425}, {15, x - 425}}) {

   // Set anchors
   anchor_x = x;
   anchor_y = y;
   element_shape.shape_type.square.angle = angle;

   // Special state for 0 angle
   if (angle == 0.0f) {
      // Set hitbox for rosea body
      set_hitbox(x, y);

      arms_attack.set_hitbox(arms_attack.get_tex_x(), arms_attack.get_tex_y());
      arms_still.set_hitbox(arms_still.get_tex_x() + 61, arms_still.get_tex_y() + 39);

      // TODO: get separate elements for the arms (ie new elements and set is as dynamic body)
      // Set texture location
      arms_attack.textures["attack"].set_x(arms_attack.get_tex_x());
      arms_attack.textures["attack"].set_y(arms_attack.get_tex_y());

      // Set initial arm postion
      arms_attack.set_tex_y(arm_heights_[0]);
   }
   
   // Need to change arm position if rotated
   if (angle == 90.0f) {
      // Set width and height of main body
      set_width(144);
      set_height(189);

      // Set hitboxes
      set_hitbox(x - 72, y + 94);

      // Set arms still height, width, x and y
      arms_still.set_width(78);
      arms_still.set_height(112);
      arms_still.set_y(y + 50);
      arms_still.set_x(x + 47);

      // Set height and width
      arms_attack.set_width(387);
      arms_attack.set_height(122);
      arms_attack.set_tex_y(y + 37);
      arms_attack.set_tex_x(x + 310);

      arms_attack.textures["attack"].set_x(arms_attack.get_x());
      arms_attack.textures["attack"].set_y(arms_attack.get_y());

      arms_still.textures["arms_idle"].set_x(arms_still.get_x());
      arms_still.textures["arms_idle"].set_y(arms_still.get_y());

      arms_still.textures["arms_hurt"].set_x(arms_still.get_x());
      arms_still.textures["arms_hurt"].set_y(arms_still.get_y());

      arms_attack.set_hitbox(arms_attack.get_tex_x(), arms_attack.get_tex_y());
      arms_still.set_hitbox(arms_still.get_tex_x() - 70, arms_still.get_tex_y() + 60);

      arms_attack.set_tex_y(y + 40);
      arms_attack.set_tex_x(arm_widths_[0]);
      
      // Set texture angle
      textures["idle"].angle = angle;
      textures["hurt"].angle = angle;
      arms_still.textures["arms_idle"].angle = angle;
      arms_still.textures["arms_hurt"].angle = angle;
      arms_attack.textures["attack"].angle = angle;

      // Set texture centers?
      textures["idle"].center_ = {0, 0};
      textures["hurt"].center_ = {0, 0};
      arms_still.textures["arms_idle"].center_ = {0, 0};
      arms_still.textures["arms_hurt"].center_ = {0, 0};
      arms_attack.textures["attack"].center_ = {0, 0};
   }

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
   load_image(189, 144, 15, 1.0f / 20.0f, "idle", "images/enemies/Rosea/rosea_idle.png", success);

   // Load hurt texture
   load_image(189, 144, 15, 1.0f / 20.0f, "hurt", "images/enemies/Rosea/rosea_hurt.png", success);

   // Load arms_idle texture
   arms_still.load_image(112, 78, 15, 1.0f/ 20.0f, "arms_idle", "images/enemies/Rosea/arms_idle.png", success);

   // Load arms_hurt texture
   arms_still.load_image(112, 78, 15, 1.0f/ 20.0f, "arms_hurt", "images/enemies/Rosea/arms_hurt.png", success);

   // Load arms_hurt texture
   arms_attack.load_image(122, 387, 15, 1.0f / 20.0f, "attack", "images/enemies/Rosea/arms_attack.png", success);

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
      if (angle_ == 0.0f) {
         arms_still.render(&arms_still.textures["arms_idle"]);
      } else {
         arms_still.texture_render(&arms_still.textures["arms_idle"]);
      }
   } else if (enemy_state_ == HURT) {
      if (angle_ == 0.0f) {
         arms_still.render(&arms_still.textures["arms_hurt"]);
      } else {
         arms_still.texture_render(&arms_still.textures["arms_hurt"]);
      }
   } else if (enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      arms_attack.texture_render(&arms_attack.textures["attack"]);
   }

   // Render enemy
   if (angle_ == 0.0f) {
      render(enemytexture);
   } else {
      render(enemytexture, anchor_x, anchor_y);
   }
}

// Rosea move
void Rosea::move() {
   // Check for enemy_state_ hurt
   if (enemy_state_ == HURT) {
      // Set arm height to 0
      if (angle_ == 0.0f) {
         arms_attack.set_tex_y(arm_heights_[0]);
      } else {
         arms_attack.set_tex_x(arm_widths_[0]);
      }

      // Increment hurt counter
      ++hurt_counter_;

      // Check for expiration
      if (hurt_counter_ >= 50 && arms_still.textures["arms_hurt"].completed_) {
         enemy_state_ = IDLE;
         hurt_counter_ = 0;
         arms_still.textures["arms_hurt"].completed_ = false;
      }
   } else if (enemy_state_ == RETREAT) {
      if (angle_ == 0.0f) {
         arms_attack.set_tex_y(arm_heights_[arms_attack.textures["attack"].frame_]);
      } else {
         arms_attack.set_tex_x(arm_widths_[arms_attack.textures["attack"].frame_]);
      }
      if (arms_attack.textures["attack"].frame_ > 14) {
         enemy_state_ = IDLE;
         arms_attack.textures["attack"].frame_ = 0;
         arms_attack.textures["attack"].completed_ = false;
         if (angle_ == 0.0f) {
            arms_attack.set_tex_y(arm_heights_[14]);
         } else {
            arms_attack.set_tex_x(arm_widths_[14]);
         }
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
         if (angle_ == 0.0f) {
            arms_attack.set_tex_y(arm_heights_[temp]);
         } else {
            arms_attack.set_tex_x(arm_widths_[temp]);
         }
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
   if (angle_ == 0.0f) {
      if (get_application()->get_player()->get_x() >= get_x() - 250 
         && get_application()->get_player()->get_x() <= get_x() + 250) {
         return true;
      }
   } else {
      if (get_application()->get_player()->get_y() >= (get_y() - 250) 
         && get_application()->get_player()->get_y() <= (get_y() + 250)
         && get_application()->get_player()->get_x() <= (get_x() + 400)) {
         return true;
      }
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

   // Set element shape stuff
   element_shape.dynamic = true;

   // Set hitbox
   set_hitbox(x, y, SQUARE, 2);
 
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
   load_image(109, 97, 12, 1.0f / 20.0f, "fly", "images/enemies/Mosquibler/fly.png", success);

   // Load death for mosquibler
   load_image(109, 89, 27, 1.0f / 20.0f, "death", "images/enemies/Mosquibler/death.png", success);

   // Load turn texture
   load_image(109, 97, 12, 1.0f / 20.0f, "turn", "images/enemies/Mosquibler/turn.png", success);

   // Load hit texture
   load_image(109, 89, 4, 1.0f / 20.0f, "hit", "images/enemies/Mosquibler/hit.png", success);

   // Load fall texture
   load_image(109, 89, 6, 1.0f / 20.0f, "fall", "images/enemies/Mosquibler/fall.png", success);

   // Return success
   return success;
}

// Move function
void Mosquibler::move() {
   // Check to see what direction the enemy should be facing
   if (enemy_state_ != DEATH && enemy_state_ != HURT && enemy_state_ != FALL) {
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

   // In state hurt
   if (enemy_state_ == HURT) {
      if (textures["hit"].frame_ > 3) {
         enemy_state_ = FALL;
      }
   }

   // Set to dead
   if (in_contact_down && (enemy_state_ == HURT || enemy_state_ == FALL || enemy_state_ == DEATH)) {
      // std::cout << "Mosquibler::move() - dead\n";
      enemy_state_ = DEATH;
      start_death_ = 16;
      end_death_ = 16;
   }
}

// Animate function
void Mosquibler::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["fly"]);
   } else if (enemy_state_ == TURN) {
      Element::animate(&textures["turn"]);
   } else if (enemy_state_ == HURT) {
      Element::animate(&textures["hit"]);
   } else if (enemy_state_ == FALL) {
      Element::animate(&textures["fall"]);
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

   // Get hit texture
   if (enemy_state_ == HURT) {
      return &textures["hit"];
   }

   // Get fall texture
   if (enemy_state_ == FALL) {
      return &textures["fall"];
   }

   // Get death texture
   if (enemy_state_ == DEATH) {
      return &textures["death"];
   }
}

// Start contact function
void Mosquibler::start_contact(Element *element) {
   if (element) {
      if ((element->type() == "Player" || element->type() == "Projectile") && enemy_state_ != DEATH) {
         // std::cout << "Mosquibler::start_contact() - hit by player\n";
         set_collision(CAT_PLATFORM);
         enemy_state_ = HURT;
      }

      if (element->type() == "Platform" || element->type() == "Mosqueenbler") {
         in_contact_down = true;
         if ((enemy_state_ == HURT || enemy_state_ == FALL || enemy_state_ == DEATH)) {
            // std::cout << "Mosquibler::start_contact() - hit the ground\n";
            enemy_state_ = DEATH;
         }
      }
   }
}

void Mosquibler::end_contact(Element *element) {
   if (element) {
      if (element->type() == "Platform") {
         in_contact_down = false;
      }
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
   load_image(79, 140, 20, 1.0f / 20.0f, "idle", "images/enemies/Fruig/idle.png", success);

   // Load death
   load_image(85, 136, 20, 1.0f / 20.0f, "death", "images/enemies/Fruig/death.png", success);

   // Return success
   return success;
}

// Mpve function
void Fruig::move() {
   // Check death state first
   if (enemy_state_ == DEATH) {
      // Set enemy to dead
      //alive = false;

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
         TextureData normal(10, 9, 5);
         TextureData hit(65, 9, 9);
         proj_ = create_projectile(-30, 0, 170, 10, 10, 0.0f, 0.0f, normal, hit);
         shoot_timer_ = 0;
      }

      // Update shoot timer
      shoot_timer_++;

      // Check for death of proj
      if (proj_ && !proj_->shift && proj_->get_state() == Object::DEAD) {
         proj_->sub_tex_x(24);
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
   if ((element->type() == "Player" || element->type() == "Projectile") && alive) {
      health -= 10;
      if (health == 0) {
         enemy_state_ = DEATH;
         set_collision(CAT_PLATFORM);
         start_death_ = 14;
         end_death_ = 19;
      }
   }
}

///////////////////////////////////////////////////
/************* FLEET SENSOR **********************/
///////////////////////////////////////////////////

// Constructor
FleetSensor::FleetSensor(float height, float width, Entity *entity, CONTACT contact_type, float center_x, float center_y) :
   Sensor(height, width, entity, contact_type, center_x, center_y, 1.0f) {}

// Contact functions
void FleetSensor::start_contact(Element *element) {
   if (element) {
      if (element->type() == "Platform") {
         entity_->in_contact = true;
      } else if (element->type() == "Fleet") {
         entity_->body->ApplyLinearImpulseToCenter(b2Vec2(0.5f, 0.0f), true);
         entity_->in_contact = true;
      }
   }
}
void FleetSensor::end_contact(Element *element) {
   if (element) {
      if (element->type() == "Platform") {
         entity_->in_contact = false;
      } else if (element->type() == "Fleet") {
         entity_->in_contact = false;
      }
   }
}

///////////////////////////////////////////////////
/************* FLEET ENEMY ***********************/
///////////////////////////////////////////////////

// Constructor
Fleet::Fleet(int x, int y, Application *application) :
   Enemy(x, y, 25, 49, application) {
   
   // Set shape
   element_shape.center = {0.0f, -0.08f};
   element_shape.dynamic = true;

   // Set the hitbox (28 x 12)
   set_hitbox(x, y, SQUARE, 1);

   // Create new sensor
   fleet_sensor_ = new FleetSensor(0.07f, 0.23f, this, CONTACT_DOWN, 0.0f, -0.65f);

   // Add a filter
   // b2Fixture *fixture_list = body->GetFixtureList()->GetNext();
   // if (fixture_list != nullptr) {
   //    b2Filter filter;
   //    filter.categoryBits = CAT_ENEMY;
   //    filter.maskBits = CAT_ENEMY | CAT_PLAYER | CAT_PROJECTILE | CAT_PLATFORM;
   //    fixture_list->SetFilterData(filter);
   // }

   // Set health
   health = 20;

   // Set initial dir
   entity_direction = RIGHT;

   // Set enemy state
   enemy_state_ = IDLE;

   // Start with textures flipped
   textures["turn"].flip_ = SDL_FLIP_HORIZONTAL;
   textures["idle"].flip_ = SDL_FLIP_HORIZONTAL;
}

// Load media function
bool Fleet::load_media() {
   bool success = true;

   // Load idle
   load_image(63, 87, 11, 1.0f / 20.0f, "idle", "images/enemies/Fleet/idle.png", success);

   // Load turn
   load_image(63, 87, 11, 1.0f / 20.0f, "turn", "images/enemies/Fleet/turn.png", success);

   // Load death
   load_image(63, 87, 18, 1.0f / 20.0f, "death", "images/enemies/Fleet/death.png", success);

   // Return success
   return success;
}

// Move function
void Fleet::move() {
   if (alive) {
      if (enemy_state_ == DEATH) {
         if (in_contact) {
            //alive = false;
         }
      }

      if (in_contact) {
         if (get_application()->get_player()->get_x() <= get_x() && entity_direction == RIGHT) {
            entity_direction = LEFT;
            enemy_state_ = TURN;
         } else if (get_application()->get_player()->get_x() > get_x() && entity_direction == LEFT) {
            entity_direction = RIGHT;
            enemy_state_ = TURN;
         }
      }

      // Check for turn state
      if (enemy_state_ == TURN) {
         // Complete texture
         if (textures["turn"].frame_ > 10) {
            if (entity_direction == RIGHT) {
               textures["turn"].flip_ = SDL_FLIP_HORIZONTAL;
               textures["idle"].flip_ = SDL_FLIP_HORIZONTAL;
            } else if (entity_direction == LEFT) {
               textures["turn"].flip_ = SDL_FLIP_NONE;
               textures["idle"].flip_ = SDL_FLIP_NONE;
            }
            textures["turn"].completed_ = false;
            textures["turn"].frame_ = 0;
            enemy_state_ = IDLE;
         }
      }

      // Idle is him just hopping around towards the player
      if (enemy_state_ == IDLE) {
         // Magnitude of impulse
         float magnitude = sqrt(pow((get_application()->get_player()->body->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                                 + pow((get_application()->get_player()->body->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

         // Get a vector towards the player and apply as impulse
         if (in_contact) {
            if (textures["idle"].frame_ == 1) {
               const b2Vec2 impulse = {(get_application()->get_player()->body->GetPosition().x - body->GetPosition().x) / magnitude * 1.50f, 7.0f};
               body->SetLinearVelocity(impulse);
            }

            // Reset textures
            textures["idle"].reset_frame = 0;
         } else {
            textures["idle"].reset_frame = 10;
         }
      }
   }
}

// Animate function
void Fleet::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"], textures["idle"].reset_frame, textures["idle"].stop_frame);
   } else if (enemy_state_ == TURN) {
      Element::animate(&textures["turn"]);
   } else if (enemy_state_ == DEATH) {
      Element::animate(&textures["death"], textures["death"].max_frame_);
   }
}

// Start contact function
void Fleet::start_contact(Element *element) {
   if (element && (element->type() == "Player" || element->type() == "Projectile")) {
      health -= 10;
      if (health <= 0) {
         enemy_state_ = DEATH;
         b2Fixture *fixture_list = body->GetFixtureList();
         while (fixture_list) {
            set_collision(CAT_PLATFORM, fixture_list);
            fixture_list = fixture_list->GetNext();
         }
      }
   }
}

////////////////////////////////////////////////
/*********** MOSQUEENBLER ENEMY ***************/
////////////////////////////////////////////////

Mosqueenbler::Mosqueenbler(int x, int y, Application *application) :
   Enemy(x, y, 134, 246, application), spawn_num_of_egg_(1) {

   // Set element shape stuff
   element_shape.dynamic = true;
   element_shape.shape_type.square.height = 40;
   element_shape.shape_type.square.width = 164;
   element_shape.center = {0.0f, 0.50f};
   element_shape.density = 10000.0f;

   // Set hitbox
   set_hitbox(x, y);

   // Reset filter
   b2Filter filter;
   filter.categoryBits = CAT_PLATFORM;
   filter.maskBits = CAT_ENEMY | CAT_PLAYER | CAT_PROJECTILE;
   body->GetFixtureList()->SetFilterData(filter);

   // Set entity direction
   entity_direction = RIGHT;

   // Set state
   enemy_state_ = IDLE;

   // set anchors
   anchor_x = x;
   anchor_y = y;

   // Start the timer
   movement_timer_.start();

   // Set shoot timer
   shoot_timer_ = 0;
}

// Load the media
bool Mosqueenbler::load_media() {
   bool success = true;

   // Load idle texture
   load_image(246, 134, 12, 1.0f / 20.0f, "idle", "images/enemies/Mosqueenbler/idle.png", success);

   // Load attack texture
   load_image(246, 134, 12, 1.0f / 20.0f, "attack", "images/enemies/Mosqueenbler/attack.png", success);

   // Return if success
   return success;
}

// Move function
void Mosqueenbler::move() {
   // Allow it to float
   float y = 1 * cos(movement_timer_.getTicks() / 1000.0f) + 0.197f;
   body->SetLinearVelocity({0.0f, y});

   // Spawn enemies
   if (get_application()->get_level_flag() == Application::FORESTBOSS) {
      if (shoot_timer_ > 200) {
         enemy_state_ = ATTACK;
         if (textures["attack"].frame_ == 8 && spawn_num_of_egg_ == 1) {
            MosquiblerEgg *egg = new MosquiblerEgg(get_x() + 95, get_y() + 134, get_application());
            get_application()->get_level()->add_enemy(egg);
            spawn_num_of_egg_ = 0;
         } else if (textures["attack"].completed_) {
            shoot_timer_ = 0;
            spawn_num_of_egg_ = 1;
         }
      } else {
         enemy_state_ = IDLE;
         textures["attack"].frame_ = 0;
      }

      ++shoot_timer_;
   }
}

// Animate function
void Mosqueenbler::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == ATTACK) {
      Element::animate(&textures["attack"]);
   }
}

////////////////////////////////////////////////
/*********** MOSQUIBLER EGG ********************/
////////////////////////////////////////////////

// Constructor
MosquiblerEgg::MosquiblerEgg(int x, int y, Application *application) :
   Enemy(x, y, 42, 28, application) {

   // Set hitbox
   element_shape.dynamic = true;
   set_hitbox(x, y);

   // Set enemy state
   enemy_state_ = IDLE;
}

// Load egg media
bool MosquiblerEgg::load_media() {
   bool success = true;

   // Load idle (falling)
   load_image(92, 59, 6, 1.0 / 20.0f, "idle", "images/enemies/Mosqueenbler/egg_idle.png", success);

   // Load break
   load_image(92, 59, 7, 1.0 / 20.0f, "attack", "images/enemies/Mosqueenbler/egg_break.png", success);

   return success;
}

// MosquiblerEgg move function
void MosquiblerEgg::move() {
   if (enemy_state_ == ATTACK) {
      if (textures["attack"].completed_) {
         get_application()->get_level()->add_enemy(new Mosquibler(get_x(), get_y(), get_application()));
         get_application()->get_level()->destroy_enemy(this);
      }
   }
}

// Animate
void MosquiblerEgg::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == ATTACK) {
      Element::animate(&textures["attack"]);
   }
}

// Start contact for egg
void MosquiblerEgg::start_contact(Element *element) {
   if (element->type() == "Platform") {
      enemy_state_ = ATTACK;
      set_collision(CAT_PLATFORM);
   }
}

////////////////////////////////////////////////
/*********** WORMORED BOSS ********************/
////////////////////////////////////////////////

// Sensor constructor
WormoredSensor::WormoredSensor(float height, float width, Entity *entity, CONTACT contact_type, float center_x, float center_y) :
   Sensor(height, width, entity, contact_type, center_x, center_y, 1.0f) {}

// start contact
void WormoredSensor::start_contact(Element *element) {
   if (element) {
      if (element->type() == "Player" || element->type() == "Projectile") {
         //Wormored *wormored = dynamic_cast<Wormored*>(entity_);
         //get_application()->get_player()->take_damage(10);
      }
   }
}

// Constructor for Wormored
Wormored::Wormored(int x, int y, Application *application) :
   Enemy(x, y, 418, 796, application) {

   // Set the body type to dynamic
   element_shape.dynamic = true;
   element_shape.shape_type.square.width = 640;
   element_shape.shape_type.square.height = 404;
   element_shape.center = {-0.05f, -0.06f};
   set_hitbox(x, y);
   
   // Set Wormored's collision logic
   b2Filter filter;
   filter.categoryBits = CAT_ENEMY;
   filter.maskBits = CAT_PLATFORM;
   body->GetFixtureList()->SetFilterData(filter);

   // Add new sensors to the body
   sensors_[0] = new WormoredSensor(1.97f, 0.71f, this, CONTACT_UP, -2.34f, -0.06f);
   //sensors_[1] = new WormoredSensor(395, 124, this, CONTACT_UP, x - 109, y + 13);
   //sensors_[2] = new WormoredSensor(363, 80, this, CONTACT_UP, x - 7, y + 28);
   //sensors_[3] = new WormoredSensor(320, 86, this, CONTACT_UP, x + 77, y + 50);
   //sensors_[4] = new WormoredSensor(230, 60, this, CONTACT_UP, x + 150, y + 95);
   //sensors_[5] = new WormoredSensor(179, 44, this, CONTACT_UP, x + 202, y + 120);

   // Set state to idle
   enemy_state_ = IDLE;
   entity_direction = LEFT;
}

bool Wormored::load_media() {
   bool success = true;

   // Load idle
   load_image(796, 418, 21, 1.0f / 24.0f, "idle", "images/enemies/Wormored/idle.png", success, 2);
   textures["idle"].center_ = {0, 0};

   // Load turn
   load_image(796, 418, 29, 1.0f / 24.0f, "turn", "images/enemies/Wormored/turn.png", success, 2);
   textures["turn"].center_ = {0, 0};
   
   // Load attack
   load_image(796, 418, 22, 1.0f / 24.0f, "attack", "images/enemies/Wormored/attack.png", success, 2);

   // Load excrete
   load_image(796, 418, 28, 1.0f / 24.0f, "excrete", "images/enemies/Wormored/excrete.png", success, 2);

   return success;
}

void Wormored::move() {
   if (enemy_state_ != DEATH && enemy_state_ != ATTACK && enemy_state_ != EXCRETE) {
      if (get_application()->get_player()->get_x() < (get_x() - get_width() / 2) && entity_direction == RIGHT) {
         entity_direction = LEFT;
         enemy_state_ = TURN;
      } else if (get_application()->get_player()->get_x() > (get_x() + get_width() / 2) && entity_direction == LEFT) {
         entity_direction = RIGHT;
         enemy_state_ = TURN;
      }
   }

   if (enemy_state_ == IDLE) {
      if (entity_direction == LEFT) {
         b2Vec2 vel = {-1.0f, 0.0f};
         body->SetLinearVelocity(vel);
      } else if (entity_direction == RIGHT) {
         b2Vec2 vel = {1.0f, 0.0f};
         body->SetLinearVelocity(vel);
      }
   }

   if (enemy_state_ == TURN) {
      if (textures["turn"].frame_ > textures["turn"].max_frame_ - 1) {
         if (entity_direction == RIGHT) {
            textures["idle"].flip_ = SDL_FLIP_HORIZONTAL;
            textures["turn"].flip_ = SDL_FLIP_HORIZONTAL;
         } else if (entity_direction == LEFT) {
            textures["idle"].flip_ = SDL_FLIP_NONE;
            textures["turn"].flip_ = SDL_FLIP_NONE;
         }
         textures["turn"].completed_ = false;
         textures["turn"].frame_ = 0;
         enemy_state_ = IDLE;
      }
   }
}

void Wormored::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == TURN) {
      Element::animate(&textures["turn"]);
   }
}

Texture *Wormored::get_texture() {
   if (enemy_state_ == IDLE) {
      return &textures["idle"];
   }

   if (enemy_state_ == TURN) {
      return &textures["turn"];
   }

   if (enemy_state_ == ATTACK) {
      return &textures["attack"];
   }

   if (enemy_state_ == EXCRETE) {
      return &textures["excrete"];
   }
}

Wormored::~Wormored() {
   for (int i = 0; i < 5; i++) {
      delete sensors_[i];
   }
}