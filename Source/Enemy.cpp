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

   // Enemy sprite sheet
   textures.emplace("idle", Texture(this, 17, 1.0f / 20.0f));
   if (!textures["idle"].loadFromFile("images/enemies/Fecreez/fecreez_idle.png")) {
      printf("Failed to load feecreez idle texture!\n");
      success = false;
   } else {
      // Allocate enough room
      textures["idle"].clips_ = new SDL_Rect[18];
      SDL_Rect *temp = textures["idle"].clips_;

      // Calculate sprite locations
      for (int i = 0; i < 18; i++) {
         temp[i].x = i * 82;
         temp[i].y = 0;
         temp[i].w = 82;
         temp[i].h = 92;
      }

      // Set curr clip
      textures["idle"].curr_clip_ = &temp[0];
   }

   textures.emplace("shoot", Texture(this, 6, 1.0f / 20.0f));
   if (!textures["shoot"].loadFromFile("images/enemies/Fecreez/fecreez_shoot.png")) {
      printf("Failed to load feecreez shoot texture!\n");
      success = false;
   } else {
      // Allocate enough room
      textures["shoot"].clips_ = new SDL_Rect[7];
      SDL_Rect *temp = textures["shoot"].clips_;

      // Calculate sprite locations
      for (int i = 0; i < 7; i++) {
         temp[i].x = i * 82;
         temp[i].y = 0;
         temp[i].w = 82;
         temp[i].h = 92;
      }

      // Set curr clip
      textures["shoot"].curr_clip_ = &temp[0];
   }

   textures.emplace("poojectile", Texture(this, 7, 1.0f / 20.0f));
   if (!textures["poojectile"].loadFromFile("images/enemies/Fecreez/poojectile.png")) {
      printf("Failed to load poojectile texture!\n");
      success = false;
   } else {
      // Allocate enough room
      textures["poojectile"].clips_ = new SDL_Rect[8];
      SDL_Rect *temp = textures["poojectile"].clips_;

      // Calculate sprite locations
      for (int i = 0; i < 8; i++) {
         temp[i].x = i * 24;
         temp[i].y = 0;
         temp[i].w = 24;
         temp[i].h = 15;
      }

      // Set curr clip
      textures["poojectile"].curr_clip_ = &temp[0];
   }

   textures.emplace("death", Texture(this, 15, 1.0f / 20.0f));
   if (!textures["death"].loadFromFile("images/enemies/Fecreez/fecreez_death.png")) {
      printf("Failed to load fecreez death texture!\n");
      success = false;
   } else {
      // Allocate enough room
      textures["death"].clips_ = new SDL_Rect[16];
      SDL_Rect *temp = textures["death"].clips_;

      // Calculate sprite locations
      for (int i = 0; i < 16; i++) {
         temp[i].x = i * 143;
         temp[i].y = 0;
         temp[i].w = 143;
         temp[i].h = 92;
      }

      // Set curr clip
      textures["death"].curr_clip_ = &temp[0];
   }

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
   arms_attack(x + 30, y - 230, 387, 168, application), hurt_counter_(0),
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
   textures.emplace("idle", Texture(this, 14, 1.0f / 20.0f));
   if (!textures["idle"].loadFromFile("images/enemies/Rosea/rosea_idle.png")) {
      printf("Failed to load rosea_idle.png\n");
      success = false;
   } else {
      // Allocate room
      textures["idle"].clips_ = new SDL_Rect[15];
      SDL_Rect *temp = textures["idle"].clips_;

      // Set sprites
      for (int i = 0; i < 15; i++) {
         temp[i].x = i * 189;
         temp[i].y = 0;
         temp[i].w = 189;
         temp[i].h = 144;
      }
      
      // Set first clip
      textures["idle"].curr_clip_ = &temp[0];
   }

   // Load hurt texture
   textures.emplace("hurt", Texture(this, 14, 1.0f / 20.0f));
   if (!textures["hurt"].loadFromFile("images/enemies/Rosea/rosea_hurt.png")) {
      printf("Failed to load rosea_hurt.png\n");
      success = false;
   } else {
      // Allocate room
      textures["hurt"].clips_ = new SDL_Rect[15];
      SDL_Rect *temp = textures["hurt"].clips_;

      // Set sprites
      for (int i = 0; i < 15; i++) {
         temp[i].x = i * 189;
         temp[i].y = 0;
         temp[i].w = 189;
         temp[i].h = 144;
      }
      
      // Set first clip
      textures["hurt"].curr_clip_ = &temp[0];
   }

   // Load arms_idle texture
   arms_still.textures.emplace("arms_idle", Texture(this, 14, 1.0f / 20.0f));
   if (!arms_still.textures["arms_idle"].loadFromFile("images/enemies/Rosea/arms_idle.png")) {
      printf("Failed to load arms_idle.png\n");
      success = false;
   } else {
      // Allocate room
      arms_still.textures["arms_idle"].clips_ = new SDL_Rect[15];
      SDL_Rect *temp = arms_still.textures["arms_idle"].clips_;

      // Set sprites
      for (int i = 0; i < 15; i++) {
         temp[i].x = i * 112;
         temp[i].y = 0;
         temp[i].w = 112;
         temp[i].h = 78;
      }
      
      // Set first clip
      arms_still.textures["arms_idle"].curr_clip_ = &temp[0];
   }

   // Load arms_hurt texture
   arms_still.textures.emplace("arms_hurt", Texture(this, 14, 1.0f / 20.0f));
   if (!arms_still.textures["arms_hurt"].loadFromFile("images/enemies/Rosea/arms_hurt.png")) {
      printf("Failed to load arms_hurt.png\n");
      success = false;
   } else {
      // Allocate room
      arms_still.textures["arms_hurt"].clips_ = new SDL_Rect[15];
      SDL_Rect *temp = arms_still.textures["arms_hurt"].clips_;

      // Set sprites
      for (int i = 0; i < 15; i++) {
         temp[i].x = i * 112;
         temp[i].y = 0;
         temp[i].w = 112;
         temp[i].h = 78;
      }
      
      // Set first clip
      arms_still.textures["arms_hurt"].curr_clip_ = &temp[0];
   }

   // Load arms_hurt texture
   arms_attack.textures.emplace("attack", Texture(this, 14, 1.0f / 20.0f));
   if (!arms_attack.textures["attack"].loadFromFile("images/enemies/Rosea/arms_attack.png")) {
      printf("Failed to load arms_attack.png\n");
      success = false;
   } else {
      // Allocate room
      arms_attack.textures["attack"].clips_ = new SDL_Rect[15];
      SDL_Rect *temp = arms_attack.textures["attack"].clips_;

      // Set sprites
      for (int i = 0; i < 15; i++) {
         temp[i].x = i * 122;
         temp[i].y = 0;
         temp[i].w = 122;
         temp[i].h = 387;
      }
      
      // Set first clip
      arms_attack.textures["attack"].curr_clip_ = &temp[0];
   }

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
   } else if (enemy_state_ == ATTACK) {
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

   // Now, check to see if player is within bounds
   if (get_application()->get_player()->get_x() >= get_x() - 200 &&
         get_application()->get_player()->get_x() <= get_x() + 200) {
      // Set state to attack
      enemy_state_ = ATTACK;

      // Deactivate main body
      body->SetActive(false);

      // TODO: Need to find a sinusoidal function that describes the motion of the arm
      // and change the y position of the hitbox based on that.
      //std::cout << "frame: " << arms_attack.textures["attack"].frame_ << " " <<
         //arm_heights_[arms_attack.textures["attack"].frame_] << std::endl;
      arms_attack.set_y(arm_heights_[arms_attack.textures["attack"].frame_]);
   } else {
      if (enemy_state_ != HURT && arms_attack.textures["attack"].completed_) {
         enemy_state_ = IDLE;
         arms_attack.textures["attack"].completed_ = true;
         arms_attack.set_y(arm_heights_[15]);
      }
   }
}

// Rosea animate
void Rosea::animate(Texture *tex, int reset, int max) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
      Element::animate(&arms_still.textures["arms_idle"]);
   } else if (enemy_state_ == ATTACK) {
      Element::animate(&textures["idle"]);
      Element::animate(&arms_attack.textures["attack"]);
   } else if (enemy_state_ == HURT) {
      Element::animate(&textures["hurt"]);
      Element::animate(&arms_still.textures["arms_hurt"]);
   }
}

// Rosea get texture
Texture* Rosea::get_texture() {
   // Get idle texture
   if (enemy_state_ == IDLE || enemy_state_ == ATTACK) {
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

// Rosea destructor
Rosea::~Rosea() {
   textures["idle"].free();
   textures["hurt"].free();
   arms_still.textures["arms_idle"].free();
   arms_still.textures["arms_hurt"].free();
   arms_attack.textures["arms_attack"].free();
}
