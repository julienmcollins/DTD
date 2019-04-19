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
   Entity(x, y, height, width, application), enemy_state_(IDLE), shoot_timer_(0) {

   // Setup Box2D
   // Set body type
   body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x_temp = float(x) * application->to_meters_;
   float y_temp = -float(y) * application->to_meters_;
   body_def.position.Set(x_temp, y_temp);
   body_def.fixedRotation = true;

   // Attach body to world
   body = get_application()->world_.CreateBody(&body_def);

   // Set box dimensions
   float width_box = (get_width() / 2.0f) * application->to_meters_ - 0.02f;
   float height_box = (get_height() / 2.0f) * application->to_meters_ - 0.02f;
   box.SetAsBox(width_box, height_box);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 1.8f;
   body->CreateFixture(&fixture_def);

   // Set user data so it can react
   body->SetUserData(this);
}

Enemy::~Enemy() {}

/************** FECREEZ IMPLEMENTATIONS ********************/
Fecreez::Fecreez(int x, int y, int height, int width, Application *application) :
   Enemy(x, y, height, width, application) {

   // Run the load media function
   if (load_media() == false) {
      application->set_quit();
   }

   // Set fps for textures
   textures["idle"].fps = 1.0f / 20.0f;
   textures["shoot"].fps = 1.0f / 20.0f;
   textures["poojectile"].fps = 1.0 / 20.0f;
   textures["death"].fps = 1.0 / 20.0f;

   // Set health
   health = 100;
}

// Load media function for fecreez
bool Fecreez::load_media() {
   // Flag for success
   bool success = true;

   // Enemy sprite sheet
   textures.emplace("idle", Texture(this, 17));
   if (!textures["idle"].loadFromFile("images/enemies/fecreez_idle.png")) {
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

   textures.emplace("shoot", Texture(this, 6));
   if (!textures["shoot"].loadFromFile("images/enemies/fecreez_shoot.png")) {
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

   textures.emplace("poojectile", Texture(this, 7));
   if (!textures["poojectile"].loadFromFile("images/enemies/poojectile.png")) {
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

   textures.emplace("death", Texture(this, 15));
   if (!textures["death"].loadFromFile("images/enemies/fecreez_death.png")) {
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
   SDL_Rect *curr_clip = get_curr_clip();
   if (curr_clip) {
     // Render player
     render(enemytexture, curr_clip);
   }
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
         enemy_state_ = SHOOT;

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
      // For now, use IDLE animation
      // TODO: set state to death
      enemy_state_ = DEATH;
   }

   // Update frames
   animate();
}

// Animate function
void Fecreez::animate(Texture *tex, int reset, int max) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      Element::animate(&textures["idle"]);
   } else if (enemy_state_ == SHOOT) {
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
   if (enemy_state_ == SHOOT) {
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
