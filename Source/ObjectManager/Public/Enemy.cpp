#include "Source/ObjectManager/Private/Enemy.h"
#include "Source/ObjectManager/Private/Entity.h"
#include "Source/ObjectManager/Private/Object.h"
#include "Source/ObjectManager/Private/Global.h"

#include "Source/GameEngine/Private/Application.h"
#include "Source/GameEngine/Private/Level.h"

#include "Source/RenderingEngine/Private/RenderingEngine.h"
#include "Source/RenderingEngine/Private/Texture.h"

#include "Source/MathStructures/Private/Coordinates.h"

#include <stdio.h>
#include <cmath>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <Box2D/Box2D.h>

const std::string Enemy::media_path = Application::sprite_path + "/Enemies/";

/********************* ENEMY IMPLEMENTATIONS ******************/

Enemy::Enemy(int x, int y, int height, int width) :
   Entity(x, y, height, width), enemy_state_(IDLE), shoot_timer_(101) {

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
//       such as to properly load each texture (one enemy has different sizes for GetAnimationByName(
// TODO: Do it based on get_x() instead of get_tex_x() (will need to change consts)
Projectile* Enemy::CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y, 
      bool owner, bool damage, float force_x, float force_y) {
   
   // First, create a new projectile
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new Projectile(name, get_tex_x() + get_width() + delta_x_r, get_tex_y() + delta_y, 
            width, height, 0, damage, force_x, force_y, this);
   } else {
      proj = new Projectile(name, get_tex_x() + delta_x_l, get_tex_y() + delta_y,
            width, height, 0, damage, force_x, force_y, this);
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
   Animation *anim = GetAnimationFromState();

   // Render player
   if (anim) {
      std::cout << get_x() << " " << get_y() << std::endl;
      sprite_sheet->Render(get_x(), get_y(), 0.0f, anim);
   }
}

Animation *Enemy::GetAnimationFromState() {
   // Get idle texture
   if (enemy_state_ == IDLE) {
      return GetAnimationByName("idle");
   }
   
   // Get attack texture
   if (enemy_state_ == ATTACK) {
      return GetAnimationByName("attack");
   }

   // Get death texture
   if (enemy_state_ == DEATH) {
      return GetAnimationByName("death");
   }

   // Get turn texture
   if (enemy_state_ == TURN) {
      return GetAnimationByName("turn");
   }
}

bool Enemy::within_bounds() {
   return Application::GetInstance().get_player()->get_y() >= get_y() - get_height() &&
            Application::GetInstance().get_player()->get_y() <= get_y() + get_height();
}

Enemy::~Enemy() {}

/************** FECREEZ IMPLEMENTATIONS ********************/
Fecreez::Fecreez(int x, int y) :
   Enemy(x, y, 92, 82) {

   // Set the hitboxes
   SetHitbox(x, y);

   // Set health
   health = 30;

   // Set entity direction to right
   entity_direction = LEFT;
}

// Load media function for fecreez
bool Fecreez::LoadMedia() {
   // Flag for success
   bool success = true;

   // Instantiate animations
   std::string fecreez_path = Application::GetInstance().sprite_path + "Enemies/Fecreez/fecreez_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("fecreez_master_sheet", fecreez_path.c_str());
   animations.emplace("turn", new Animation(sprite_sheet, "turn", 82.0f, 92.0f, 0.0f, 7, 1.0f / 20.0f));
   animations.emplace("attack", new Animation(sprite_sheet, "attack", 82.0f, 92.0f, 92.0f, 9, 1.0f / 20.0f));
   animations.emplace("idle", new Animation(sprite_sheet, "idle", 82.0f, 92.0f, 184.0f, 18, 1.0f / 20.0f));
   animations.emplace("death", new Animation(sprite_sheet, "death", 143.0f, 92.0f, 276.0f, 16, 1.0f / 20.0f));
   RenderingEngine::GetInstance().LoadResources(this);

   // Start flipped
   flipAllAnimations();

   // Return success
   return success;
}

void Fecreez::move() {
   // Overall check to see if it's alive
   if (enemy_state_ == DEATH) {
      // TODO: find better solution
      if (!shift_) {
         sub_tex_x(-70);
         shift_ = true;
      }
   }

   // Check to see what direction the enemy should be facing
   if (enemy_state_ != DEATH && within_bounds()) {
      // Turn if direction changed
      if (Application::GetInstance().get_player()->get_x() <= get_x() 
         && entity_direction == RIGHT) {
         enemy_state_ = TURN;
         entity_direction = LEFT;
      } else if (Application::GetInstance().get_player()->get_x() > get_x() 
         && entity_direction == LEFT) {
         enemy_state_ = TURN;
         entity_direction = RIGHT;
      }
   }

   // Turn fecreez
   if (enemy_state_ == TURN) {
      if (GetAnimationByName("turn")->completed) {
         if (entity_direction == RIGHT) {
            if (textureFlipped()) {
               flipAllAnimations();
               texture_flipped = false;
            }
         } else if (entity_direction == LEFT) {
            if (!textureFlipped()) {
               flipAllAnimations();
               texture_flipped = true;
            }
         }
         GetAnimationByName("turn")->completed = false;
         enemy_state_ = IDLE;
      }
   }

   // Check to see if get_player() within bounds of enemy
   if (Application::GetInstance().get_player()->get_y() >= get_y() - get_height() &&
      Application::GetInstance().get_player()->get_y() <= get_y() + get_height()
      && enemy_state_ != TURN && enemy_state_ != DEATH) {
      if (shoot_timer_ >= 100) {
         enemy_state_ = ATTACK;
      } else if (shoot_timer_ < 100 && GetAnimationByName("attack")->completed) {
         enemy_state_ = IDLE;
         GetAnimationByName("attack")->completed = false;
      }
      ++shoot_timer_;
   } else if (enemy_state_ != TURN && enemy_state_ != DEATH && enemy_state_ != ATTACK) {
      enemy_state_ = IDLE;
   }

   // attack
   if (enemy_state_ == ATTACK) {
      if (GetAnimationByName("attack")->curr_frame > 4 && shoot_timer_ >= 100) {
         Projectile *tmp = CreateProjectile("fecreez_projectile", 24.0f, 15.0f, 15, -10, 70, 0, 10, 10.0f, 0.0f);
         tmp->body->SetGravityScale(0);
         shoot_timer_ = 0;
      }
      if (GetAnimationByName("attack")->completed) {
         enemy_state_ = IDLE;
         GetAnimationByName("attack")->completed = false;
      }
   }

   // Update frames
   animate();
}

// Start contact function
void Fecreez::StartContact(Element *element) {
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
      sprite_sheet->Animate(GetAnimationByName("idle"));
   } else if (enemy_state_ == ATTACK) {
      sprite_sheet->Animate(GetAnimationByName("attack"), GetAnimationByName("attack")->reset_frame, GetAnimationByName("attack")->stop_frame);
   } else if (enemy_state_ == DEATH) {
      sprite_sheet->Animate(GetAnimationByName("death"), 15);
   } else if (enemy_state_ == TURN) {
      sprite_sheet->Animate(GetAnimationByName("turn"));
   }
}

Fecreez::~Fecreez() {
}

//////////////////////////////////////////////////////
/********************* ARM **************************/
//////////////////////////////////////////////////////

Arm::Arm(int x, int y, int height, int width, Rosea *rosea) :
   Enemy(x, y, height, width), rosea_(rosea) {}

Animation *Arm::GetAnimationFromState() {
   if (enemy_state_ == IDLE) {
      return GetAnimationByName("arms_idle");
   }

   if (enemy_state_ == HURT) {
      return GetAnimationByName("arms_hurt");
   }

   if (enemy_state_ == ATTACK) {
      return GetAnimationByName("attack");
   }
}

// Callback function for arm will set rosea's state to hurt
void Arm::StartContact(Element *element) {
   if (element->type() == "Player") {
      Application::GetInstance().get_player()->TakeDamage(10);
   } else if (element->type() == "Projectile") {
      if (rosea_->get_state() != ATTACK) {
         rosea_->set_state(HURT);
      }
   }
}

//////////////////////////////////////////////////////
/****************** ROSEA ENEMY *********************/
//////////////////////////////////////////////////////

Rosea::Rosea(int x, int y, float angle) :
   Enemy(x, y, 144, 189), 
   arms_still(x - 46, y - 118, 78, 122, this),
   arms_attack(x + 5, y - 230, 387, 122, this), 
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

   // Set enemy state
   enemy_state_ = IDLE;

   // Special state for 0 angle
   // if (angle == 0.0f) {
      // Set hitbox for rosea body
      SetHitbox(x, y);

      arms_attack.SetHitbox(arms_attack.get_tex_x(), arms_attack.get_tex_y());
      arms_still.SetHitbox(arms_still.get_tex_x() + 61, arms_still.get_tex_y() + 42);

      // TODO: get separate elements for the arms (ie new elements and set is as dynamic body)
      // Set texture location
      // arms_attack.GetAnimationByName("attack")->set_x(arms_attack.get_tex_x());
      // arms_attack.GetAnimationByName("attack")->set_y(arms_attack.get_tex_y());
      arms_attack.static_positions["attack"].x = arms_attack.get_tex_x() + 5;
      arms_attack.static_positions["attack"].y = arms_attack.get_tex_y();

      // Set initial arm postion
      arms_attack.set_tex_y(arm_heights_[0]);
   // }
   
   // Set arm model to be normal you know?
   arm_model = glm::mat4(1.0f);

   // Need to change arm position if rotated
   if (angle == 90.0f) {
      // Construct a matrix that will essentially rotate with the entire object
      arm_model = glm::translate(arm_model, glm::vec3(get_anim_x() - (arms_still.get_anim_x() + arms_still.GetAnimationFromState()->half_width), get_anim_y() - (arms_still.get_anim_y() + arms_still.GetAnimationFromState()->half_height), 0.0f));

      // // Set arms still height, width, x and y
      // arms_still.set_width(78);
      // arms_still.set_height(112);
      // arms_still.set_y(y + 50);
      // arms_still.set_x(x + 47);

      // // Set height and width
      // arms_attack.set_width(387);
      // arms_attack.set_height(122);
      // arms_attack.set_tex_y(y + 37);
      // arms_attack.set_tex_x(x + 310);

      // // arms_attack.GetAnimationByName("attack")->set_x(arms_attack.get_x());
      // // arms_attack.GetAnimationByName("attack")->set_y(arms_attack.get_y());
      // arms_attack.static_positions["attack"].x = arms_attack.get_x();
      // arms_attack.static_positions["attack"].y = arms_attack.get_y();

      // // arms_still.GetAnimationByName("arms_idle")->set_x(arms_still.get_x());
      // // arms_still.GetAnimationByName("arms_idle")->set_y(arms_still.get_y());
      // arms_still.static_positions["arms_idle"].x = arms_still.get_x();
      // arms_still.static_positions["arms_idle"].y = arms_still.get_y();

      // // arms_still.GetAnimationByName("arms_hurt")->set_x(arms_still.get_x());
      // // arms_still.GetAnimationByName("arms_hurt")->set_y(arms_still.get_y());
      // arms_still.static_positions["arms_hurt"].x = arms_still.get_x();
      // arms_still.static_positions["arms_hurt"].y = arms_still.get_y();

      // arms_attack.SetHitbox(arms_attack.get_tex_x(), arms_attack.get_tex_y());
      // arms_still.SetHitbox(arms_still.get_tex_x() - 70, arms_still.get_tex_y() + 60);

      // arms_attack.set_tex_y(y + 40);
      // arms_attack.set_tex_x(arm_widths_[0]);
      
      // // Set texture angle
      // // GetAnimationByName("idle")->angle = angle;
      // // GetAnimationByName("hurt")->angle = angle;
      // // arms_still.GetAnimationByName("arms_idle")->angle = angle;
      // // arms_still.GetAnimationByName("arms_hurt")->angle = angle;
      // // arms_attack.GetAnimationByName("attack")->angle = angle;
   }

   // Set health
   health = 100;
}

// Load media for rosea
bool Rosea::LoadMedia() {
   // Flag for success
   bool success = true;

   // Load rosea main body
   std::string rosea_path = media_path + "Rosea/rosea_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("rosea_master_sheet", rosea_path.c_str());
   animations.emplace("idle", new Animation(sprite_sheet, "idle", 189.0, 144.0, 0.0, 15, 1.0 / 20.0));
   animations.emplace("hurt", new Animation(sprite_sheet, "hurt", 189.0, 144.0, 144.0, 15, 1.0 / 20.0));
   RenderingEngine::GetInstance().LoadResources(this);

   std::string arm_path = media_path + "Rosea/rosea_arm_master_sheet.png";
   arm_sheet = RenderingEngine::GetInstance().LoadTexture("rosea_arm_master_sheet", arm_path.c_str());
   arms_still.animations.emplace("arms_idle", new Animation(arm_sheet, "arms_idle", 112.0, 78.0, 0.0, 15, 1.0 / 20.0));
   arms_still.animations.emplace("arms_hurt", new Animation(arm_sheet, "arms_hurt", 112.0, 78.0, 78.0, 15, 1.0 / 20.0));
   arms_attack.animations.emplace("attack", new Animation(arm_sheet, "attack", 122.0, 387.0, 156.0, 15, 1.0 / 20.0));
   RenderingEngine::GetInstance().LoadResources(&arms_still);
   RenderingEngine::GetInstance().LoadResources(&arms_attack);

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
   Animation *enemytexture = GetAnimationFromState();

   // Render arms
   if (enemy_state_ == IDLE) {
      arms_still.set_state(IDLE);
      if (angle_ == 0.0f) {
         arm_sheet->Render(arms_still.get_anim_x(), arms_still.get_anim_y(), 0.0f, arms_still.GetAnimationByName("arms_idle"));
      } else {
         // arms_still.texture_render(arms_still.GetAnimationByName("arms_idle"));
         arm_sheet->Render(arms_still.static_positions["arms_idle"].x, arms_still.static_positions["arms_idle"].y, 0.0f, arms_still.GetAnimationByName("arms_idle"));
      }
   } else if (enemy_state_ == HURT) {
      arms_still.set_state(HURT);
      if (angle_ == 0.0f) {
         // arms_still.Render(arms_still.GetAnimationByName("arms_hurt"));
         arm_sheet->Render(arms_still.get_anim_x(), arms_still.get_anim_y(), 0.0f, arms_still.GetAnimationByName("arms_hurt"));
      } else {
         // arms_still.texture_render(arms_still.GetAnimationByName("arms_hurt"));
      }
   } else if (enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      arms_attack.set_state(ATTACK);
      // arms_attack.texture_render(arms_attack.GetAnimationByName("attack"));
      arm_sheet->Render(arms_attack.static_positions["attack"].x, arms_attack.static_positions["attack"].y, 0.0f, arms_attack.GetAnimationByName("attack"));
   }

   // Render enemy
   if (angle_ == 0.0f) {
      sprite_sheet->Render(get_anim_x(), get_anim_y(), 0.0f, enemytexture);
   } else {
      sprite_sheet->Render(anchor_x, anchor_y, angle_, enemytexture);
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
      if (hurt_counter_ >= 50 && arms_still.GetAnimationByName("arms_hurt")->completed) {
         enemy_state_ = IDLE;
         hurt_counter_ = 0;
         arms_still.GetAnimationByName("arms_hurt")->completed = false;
      }
   } else if (enemy_state_ == RETREAT) {
      if (angle_ == 0.0f) {
         arms_attack.set_tex_y(arm_heights_[arms_attack.GetAnimationByName("attack")->curr_frame]);
      } else {
         arms_attack.set_tex_x(arm_widths_[arms_attack.GetAnimationByName("attack")->curr_frame]);
      }
      if (arms_attack.GetAnimationByName("attack")->curr_frame > 14) {
         enemy_state_ = IDLE;
         arms_attack.GetAnimationByName("attack")->curr_frame = 0;
         arms_attack.GetAnimationByName("attack")->completed = false;
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
         int temp = (arms_attack.GetAnimationByName("attack")->curr_frame < arm_state_) ? 
                        arms_attack.GetAnimationByName("attack")->curr_frame : arm_state_;

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
            arm_state_ = 15;
            in_bounds_ = false;
         }
      }
   }
}

// Rosea animate
void Rosea::animate(Texture *tex, int reset, int max, int start) {
   // Animate based on different states
   if (enemy_state_ == IDLE) {
      sprite_sheet->Animate(arms_still.GetAnimationByName("arms_idle"));
      sprite_sheet->Animate(GetAnimationByName("idle"));
   } else if (enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      sprite_sheet->Animate(arms_attack.GetAnimationByName("attack"), arm_state_, arm_state_);
      sprite_sheet->Animate(GetAnimationByName("idle"));
   } else if (enemy_state_ == HURT) {
      sprite_sheet->Animate(arms_still.GetAnimationByName("arms_hurt"));
      sprite_sheet->Animate(GetAnimationByName("hurt"));
   }
}

// Rosea get texture
Animation* Rosea::GetAnimationFromState() {
   // Get idle texture
   if (enemy_state_ == IDLE || enemy_state_ == ATTACK || enemy_state_ == RETREAT) {
      return GetAnimationByName("idle");
   }
   
   // Get hurt texture for main body
   if (enemy_state_ == HURT) {
      return GetAnimationByName("hurt");
   }
   return nullptr;
}

// Get contact
void Rosea::StartContact(Element *element) {
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
      if (Application::GetInstance().get_player()->get_x() >= get_x() - 250 
         && Application::GetInstance().get_player()->get_x() <= get_x() + 250) {
         return true;
      }
   } else {
      if (Application::GetInstance().get_player()->get_y() >= (get_y() - 250) 
         && Application::GetInstance().get_player()->get_y() <= (get_y() + 250)
         && Application::GetInstance().get_player()->get_x() <= (get_x() + 400)) {
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

Mosquibler::Mosquibler(int x, int y) :
   Enemy(x, y, 89, 109) {

   // Set element shape stuff
   element_shape.dynamic = true;

   // Set hitbox
   SetHitbox(x, y, SQUARE, 2);
 
   // Set health
   health = 10;

   // Set initial dir
   entity_direction = LEFT;

   // Set enemy state
   enemy_state_ = IDLE;   
}

// Load media function
bool Mosquibler::LoadMedia() {
   // Flag for success
   bool success = true;

   // Set sprite sheet
   std::string path = media_path + "Mosquibler/mosquibler_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("mosquibler_master_sheet", path.c_str());
   animations.emplace("fly", new Animation(sprite_sheet, "fly", 111.0, 99.0, 0.0, 12, 1.0 / 20.0));
   animations.emplace("turn", new Animation(sprite_sheet, "turn", 111.0, 99.0, 99.0, 12, 1.0 / 20.0));
   animations.emplace("hit", new Animation(sprite_sheet, "hit", 111.0, 99.0, 198.0, 4, 1.0 / 20.0));
   animations.emplace("fall", new Animation(sprite_sheet, "fall", 111.0, 91.0, 297.0, 6, 1.0 / 20.0));
   animations.emplace("death", new Animation(sprite_sheet, "death", 111.0, 91.0, 388.0, 17, 1.0 / 20.0));
   RenderingEngine::GetInstance().LoadResources(this);
   
   // Return success
   return success;
}

// Move function
void Mosquibler::move() {
   // Check to see what direction the enemy should be facing
   if (enemy_state_ != DEATH && enemy_state_ != HURT && enemy_state_ != FALL) {
      if (Application::GetInstance().get_player()->get_x() <= get_x() && entity_direction == RIGHT) {
         entity_direction = LEFT;
         enemy_state_ = TURN;
      } else if (Application::GetInstance().get_player()->get_x() > get_x() && entity_direction == LEFT) {
         entity_direction = RIGHT;
         enemy_state_ = TURN;
      }
   }

   // Turn around
   if (enemy_state_ == TURN) {
      // Magnitude of impulse
      float magnitude = sqrt(pow((Application::GetInstance().get_player()->body->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                              + pow((Application::GetInstance().get_player()->body->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

      // Get a vector towards the player and apply as impulse
      const b2Vec2 impulse = {(Application::GetInstance().get_player()->body->GetPosition().x - body->GetPosition().x) / magnitude * 0.90f, 
                     (Application::GetInstance().get_player()->body->GetPosition().y - body->GetPosition().y) / magnitude * 0.90f};
      
      // Apply impulse
      body->SetLinearVelocity(impulse);

      // Complete texture
      if (GetAnimationByName("turn")->completed) {
         if (entity_direction == RIGHT) {
            if (!textureFlipped()) {
               flipAllAnimations();
               texture_flipped = true;
            }
         } else if (entity_direction == LEFT) {
            if (textureFlipped()) {
               flipAllAnimations();
               texture_flipped = false;
            }
         }
         GetAnimationByName("turn")->completed = false;
         GetAnimationByName("turn")->curr_frame = 0;
         enemy_state_ = IDLE;
      }
   }

   // Fly towards player
   if (enemy_state_ == IDLE) {
      // Magnitude of impulse
      float magnitude = sqrt(pow((Application::GetInstance().get_player()->body->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                              + pow((Application::GetInstance().get_player()->body->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

      // Get a vector towards the player and apply as impulse
      const b2Vec2 impulse = {(Application::GetInstance().get_player()->body->GetPosition().x - body->GetPosition().x) / magnitude * 0.90f, 
                     (Application::GetInstance().get_player()->body->GetPosition().y - body->GetPosition().y) / magnitude * 0.90f};
      
      // Apply impulse
      body->SetLinearVelocity(impulse);
   }

   // In state hurt
   if (enemy_state_ == HURT) {
      if (GetAnimationByName("hit")->curr_frame > 3) {
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
      sprite_sheet->Animate(GetAnimationByName("fly"));
   } else if (enemy_state_ == TURN) {
      sprite_sheet->Animate(GetAnimationByName("turn"));
   } else if (enemy_state_ == HURT) {
      sprite_sheet->Animate(GetAnimationByName("hit"));
   } else if (enemy_state_ == FALL) {
      sprite_sheet->Animate(GetAnimationByName("fall"));
   } else if (enemy_state_ == DEATH) {
      sprite_sheet->Animate(GetAnimationByName("death"), start_death_, end_death_);
   }
}

// Get texture function
Animation* Mosquibler::GetAnimationFromState() {
   // Get idle texture
   if (enemy_state_ == IDLE) {
      return GetAnimationByName("fly");
   }

   // Get turn texture
   if (enemy_state_ == TURN) {
      return GetAnimationByName("turn");
   }

   // Get hit texture
   if (enemy_state_ == HURT) {
      return GetAnimationByName("hit");
   }

   // Get fall texture
   if (enemy_state_ == FALL) {
      return GetAnimationByName("fall");
   }

   // Get death texture
   if (enemy_state_ == DEATH) {
      return GetAnimationByName("death");
   }
}

// Start contact function
void Mosquibler::StartContact(Element *element) {
   if (element) {
      if ((element->type() == "Player" || element->type() == "Projectile") && enemy_state_ != DEATH) {
         // std::cout << "Mosquibler::StartContact() - hit by player\n";
         set_collision(CAT_PLATFORM);
         enemy_state_ = HURT;
      }

      if (element->type() == "Platform" || element->type() == "Mosqueenbler") {
         in_contact_down = true;
         if ((enemy_state_ == HURT || enemy_state_ == FALL || enemy_state_ == DEATH)) {
            // std::cout << "Mosquibler::StartContact() - hit the ground\n";
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
Fruig::Fruig(int x, int y) :
   Enemy(x, y, 140, 79) {

   // Set hitbox
   SetHitbox(x, y);

   // Set health
   health = 10;

   // Set initial dir
   entity_direction = RIGHT;

   // Set enemy state
   enemy_state_ = IDLE;
}

// Load media function
bool Fruig::LoadMedia() {
   bool success = true;

   std::string path = media_path + "Fruig/fruig_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("fruig_master_sheet", path.c_str());
   animations.emplace("idle", new Animation(sprite_sheet, "idle", 79.0, 140.0, 0.0, 20, 1.0 / 20.0));
   animations.emplace("death", new Animation(sprite_sheet, "death", 85.0, 136.0, 140.0, 20, 1.0 / 20.0));
   RenderingEngine::GetInstance().LoadResources(this);

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
      if (GetAnimationByName("death")->curr_frame >= 14) {
         start_death_ = 14;
         end_death_ = 19;
      }
   }

   // Check if state is idle?
   if (enemy_state_ == IDLE) {
      // Let goop fall if it reaches the correct point in the animation.
      if (GetAnimationByName("idle")->curr_frame == 8 && shoot_timer_ > 20) {
         proj_ = CreateProjectile("fruig_projectile", 10.0f, 9.0f, -30, 0, 170, 10, 10, 0.0f, 0.0f);
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
      sprite_sheet->Animate(GetAnimationByName("idle"));
   } else if (enemy_state_ == DEATH) {
      sprite_sheet->Animate(GetAnimationByName("death"), start_death_, end_death_);
   }
}

// Get contact function
void Fruig::StartContact(Element *element) {
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
FleetSensor::FleetSensor(float width, float height, Entity *entity, CONTACT contact_type, float center_x, float center_y) :
   Sensor(height, width, entity, contact_type, center_x, center_y, 1.0f) {

   // Initialize
   initialize(width, height, center_x, center_y);
}

// Contact functions
void FleetSensor::StartContact(Element *element) {
   if (element) {
      if (element->type() == "Platform") {
         owner_->in_contact = true;
      } else if (element->type() == "Fleet") {
         owner_->body->ApplyLinearImpulseToCenter(b2Vec2(0.5f, 0.0f), true);
         owner_->in_contact = true;
      }
   }
}

void FleetSensor::end_contact(Element *element) {
   if (element) {
      if (element->type() == "Platform") {
         owner_->in_contact = false;
      } else if (element->type() == "Fleet") {
         owner_->in_contact = false;
      }
   }
}

///////////////////////////////////////////////////
/************* FLEET ENEMY ***********************/
///////////////////////////////////////////////////

// Constructor
Fleet::Fleet(int x, int y) :
   Enemy(x, y, 25, 49) {
   
   // Set shape
   element_shape.center = {0.0f, -0.08f};
   element_shape.dynamic = true;

   // Set the hitbox (28 x 12)
   SetHitbox(x, y, SQUARE, 1);

   // Create new sensor
   fleet_sensor_ = new FleetSensor(0.23f, 0.07f, this, CONTACT_DOWN, 0.0f, -0.35f);

   // Set health
   health = 20;

   // Set initial dir
   entity_direction = RIGHT;

   // Set enemy state
   enemy_state_ = IDLE;

   // Flip animations to start
   flipAllAnimations();
   texture_flipped = false;
}

// Load media function
bool Fleet::LoadMedia() {
   bool success = true;

   // Load master sheet
   std::string path = media_path + "Fleet/fleet_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("fleet_master_sheet", path.c_str());
   animations.emplace("idle", new Animation(sprite_sheet, "idle", 63.0, 87.0, 0.0, 11, 1.0 / 20.0));
   animations.emplace("turn", new Animation(sprite_sheet, "turn", 63.0, 87.0, 88.0, 11, 1.0 / 20.0));
   animations.emplace("death", new Animation(sprite_sheet, "death", 63.0, 87.0, 174.0, 18, 1.0 / 20.0));
   RenderingEngine::GetInstance().LoadResources(this);

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
         if (Application::GetInstance().get_player()->get_x() <= get_x() && entity_direction == RIGHT) {
            entity_direction = LEFT;
            enemy_state_ = TURN;
         } else if (Application::GetInstance().get_player()->get_x() > get_x() && entity_direction == LEFT) {
            entity_direction = RIGHT;
            enemy_state_ = TURN;
         }
      }

      // Check for turn state
      if (enemy_state_ == TURN) {
         // Complete texture
         if (GetAnimationByName("turn")->completed) {
            if (entity_direction == RIGHT) {
               if (textureFlipped()) {
                  flipAllAnimations();
                  texture_flipped = false;
               }
            } else if (entity_direction == LEFT) {
               if (!textureFlipped()) {
                  flipAllAnimations();
                  texture_flipped = true;
               }
            }
            GetAnimationByName("turn")->completed = false;
            GetAnimationByName("turn")->curr_frame = 0;
            enemy_state_ = IDLE;
         }
      }

      // Idle is him just hopping around towards the player
      if (enemy_state_ == IDLE) {
         // Magnitude of impulse
         float magnitude = sqrt(pow((Application::GetInstance().get_player()->body->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                                 + pow((Application::GetInstance().get_player()->body->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

         // Get a vector towards the player and apply as impulse
         if (in_contact) {
            if (GetAnimationByName("idle")->curr_frame == 1) {
               const b2Vec2 impulse = {(Application::GetInstance().get_player()->body->GetPosition().x - body->GetPosition().x) / magnitude * 1.50f, 7.0f};
               body->SetLinearVelocity(impulse);
            }

            GetAnimationByName("idle")->reset_frame = 0;
         } else {
            GetAnimationByName("idle")->reset_frame = 10;
         }
      }
   }
}

// Animate function
void Fleet::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      sprite_sheet->Animate(GetAnimationByName("idle"), GetAnimationByName("idle")->reset_frame, GetAnimationByName("idle")->stop_frame);
   } else if (enemy_state_ == TURN) {
      sprite_sheet->Animate(GetAnimationByName("turn"));
   } else if (enemy_state_ == DEATH) {
      sprite_sheet->Animate(GetAnimationByName("death"), GetAnimationByName("death")->max_frame);
   }
}

// Start contact function
void Fleet::StartContact(Element *element) {
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

Mosqueenbler::Mosqueenbler(int x, int y) :
   Enemy(x, y, 134, 246), spawn_num_of_egg_(1) {

   // Set element shape stuff
   element_shape.dynamic = true;
   element_shape.shape_type.square.height = 40;
   element_shape.shape_type.square.width = 164;
   element_shape.center = {0.0f, 0.50f};
   element_shape.density = 10000.0f;

   // Set hitbox
   SetHitbox(x, y);

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
bool Mosqueenbler::LoadMedia() {
   bool success = true;

   // Load data
   std::string path = media_path + "Mosqueenbler/mosqueenbler_master_sheet.png";
   RenderingEngine::GetInstance().LoadTexture("mosqueenbler_master_sheet", path.c_str());
   animations.emplace("idle", new Animation(sprite_sheet, "idle", 246.0, 134.0, 0.0, 12, 1.0 / 20.0));
   animations.emplace("attack", new Animation(sprite_sheet, "attack", 246.0, 134.0, 134.0, 12, 1.0 / 20.0));
   RenderingEngine::GetInstance().LoadResources(this);

   // Return if success
   return success;
}

// Move function
void Mosqueenbler::move() {
   // Allow it to float
   float y = 1 * cos(movement_timer_.getTicks() / 1000.0f) + 0.197f;
   body->SetLinearVelocity({0.0f, y});

   // Spawn enemies
   if (Application::GetInstance().get_level_flag() == Application::FORESTBOSS) {
      if (shoot_timer_ > 200) {
         enemy_state_ = ATTACK;
         if (GetAnimationByName("attack")->curr_frame == 8 && spawn_num_of_egg_ == 1) {
            MosquiblerEgg *egg = new MosquiblerEgg(get_x() + 95, get_y() + 134);
            Application::GetInstance().get_level()->add_enemy(egg);
            spawn_num_of_egg_ = 0;
         } else if (GetAnimationByName("attack")->completed) {
            shoot_timer_ = 0;
            spawn_num_of_egg_ = 1;
         }
      } else {
         enemy_state_ = IDLE;
         GetAnimationByName("attack")->curr_frame = 0;
      }

      ++shoot_timer_;
   }
}

// Animate function
void Mosqueenbler::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      sprite_sheet->Animate(GetAnimationByName("idle"));
   } else if (enemy_state_ == ATTACK) {
      sprite_sheet->Animate(GetAnimationByName("attack"));
   }
}

////////////////////////////////////////////////
/*********** MOSQUIBLER EGG ********************/
////////////////////////////////////////////////

// Constructor
MosquiblerEgg::MosquiblerEgg(int x, int y) :
   Enemy(x, y, 42, 28) {

   // Set hitbox
   element_shape.dynamic = true;
   SetHitbox(x, y);

   // Set enemy state
   enemy_state_ = IDLE;
}

// Load egg media
bool MosquiblerEgg::LoadMedia() {
   bool success = true;

   // Instantiate data
   std::vector<TextureData> data;
   data.push_back(TextureData(6, 1.0f / 20.0f, "idle", media_path + "Mosqueenbler/egg_idle.png"));
   data.push_back(TextureData(7, 1.0f / 20.0f, "attack", media_path + "Mosqueenbler/egg_break.png"));

   // Load resources
   success = RenderingEngine::GetInstance().LoadResources(this, data);

   return success;
}

// MosquiblerEgg move function
void MosquiblerEgg::move() {
   if (enemy_state_ == ATTACK) {
      if (GetAnimationByName("attack")->completed) {
         Application::GetInstance().get_level()->add_enemy(new Mosquibler(get_x(), get_y()));
         Application::GetInstance().get_level()->destroy_enemy(this);
      }
   }
}

// Animate
void MosquiblerEgg::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      sprite_sheet->Animate(GetAnimationByName("idle"));
   } else if (enemy_state_ == ATTACK) {
      sprite_sheet->Animate(GetAnimationByName("attack"));
   }
}

// Start contact for egg
void MosquiblerEgg::StartContact(Element *element) {
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
   Sensor(height, width, entity, contact_type, center_x, center_y, 1.0f, true) {}

// start contact
void WormoredSensor::StartContact(Element *element) {
   if (element) {
      if (element->type() == "Player" || element->type() == "Projectile") {
         //Wormored *wormored = dynamic_cast<Wormored*>(entity_);
         //Application::GetInstance().get_player()->TakeDamage(10);
      }
   }
}

// Constructor for Wormored
Wormored::Wormored(int x, int y) :
   Enemy(x, y, 418, 796),
   body_1_heights_({{0, 0}, {1, -2}, {2, -2}, {3, -2}, {4, -2}, {5, 2}, {6, 2},
                  {7, 2}, {8, 2}}),
   body_2_heights_({{2, 0}, {3, -1}, {4, -1}, {5, -2}, {6, -2}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 2}}),
   body_3_heights_({{5, 0}, {6, -1}, {7, -1}, {8, -2}, {9, -2}, {10, 1}, {11, 1}, {12, 1}, {13, 1}, {14, 2}}),
   body_4_heights_({{7, 0}, {8, -1}, {9, -1}, {10, -2}, {11, -2}, {12, 1}, {13, 1}, {14, 1}, {15, 1}, {16, 1}, {17, 1}}),
   body_5_heights_({{10, 0}, {11, -1}, {12, -1}, {13, -2}, {14, -2}, {15, 1}, {16, 1}, {17, 1}, {18, 1}, {19, 2}}),
   body_6_heights_({{12, 0}, {13, -1}, {14, -1}, {15, -2}, {16, -2}, {17, 2}, {18, 2}, {19, 1}, {20, 1}}) {

   // Set the body type to dynamic
   element_shape.dynamic = true;
   element_shape.shape_type.square.width = 640;
   element_shape.shape_type.square.height = 404;
   element_shape.center = {-0.05f, -0.06f};
   SetHitbox(x, y);
   
   // Set Wormored's collision logic
   b2Filter filter;
   filter.categoryBits = CAT_BOSS;
   filter.maskBits = CAT_PLATFORM;
   body->GetFixtureList()->SetFilterData(filter);

   // Add new sensors to the body when facing left
   // left_facing_sensors_[0] = new WormoredSensor(1.97f, 0.71f, this, CONTACT_UP, -2.34f, -0.06f);
   // left_facing_sensors_[1] = new WormoredSensor(1.97f, 0.62f, this, CONTACT_UP, -1.09, -0.06f);
   // left_facing_sensors_[2] = new WormoredSensor(1.81f, 0.40f, this, CONTACT_UP, -0.07f, -0.21f);
   // left_facing_sensors_[3] = new WormoredSensor(1.60f, 0.43f, this, CONTACT_UP, 0.77f, -0.43f);
   // left_facing_sensors_[4] = new WormoredSensor(1.15f, 0.30f, this, CONTACT_UP, 1.50f, -0.92f);
   // left_facing_sensors_[5] = new WormoredSensor(0.89f, 0.22f, this, CONTACT_UP, 2.02f, -1.16f);

   left_facing_sensors_[0] = new BodyPart(this, -243, -6, 143, 395, false, CAT_BOSS);
   left_facing_sensors_[1] = new BodyPart(this, -109, -6, 124, 395, false, CAT_BOSS);
   left_facing_sensors_[2] = new BodyPart(this, -7, -21, 80, 363, false, CAT_BOSS);
   left_facing_sensors_[3] = new BodyPart(this, 77, -43, 86, 320, false, CAT_BOSS);
   left_facing_sensors_[4] = new BodyPart(this, 150, -92, 60, 230, false, CAT_BOSS);
   left_facing_sensors_[5] = new BodyPart(this, 202, -116, 44, 179, false, CAT_BOSS);

   // Add new sensors to the body when facing left
   // right_facing_sensors_[0] = new WormoredSensor(1.97f, 0.71f, this, CONTACT_UP, 2.34f, -0.06f);
   // right_facing_sensors_[1] = new WormoredSensor(1.97f, 0.62f, this, CONTACT_UP, 1.09, -0.06f);
   // right_facing_sensors_[2] = new WormoredSensor(1.81f, 0.40f, this, CONTACT_UP, 0.07f, -0.21f);
   // right_facing_sensors_[3] = new WormoredSensor(1.60f, 0.43f, this, CONTACT_UP, -0.77f, -0.43f);
   // right_facing_sensors_[4] = new WormoredSensor(1.15f, 0.30f, this, CONTACT_UP, -1.50f, -0.92f);
   // right_facing_sensors_[5] = new WormoredSensor(0.89f, 0.22f, this, CONTACT_UP, -2.02f, -1.16f);

   // Deactivate right facing sensors to begin with
   for (int i = 0; i < 6; i++) {
      //right_facing_sensors_[i]->deactivate_sensor();
   }

   // Set state to idle
   enemy_state_ = IDLE;
   entity_direction = LEFT;
}

bool Wormored::LoadMedia() {
   bool success = true;

   // Instantiate data
   std::vector<TextureData> data;
   data.push_back(TextureData(21, 1.0f / 24.0f, "idle", media_path + "Wormored/idle.png"));
   data.push_back(TextureData(29, 1.0f / 24.0f, "turn", media_path + "Wormored/turn.png"));
   data.push_back(TextureData(22, 1.0f / 24.0f, "attack", media_path + "Wormored/attack.png"));
   data.push_back(TextureData(28, 1.0f / 24.0f, "excrete", media_path + "Wormored/excrete.png"));

   // Load resources
   success = RenderingEngine::GetInstance().LoadResources(this, data);

   return success;
}

void Wormored::move() {
   if (enemy_state_ != DEATH && enemy_state_ != ATTACK && enemy_state_ != EXCRETE) {
      if (Application::GetInstance().get_player()->get_x() < (get_x() - get_width() / 2) && entity_direction == RIGHT) {
         entity_direction = LEFT;
         enemy_state_ = TURN;

         // Activate left side and deactivate right side
         for (int i = 0; i < 6; i++) {
            left_facing_sensors_[i]->activate_sensor();
            //right_facing_sensors_[i]->deactivate_sensor();
         }
      } else if (Application::GetInstance().get_player()->get_x() > (get_x() + get_width() / 2) && entity_direction == LEFT) {
         entity_direction = RIGHT;
         enemy_state_ = TURN;

         // Activate left side and deactivate right side
         for (int i = 0; i < 6; i++) {
            //right_facing_sensors_[i]->activate_sensor();
            left_facing_sensors_[i]->deactivate_sensor();
         }
      }
   }

   if (enemy_state_ == IDLE) {
      if (entity_direction == LEFT) {
         b2Vec2 vel = {-0.25f, 0.0f};
         body->SetLinearVelocity(vel);

         // Move each individual body part according to the map
         prev_frame = curr_frame;
         curr_frame = GetAnimationByName("idle")->curr_frame;
         //if (prev_frame != curr_frame) {
            if (curr_frame < 9) {
               std::cout << "curr_frame = " << curr_frame << std::endl;
               std::cout << "offset = " << body_1_heights_[curr_frame] << std::endl;
               std::cout << "get_y() before = " << left_facing_sensors_[0]->get_y() << std::endl;
               left_facing_sensors_[0]->update(0, body_1_heights_[curr_frame]);
               left_facing_sensors_[0]->set_y(left_facing_sensors_[0]->get_y() + body_1_heights_[curr_frame]);
               std::cout << "get_y() after = " << left_facing_sensors_[0]->get_y() << std::endl;
            }
            if (curr_frame > 1 && curr_frame < 12) {
               //left_facing_sensors_[1]->update(0, body_2_heights_[curr_frame]);
            }
            if (curr_frame > 4 && curr_frame < 15) {
               //left_facing_sensors_[2]->update(0, body_3_heights_[curr_frame]);
            }
            if (curr_frame > 6 && curr_frame < 18) {
               //left_facing_sensors_[3]->update(0, body_4_heights_[curr_frame]);
            }
            if (curr_frame > 9 && curr_frame < 20) {
               //left_facing_sensors_[4]->update(0, body_5_heights_[curr_frame]);
            }
            if (curr_frame > 11 && curr_frame < 21) {
               //left_facing_sensors_[5]->update(0, body_6_heights_[curr_frame]);
            }
         //}
      } else if (entity_direction == RIGHT) {
         b2Vec2 vel = {0.25f, 0.0f};
         body->SetLinearVelocity(vel);
      }
   }

   if (enemy_state_ == TURN) {
      if (GetAnimationByName("turn")->curr_frame > GetAnimationByName("turn")->max_frame - 1) {
         // if (entity_direction == RIGHT) {
         //    GetAnimationByName("idle"]->flip_ = SDL_FLIP_HORIZONTAL;
         //    GetAnimationByName("turn"]->flip_ = SDL_FLIP_HORIZONTAL;
         // } else if (entity_direction == LEFT) {
         //    GetAnimationByName("idle"]->flip_ = SDL_FLIP_NONE;
         //    GetAnimationByName("turn"]->flip_ = SDL_FLIP_NONE;
         // }
         GetAnimationByName("turn")->completed = false;
         GetAnimationByName("turn")->curr_frame = 0;
         enemy_state_ = IDLE;
      }
   }
}

void Wormored::animate(Texture *tex, int reset, int max, int start) {
   if (enemy_state_ == IDLE) {
      sprite_sheet->Animate(GetAnimationByName("idle"));
   } else if (enemy_state_ == TURN) {
      sprite_sheet->Animate(GetAnimationByName("turn"));
   }
}

Texture *Wormored::get_texture() {
   if (enemy_state_ == IDLE) {
      // return GetAnimationByName("idle"];
   }

   if (enemy_state_ == TURN) {
      // return GetAnimationByName("turn"];
   }

   if (enemy_state_ == ATTACK) {
      // return GetAnimationByName("attack"];
   }

   if (enemy_state_ == EXCRETE) {
      // return GetAnimationByName("excrete"];
   }
   return nullptr;
}

Wormored::~Wormored() {
   for (int i = 0; i < 5; i++) {
      delete left_facing_sensors_[i];
      //delete right_facing_sensors_[i];
   }
}