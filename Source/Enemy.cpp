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

Enemy::Enemy(int x, int y, Application *application) :
   Entity(x, y, 92, 82, application), enemy_state_(IDLE),
   idle_texture(this, 17), shoot_texture(this, 6), poojectile_texture(this, 7),
   death_texture(this, 15), shoot_timer_(0) {

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

   // Set fps for textures
   idle_texture.fps = 1.0f / 20.0f;
   shoot_texture.fps = 1.0f / 20.0f;
   poojectile_texture.fps = 1.0 / 20.0f;
   death_texture.fps = 1.0 / 20.0f;
}

void Enemy::update(bool freeze) {
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
   /*
   last_frame += 
      (fps_timer.getDeltaTime() / 1000.0f);
   if (last_frame > get_application()->animation_update_time_) {
      animate();
      last_frame = 0.0f;
   }
   */
   animate();
}

void Enemy::animate(Texture *tex, int reset, int max) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      /*
      if (idle_texture.frame_ > 17) {
         idle_texture.frame_ = 0;
      }
      idle_texture.curr_clip_ = &idle_texture.clips_[idle_texture.frame_];
      ++idle_texture.frame_;
      */
      Element::animate(&idle_texture);
   } else if (enemy_state_ == SHOOT) {
      /*
      if (shoot_texture.frame_ > 6) {
         shoot_texture.frame_ = 0;
      }
      shoot_texture.curr_clip_ = &shoot_texture.clips_[shoot_texture.frame_];
      ++shoot_texture.frame_;
      */
      Element::animate(&shoot_texture);
   } else if (enemy_state_ == DEATH) {
      /*
      if (death_texture.frame_ > 15) {
         death_texture.frame_ = 15;
      }
      death_texture.curr_clip_ = &death_texture.clips_[death_texture.frame_];
      ++death_texture.frame_;
      */
      Element::animate(&death_texture, 15);
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
