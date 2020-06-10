#if 1

#include "QuiteGoodMachine/Source/ObjectManager/Private/Enemy.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Object.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Global.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Level.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "QuiteGoodMachine/Source/MathStructures/Private/Coordinates.h"

#include "QuiteGoodMachine/Source/MemoryManager/Private/ObjectManager.h"

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/EnemyStates.h"

#include <stdio.h>
#include <cmath>
#include <memory>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <Box2D/Box2D.h>

#include <glm/gtx/string_cast.hpp>

using namespace std;

/********************* ENEMY IMPLEMENTATIONS ******************/

Enemy::Enemy(std::string name, glm::vec3 initial_position, glm::vec3 size)
   : Entity(name, initial_position, size)
   , PositionalElement(name, initial_position, size)
   , shoot_timer_(101) 
{
   // Set
   start_death_ = 0;
   end_death_ = 0;

   // Projectile
   proj_ = nullptr;
}

// Create projectile
// TODO: Create a struct that holds all of the ending parameters and pass it through
//       such as to properly load each texture (one enemy has different sizes for GetAnimationByName(
// TODO: Do it based on GetPosition().x instead of get_tex_x() (will need to change consts)
Projectile* Enemy::CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y, 
      bool owner, bool damage, float force_x, float force_y) {
   
   // First, create a new projectile
   Projectile *proj;

   // Create based on direction
   if (GetDirection() == RIGHT) {
      proj = new Projectile(name, GetPosition().x + GetSize().x + delta_x_r, GetPosition().y + delta_y, 
            width, height, 0, damage, force_x, force_y, this);
   } else {
      proj = new Projectile(name, GetPosition().x + delta_x_l, GetPosition().y + delta_y,
            width, height, 0, damage, force_x, force_y, this);
   }

   // Set shot direction
   proj->shot_dir = GetDirection();

   // Return projectile reference
   return proj;
}

// Update function
void Enemy::Update(bool freeze) {
   // Call tangible element update and drawableelement updates last
   TangibleElement::Update(freeze);
   DrawableElement::Update(freeze);
}

std::shared_ptr<EnemyState> Enemy::GetCurrentEnemyState() {
   return std::static_pointer_cast<EnemyState>(GetStateContext()->GetCurrentState());
}

bool Enemy::within_bounds() {
   return Application::GetInstance().get_player()->GetPosition().y >= GetPosition().y - GetSize().y &&
            Application::GetInstance().get_player()->GetPosition().y <= GetPosition().y + GetSize().y;
}

Enemy::~Enemy() {}

/************** FECREEZ IMPLEMENTATIONS ********************/
Fecreez::Fecreez(std::string name, glm::vec3 initial_position) 
   : Enemy(name, initial_position, glm::vec3(82.f, 92.f, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(82.f, 92.f, 0.f))
{
   // Set the hitboxes
   SetHitbox(GetPosition().x, GetPosition().y);

   // Set health
   SetHealth(30);

   // Set entity direction to right
   SetDirection(LEFT);
}

// Load media function for fecreez
void Fecreez::LoadMedia() {
   // Flag for success
   bool success = true;

   // Instantiate animations
   std::string fecreez_path = Application::GetInstance().sprite_path + "Enemies/Fecreez/fecreez_master_sheet.png";
   Texture *temp = RegisterTexture(fecreez_path);

   // Register states
   GetStateContext()->RegisterState("turn", make_shared<Enemy_Turn>(GetStateContext().get(), temp, make_shared<Animation>(temp, "turn", 82.f, 92.f, 0.f, 7, 1.f / 20.f)));
   GetStateContext()->RegisterState("attack", make_shared<Fecreez_Attack>(GetStateContext().get(), temp, make_shared<Animation>(temp, "attack", 82.f, 92.f, 92.f, 9, 1.f / 20.f)));
   GetStateContext()->RegisterState("idle", make_shared<Fecreez_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 82.f, 92.f, 184.f, 18, 1.f / 20.f)));
   GetStateContext()->RegisterState("death", make_shared<Fecreez_Death>(GetStateContext().get(), temp, make_shared<Animation>(temp, "death", 143.f, 92.f, 276.f, 16, 1.f / 20.f)));

   // Set initial and reset
   GetStateContext()->SetResetState(GetStateContext()->GetState("idle"));
   GetStateContext()->SetInitialState(GetStateContext()->GetState("idle"));

   // Start flipped
   GetStateContext()->FlipAllAnimations();
}

void Fecreez::Move() {
   // Overall check to see if it's alive
   if (GetCurrentEnemyState() == GetStateContext()->GetState("death")) {
      // TODO: find better solution
      do_shift_once_([&]() {
         SetPosition(glm::vec3(GetPosition().x - 70, GetPosition().y, 0.f));
      });
   }

   // attack
   if (GetCurrentEnemyState() == GetStateContext()->GetState("attack")) {
      if (GetAnimationByName("attack")->curr_frame > 4 && shoot_timer_ >= 100) {
         Projectile *tmp = CreateProjectile("fecreez_projectile", 22.0f, 15.0f, 17, -12, 70, 0, 10, 10.0f, 0.0f);
         tmp->body->SetGravityScale(0);
         shoot_timer_ = 0;
      }
   }
}

// Start contact function
void Fecreez::StartContact(Element *element) {
   if (element && (element->type() == "Player" || element->type() == "Projectile")) {
      SetHealth(GetHealth() - 10);
      if (GetHealth() <= 0) {
         SetMarkedForDeath();
         SetCollision(CAT_PLATFORM);
      }
   }
}

Fecreez::~Fecreez() {}

//////////////////////////////////////////////////////
/********************* ARM **************************/
//////////////////////////////////////////////////////

Arm::Arm(std::string name, glm::vec3 initial_position, glm::vec3 size, Rosea *rosea)
   : Enemy(name, initial_position, size)
   , PositionalElement(name, initial_position, size)
   , rosea_(rosea)
   , name_(name) {}

// Callback function for arm will set rosea's state to hurt
void Arm::StartContact(Element *element) {
   if (element->type() == "Player") {
      std::shared_ptr<Player> temp = Application::GetInstance().get_player();
      b2Body *body = temp->GetBody();
      temp->TakeDamage(10);
      float f_y = body->GetLinearVelocity().y < 0 ? -body->GetLinearVelocity().y : body->GetLinearVelocity().y;
      body->SetLinearVelocity({-body->GetLinearVelocity().x * 1.5f, f_y * 1.5f});
   } else if (element->type() == "Projectile") {
      // if (rosea_->get_state() != ATTACK) {
      //    rosea_->set_state(HURT);
      // }
      was_hurt = true;
   }
}

//////////////////////////////////////////////////////
/****************** ROSEA ENEMY *********************/
//////////////////////////////////////////////////////

Rosea::Rosea(std::string name, glm::vec3 initial_position, glm::vec3 size, float angle) 
   : Enemy(name, initial_position, glm::vec3(144.f, 189.f, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(144.f, 189.f, 0.f))
   , arms_("still", glm::vec3(initial_position.x - 46, initial_position.y - 118, 0.f), glm::vec3(122.f, 78.f, 0.f), this)
   //, arms_("attack", glm::vec3(initial_position.x + 5, initial_position.y - 230, 0.f), glm::vec3(122.f, 387.f, 0.f), this)
   , angle_(angle)
   , arm_heights_({{0, initial_position.y - 110}, {1, initial_position.y - 250}, {2, initial_position.y - 325}, {3, initial_position.y - 395}, 
         {4, initial_position.y - 425}, {5, initial_position.y - 425}, {6, initial_position.y - 425}, 
         {7, initial_position.y - 425}, {8, initial_position.y - 425}, {9, initial_position.y - 380}, {10, initial_position.y - 270}, {11, initial_position.y - 180}, 
         {12, initial_position.y - 135}, {13, initial_position.y - 110}, {14, initial_position.y - 110}, {15, initial_position.y - 110}})
   , arm_widths_({{0, initial_position.x - 425}, {1, initial_position.x - 395}, {2, initial_position.x - 325}, {3, initial_position.x - 250}, 
         {4, initial_position.x - 80}, {5, initial_position.x - 80}, {6, initial_position.x - 80}, 
         {7, initial_position.x - 80}, {8, initial_position.x - 80}, {9, initial_position.x - 180}, {10, initial_position.x - 270}, {11, initial_position.x - 380}, 
         {12, initial_position.x - 400}, {13, initial_position.x - 425}, {14, initial_position.x - 425}, {15, initial_position.x - 425}}) {

   // TODO: Need to convert these to absolute values so that i can use them without using their relation
   // This means taking off the x - part...
   // TODO: Need to rework element set x, y bullshit to reflect use of transform models

   // Set anchors
   anchor_x = initial_position.x;
   anchor_y = initial_position.y;
   element_shape_.shape_type.square.angle = angle;

   // Set hitbox for rosea body
   SetHitbox(initial_position.x, initial_position.y);
   arms_.SetHitbox(arms_.GetPosition().x, arms_.GetPosition().y);
   //arms_still.SetHitbox(arms_still.GetPosition().x + 61, arms_still.GetPosition().y + 42);

   // TODO: translate should be based on sin and cos of angles given

   // Set health
   SetHealth(100);
}

// Load media for rosea
void Rosea::LoadMedia() {
   // Flag for success
   bool success = true;

   // Load rosea main body
   std::string rosea_path = Application::GetInstance().sprite_path + "Enemies/Rosea/rosea_master_sheet.png";
   Texture *temp = RegisterTexture(rosea_path);

   // Register states
   GetStateContext()->RegisterState("idle", make_shared<Rosea_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 189.f, 144.f, 0.f, 15, 1.f / 20.f)));
   GetStateContext()->RegisterState("hurt", make_shared<Rosea_Hurt>(GetStateContext().get(), temp, make_shared<Animation>(temp, "hurt", 189.f, 144.f, 144.f, 15, 1.f / 20.f)));

   std::string arm_path = Application::GetInstance().sprite_path + "Enemies/Rosea/rosea_arm_master_sheet.png";
   temp = RegisterTexture(arm_path);
   arms_.GetStateContext()->RegisterState("idle", make_shared<Rosea_ArmIdle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 112.f, 78.f, 0.f, 15, 1.f / 20.f)));
   arms_.GetStateContext()->RegisterState("hurt", make_shared<Rosea_ArmHurt>(GetStateContext().get(), temp, make_shared<Animation>(temp, "hurt", 112.f, 78.f, 78.f, 15, 1.f / 20.f)));
   arms_.GetStateContext()->RegisterState("attack", make_shared<Rosea_ArmAttack>(GetStateContext().get(), temp, make_shared<Animation>(temp, "attack", 122.f, 387.f, 156.f, 15, 1.f / 20.f)));
   arms_.GetStateContext()->RegisterState("retreat", make_shared<Rosea_ArmRetreat>(GetStateContext().get(), temp, make_shared<Animation>(temp, "retreat", 122.f, 387.f, 156.f, 15, 1.f / 20.f)));

   // Special state for 0 angle
   if (angle_ == 0.0f) {
      // Construct a matrix that will essentially rotate with the entire object
      arms_.draw_model_ = glm::translate(draw_model_, glm::vec3(10.0f, -(GetAnimationFromState()->half_height), 0.0f));

      // Set attack model (TODO: Change this to arms_.element_model)
      arms_.hitbox_model_ = glm::translate(hitbox_model_, glm::vec3(10.0f, -(GetAnimationFromState()->half_height + arms_.GetAnimationFromState()->half_height - 35.0f), 0.0f));
      // arms_.set_x(attack_model[3][0]);
      // arms_.set_y(0);
   }

   // Need to change arm position if rotated
   if (angle_ == 90.0f) {
      // Construct a matrix that will essentially rotate with the entire object
      arms_.draw_model_ = glm::translate(draw_model_, glm::vec3(GetAnimationFromState()->half_width - 19.0f, 10.0f, 0.0f));
      arms_.GetBody()->SetTransform(arms_.GetBody()->GetPosition(), M_PI / 2.0f);

      // Set attack model (TODO: Change this to arms_.element_model)
      arms_.hitbox_model_ = glm::translate(hitbox_model_, glm::vec3(GetAnimationFromState()->half_height + arms_.GetAnimationFromState()->half_height - 35.0f, 10.0f, 0.0f));
      arms_.GetBody()->SetTransform(arms_.GetBody()->GetPosition(), M_PI / 2.0f);
      // arms_.set_x(0);
      // arms_.set_y(attack_model[3][1]);

      // Rotate main body
      GetBody()->SetTransform(GetBody()->GetPosition(), M_PI / 2.0f);
   }

   // // Set hitbox to match locations
   // arms_still.set_x(arm_model[3][0]);
   // arms_still.set_y(arm_model[3][1]);
}

// Rosea update
void Rosea::Update(bool freeze) {
   Enemy::Update(freeze);
   arms_.TangibleElement::Update(freeze);
   arms_.DrawableElement::Update(freeze);
}

// Rosea move
void Rosea::Move() {
   // Check for enemy_state_ hurt
   if (GetCurrentEnemyState() == GetStateContext()->GetState("hurt")) {
      // Set arm height to 0
      if (angle_ == 0.0f) {
         arms_.SetPosition(glm::vec3(arms_.GetPosition().x, arms_.GetPosition().y + hitbox_model_[3][1], 0.f));
      } else {
         arms_.SetPosition(glm::vec3(arm_widths_[0], arms_.GetPosition().y, 0.f));
      }
   } else if (arms_.GetCurrentEnemyState() == arms_.GetStateContext()->GetState("retreat")) {
      if (angle_ == 0.0f) {
         arms_.SetPosition(glm::vec3(arms_.GetPosition().x, arm_heights_[arms_.GetAnimationByName("attack")->curr_frame], 0.f));
      } else {
         arms_.SetPosition(glm::vec3(arm_widths_[arms_.GetAnimationByName("attack")->curr_frame], arms_.GetPosition().y, 0.f));
      }
      if (arms_.GetCurrentEnemyState() == arms_.GetStateContext()->GetState("attack") && arms_.GetCurrentEnemyState()->GetAnimation()->completed) {
         if (angle_ == 0.0f) {
            arms_.SetPosition(glm::vec3(arms_.GetPosition().x, arms_.GetPosition().y - arms_.hitbox_model_[3][1], 0.f));
         } else {
            arms_.SetPosition(glm::vec3(arms_.GetPosition().x - arms_.hitbox_model_[3][0], arms_.GetPosition().y, 0.f));
         }
      }
   }

   // Now, check to see if player is within bounds
   if (GetCurrentEnemyState() != GetStateContext()->GetState("hurt")) {
      if (within_bounds()) {
         // Deactivate main body
         GetBody()->SetActive(false);

         // Sets hitbox to position of arm
         if (angle_ == 0.0f) {
            arms_.SetPosition(glm::vec3(arms_.GetPosition().x, arms_.hitbox_model_[3][1], 0.f));
         } else {
            arms_.SetPosition(glm::vec3(arms_.hitbox_model_[3][0], arms_.GetPosition().y, 0.f));
         }
      }
   }
}

// Get contact
void Rosea::StartContact(Element *element) {
   // Set enemy state to hurt
   if (element && element->type() == "Projectile") {
      if (GetCurrentEnemyState() != GetStateContext()->GetState("attack")) {
         was_hurt = true;
      }
   }
}

// Check to see if player is within bounds
bool Rosea::within_bounds() {
   if (angle_ == 0.0f) {
      if (Application::GetInstance().get_player()->GetPosition().x >= GetPosition().x - 250 
         && Application::GetInstance().get_player()->GetPosition().x <= GetPosition().x + 250) {
         return true;
      }
   } else {
      if (Application::GetInstance().get_player()->GetPosition().y >= (GetPosition().y - 250) 
         && Application::GetInstance().get_player()->GetPosition().y <= (GetPosition().y + 250)
         && Application::GetInstance().get_player()->GetPosition().x <= (GetPosition().x + 500)) {
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

Mosquibler::Mosquibler(std::string name, glm::vec3 initial_position) 
   : Enemy(name, initial_position, glm::vec3(107, 81, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(107, 81, 0.f))
{
   // Set element shape stuff
   element_shape_.dynamic = true;

   // Set hitbox
   SetHitbox(GetPosition().x, GetPosition().y, SQUARE, 2);
 
   // Set health
   SetHealth(10);

   // Set initial dir
   SetDirection(LEFT);
}

// Load media function
void Mosquibler::LoadMedia() {
   // Flag for success
   bool success = true;

   // Set sprite sheet
   std::string path = enemy_path + "Mosquibler/mosquibler_master_sheet.png";
   Texture *temp = RegisterTexture(path);

   GetStateContext()->RegisterState("fly", make_shared<Mosquibler_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "fly", 111.f, 99.f, 0.f, 12, 1.f / 20.f)));
   GetStateContext()->RegisterState("turn", make_shared<Enemy_Turn>(GetStateContext().get(), temp, make_shared<Animation>(temp, "turn", 111.f, 99.f, 99.f, 12, 1.f / 20.f)));
   GetStateContext()->RegisterState("hit", make_shared<Mosquibler_Hit>(GetStateContext().get(), temp, make_shared<Animation>(temp, "hit", 111.f, 99.f, 198.f, 4, 1.f / 20.f)));
   GetStateContext()->RegisterState("fall", make_shared<Mosquibler_Fall>(GetStateContext().get(), temp, make_shared<Animation>(temp, "fall", 111.f, 91.f, 297.f, 6, 1.f / 20.f)));
   GetStateContext()->RegisterState("death", make_shared<Mosquibler_Death>(GetStateContext().get(), temp, make_shared<Animation>(temp, "death", 111.f, 91.f, 388.f, 17, 1.f / 20.f)));
}

// Move function
void Mosquibler::Move() {
   // Body temp
   b2Body *body = GetBody();

   // Turn around
   if (GetCurrentEnemyState() == GetStateContext()->GetState("turn")) {
      // Magnitude of impulse
      float magnitude = sqrt(pow((Application::GetInstance().get_player()->GetBody()->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                              + pow((Application::GetInstance().get_player()->GetBody()->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

      // Get a vector towards the player and apply as impulse
      const b2Vec2 impulse = {(Application::GetInstance().get_player()->GetBody()->GetPosition().x - body->GetPosition().x) / magnitude * 0.90f, 
                     (Application::GetInstance().get_player()->GetBody()->GetPosition().y - body->GetPosition().y) / magnitude * 0.90f};
      
      // Apply impulse
      body->SetLinearVelocity(impulse);
   }

   // Fly towards player
   if (GetCurrentEnemyState() == GetStateContext()->GetState("fly")) {
      // Magnitude of impulse
      float magnitude = sqrt(pow((Application::GetInstance().get_player()->GetBody()->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                              + pow((Application::GetInstance().get_player()->GetBody()->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

      // Get a vector towards the player and apply as impulse
      const b2Vec2 impulse = {(Application::GetInstance().get_player()->GetBody()->GetPosition().x - body->GetPosition().x) / magnitude * 0.90f, 
                     (Application::GetInstance().get_player()->GetBody()->GetPosition().y - body->GetPosition().y) / magnitude * 0.90f};
      
      // Apply impulse
      body->SetLinearVelocity(impulse);
   }

   if (GetCurrentEnemyState() == GetStateContext()->GetState("death")) {
      body->SetLinearVelocity({0.0f, body->GetLinearVelocity().y});
   }
}

// Start contact function
void Mosquibler::StartContact(Element *element) {
   if (element) {
      if ((element->type() == "Player" || element->type() == "Projectile")) {
         // std::cout << "Mosquibler::StartContact() - hit by player\n";
         SetCollision(CAT_PLATFORM);
         was_hurt = true;
      }

      if (element->type() == "Platform" || element->type() == "Mosqueenbler") {
         if (was_hurt) {
            // std::cout << "Mosquibler::StartContact() - hit the ground\n";
            hit_ground = true;
         }
      }
   }
}

void Mosquibler::EndContact(Element *element) {
   if (element) {
      if (element->type() == "Platform") {
         hit_ground = false;
      }
   }
}

// Destructor
Mosquibler::~Mosquibler() {}

///////////////////////////////////////////////////
/************* FRUIG ENEMY ***********************/
///////////////////////////////////////////////////

// Constructor
Fruig::Fruig(std::string name, glm::vec3 initial_position)
   : Enemy(name, initial_position, glm::vec3(79.f, 110.f, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(79.f, 110.f, 0.f))
{

   // Set center
   element_shape_.center = {0.0f, -0.1f};

   // Set hitbox
   SetHitbox(GetPosition().x, GetPosition().y, SQUARE);

   // Set health
   SetHealth(10);

   // Set initial dir
   SetDirection(RIGHT);
}

// Load media function
void Fruig::LoadMedia() {
   std::string path = enemy_path + "Fruig/fruig_master_sheet.png";
   Texture *temp = RegisterTexture(path);

   GetStateContext()->RegisterState("idle", make_shared<Fruig_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 79.f, 140.f, 0.f, 20, 1.f / 20.f)));
   GetStateContext()->RegisterState("death", make_shared<Fruig_Death>(GetStateContext().get(), temp, make_shared<Animation>(temp, "death", 85.f, 136.f, 140.f, 20, 1.f / 20.f)));
}

// Mpve function
void Fruig::Move() {
   // Check if state is idle?
   if (GetCurrentEnemyState() == GetStateContext()->GetState("idle")) {
      // Let goop fall if it reaches the correct point in the animation.
      DrawState *draw_state = static_cast<DrawState*>(GetStateContext()->GetCurrentState().get());
      if (draw_state->GetAnimation()->curr_frame == 7 && shoot_timer_ > 20) {
         proj_ = CreateProjectile("fruig_projectile", 10.0f, 9.0f, -37, 0, 130, 10, 10, 0.0f, 0.0f);
         shoot_timer_ = 0;
      }

      // Update shoot timer
      shoot_timer_++;

      // Check for death of proj
      if (proj_ && !proj_->shift && proj_->get_state() == Object::DEAD) {
         // proj_->sub_tex_x(24);
         proj_->shift = true;
      }
   }
}

// Get contact function
void Fruig::StartContact(Element *element) {
   if ((element->type() == "Player" || element->type() == "Projectile") && IsAlive()) {
      SetHealth(GetHealth() - 10);
      if (GetHealth() == 0) {
         SetCollision(CAT_PLATFORM);
         SetMarkedForDeath();
      }
   }
}

///////////////////////////////////////////////////
/************* FLEET SENSOR **********************/
///////////////////////////////////////////////////

// Constructor
FleetSensor::FleetSensor(float width, float height, Entity *entity, Element::CONTACT contact_type, float center_x, float center_y) 
   : Sensor(height, width, entity, contact_type, center_x, center_y, 1.0f) 
{
   // Initialize
   initialize(width, height, center_x, center_y, CAT_SENSOR, CAT_PLATFORM | CAT_PLAYER | CAT_PROJECTILE | CAT_ENEMY | CAT_BOSS);
}

// Contact functions
void FleetSensor::StartContact(Element *element) {
   if (element) {
      if (element->type() == "Platform") {
         static_cast<Enemy*>(owner_)->hit_ground = true;
         owner_->GetBody()->SetLinearVelocity({0.0f, owner_->GetBody()->GetLinearVelocity().y});
      } else if (element->type() == "Fleet") {
         owner_->GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.5f, 0.0f), true);
         static_cast<Enemy*>(owner_)->hit_ground = true;
      }
   }
}

void FleetSensor::EndContact(Element *element) {
   if (element) {
      if (element->type() == "Platform") {
         static_cast<Enemy*>(owner_)->hit_ground = false;
      } else if (element->type() == "Fleet") {
         static_cast<Enemy*>(owner_)->hit_ground = false;
      }
   }
}

///////////////////////////////////////////////////
/************* FLEET ENEMY ***********************/
///////////////////////////////////////////////////

// Constructor
Fleet::Fleet(std::string name, glm::vec3 initial_position) 
   : Enemy(name, initial_position, glm::vec3(49.f, 25.f, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(49.f, 25.f, 0.f)) 
{
   
   // Set shape
   element_shape_.center = {0.0f, -0.08f};
   element_shape_.dynamic = true;

   // Set the hitbox (28 x 12)
   SetHitbox(GetPosition().x, GetPosition().y, SQUARE, 1);

   // Create new sensor
   fleet_sensor_ = new FleetSensor(0.23f, 0.07f, this, Element::CONTACT_DOWN, 0.0f, -0.35f);

   // Set health
   SetHealth(20);

   // Set initial dir
   SetDirection(RIGHT);

   // Flip animations to start
   // FlipAllAnimations();
   // texture_flipped = false;
}

// Load media function
void Fleet::LoadMedia() {
   // Load master sheet
   std::string path = enemy_path + "Fleet/fleet_master_sheet.png";
   Texture *temp = RegisterTexture(path);

   GetStateContext()->RegisterState("idle", make_shared<Fleet_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 65.f, 89.f, 0.f, 11, 1.f / 24.f)));
   GetStateContext()->RegisterState("turn", make_shared<Enemy_Turn>(GetStateContext().get(), temp, make_shared<Animation>(temp, "turn", 65.f, 89.f, 89.f, 11, 1.f / 24.f)));
   GetStateContext()->RegisterState("death", make_shared<Fleet_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "death", 65.f, 89.f, 178.f, 18, 1.f / 24.f)));
}

// Move function
void Fleet::Move() {
   // Body ptr
   b2Body *body = GetBody();

   // Only if alive
   if (IsAlive()) {
      // Idle is him just hopping around towards the player
      if (GetCurrentEnemyState() == GetStateContext()->GetState("idle")) {
         // Magnitude of impulse
         float magnitude = sqrt(pow((Application::GetInstance().get_player()->GetBody()->GetPosition().x - body->GetPosition().x) / 1.920f, 2)
                                 + pow((Application::GetInstance().get_player()->GetBody()->GetPosition().y - body->GetPosition().y) / 1.080f, 2));

         // Get a vector towards the player and apply as impulse
         if (hit_ground) {
            if (GetAnimationByName("idle")->curr_frame == 1) {
               const b2Vec2 impulse = {(Application::GetInstance().get_player()->GetBody()->GetPosition().x - body->GetPosition().x) / magnitude * 1.50f, 7.0f};
               body->SetLinearVelocity(impulse);
            }

            GetAnimationByName("idle")->reset_frame = 0;
         } else {
            GetAnimationByName("idle")->reset_frame = 10;
         }
      }
   }
}

// Start contact function
void Fleet::StartContact(Element *element) {
   if (element && (element->type() == "Player" || element->type() == "Projectile")) {
      SetHealth(GetHealth() - 10);
      if (GetHealth() <= 0) {
         was_hurt = true;
         b2Fixture *fixture_list = GetBody()->GetFixtureList();
         while (fixture_list) {
            SetCollision(CAT_PLATFORM, fixture_list);
            fixture_list = fixture_list->GetNext();
         }
      }
   }
}

////////////////////////////////////////////////
/*********** MOSQUEENBLER ENEMY ***************/
////////////////////////////////////////////////

Mosqueenbler::Mosqueenbler(std::string name, glm::vec3 initial_position) 
   : Enemy(name, initial_position, glm::vec3(246.f, 134.f, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(246.f, 134.f, 0.f))
   , spawn_num_of_egg_(1)
{
   // Set element shape stuff
   element_shape_.dynamic = true;
   element_shape_.shape_type.square.height = 40;
   element_shape_.shape_type.square.width = 164;
   element_shape_.center = {0.0f, 0.50f};
   element_shape_.density = 10000.0f;

   // Set hitbox
   SetHitbox(GetPosition().x, GetPosition().y);

   // Reset filter
   b2Filter filter;
   filter.categoryBits = CAT_PLATFORM;
   filter.maskBits = CAT_ENEMY | CAT_PLAYER | CAT_PROJECTILE;
   GetBody()->GetFixtureList()->SetFilterData(filter);

   // Set entity direction
   SetDirection(RIGHT);

   // set anchors
   anchor_x = GetPosition().x;
   anchor_y = GetPosition().y;

   // Start the timer
   movement_timer_.Start();

   // Set shoot timer
   shoot_timer_ = 0;
}

// Load the media
void Mosqueenbler::LoadMedia() {
   // Load data
   std::string path = enemy_path + "Mosqueenbler/mosqueenbler_master_sheet.png";
   Texture *temp = RegisterTexture(path);
   
   GetStateContext()->RegisterState("idle", make_shared<Mosqueenbler_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 248.f, 136.f, 0.f, 12, 1.f / 20.f)));
   GetStateContext()->RegisterState("attack", make_shared<Mosqueenbler_Attack>(GetStateContext().get(), temp, make_shared<Animation>(temp, "attack", 248.f, 136.f, 134.f, 12, 1.f / 20.f)));
}

// Move function
void Mosqueenbler::Move() {
   // Allow it to float
   float y = 1 * cos(movement_timer_.GetTime() / 1000.0f) + 0.197f;
   GetBody()->SetLinearVelocity({0.0f, y});

   // Spawn enemies
   if (Application::GetInstance().get_level_flag() == Application::FORESTBOSS) {
      if (shoot_timer_ > 200) {
         // enemy_state_ = ATTACK;
         if (GetAnimationByName("attack")->curr_frame == 8 && spawn_num_of_egg_ == 1) {
            std::shared_ptr<MosquiblerEgg> egg = std::make_shared<MosquiblerEgg>("mosquibler_egg", glm::vec3(GetPosition().x + 95, GetPosition().y + 134, 0.f));
            Application::GetInstance().get_level()->add_enemy(egg);
            spawn_num_of_egg_ = 0;
         } else if (GetAnimationByName("attack")->completed) {
            shoot_timer_ = 0;
            spawn_num_of_egg_ = 1;
         }
      } else {
         GetAnimationByName("attack")->curr_frame = 0;
      }

      ++shoot_timer_;
   }
}

////////////////////////////////////////////////
/*********** MOSQUIBLER EGG ********************/
////////////////////////////////////////////////

// Constructor
MosquiblerEgg::MosquiblerEgg(std::string name, glm::vec3 initial_position)
   : Enemy(name, initial_position, glm::vec3(28.f, 42.f, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(28.f, 42.f, 0.f))
{
   // Set hitbox
   element_shape_.dynamic = true;
   SetHitbox(GetPosition().x, GetPosition().y);
}

// Load egg media
void MosquiblerEgg::LoadMedia() {
   std::string path = enemy_path + "Mosqueenbler/mosqueenbler_egg_master_sheet.png";
   Texture *temp = RegisterTexture(path);

   GetStateContext()->RegisterState("idle", make_shared<MosquiblerEgg_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 94.f, 61.f, 0, 6, 1.f / 20.f)));
   GetStateContext()->RegisterState("attack", make_shared<MosquiblerEgg_Attack>(GetStateContext().get(), temp, make_shared<Animation>(temp, "attack", 94.f, 61.f, 61.f, 7, 1.f / 20.f)));
}

// MosquiblerEgg move function
void MosquiblerEgg::Move() {
   if (GetCurrentEnemyState() == GetStateContext()->GetState("attack")) {
      if (GetAnimationFromState()->completed) {
         Application::GetInstance().get_level()->add_enemy(std::shared_ptr<Mosquibler>(new Mosquibler("mosquibler", GetPosition())));
         Application::GetInstance().get_level()->destroy_enemy(shared_from_this());
      }
   }
}

// Start contact for egg
void MosquiblerEgg::StartContact(Element *element) {
   if (element->type() == "Platform") {
      // enemy_state_ = ATTACK;
      SetCollision(CAT_PLATFORM);
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
Wormored::Wormored(std::string name, glm::vec3 initial_position)
   : Enemy(name, initial_position, glm::vec3(796.f, 418.f, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(796.f, 418.f, 0.f))
   , tongue_("tongue", initial_position, glm::vec3(555.f, 37.f, 0.f))
   , body_1_heights_({{0, 0}, {1, -2}, {2, -2}, {3, -2}, {4, -2}, {5, 2}, {6, 2},
                  {7, 2}, {8, 2}})
   , body_2_heights_({{2, -3}, {3, -3}, {4, -3}, {5, -3}, {6, -3}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}})
   , body_3_heights_({{5, 0}, {6, -1}, {7, -1}, {8, -2}, {9, -2}, {10, 1}, {11, 1}, {12, 1}, {13, 1}, {14, 2}})
   , body_4_heights_({{7, 0}, {8, -1}, {9, -1}, {10, -2}, {11, -2}, {12, 1}, {13, 1}, {14, 1}, {15, 1}, {16, 1}, {17, 1}})
   , body_5_heights_({{10, 0}, {11, -1}, {12, -1}, {13, -2}, {14, -2}, {15, 1}, {16, 1}, {17, 1}, {18, 1}, {19, 2}})
   , body_6_heights_({{12, 0}, {13, -1}, {14, -1}, {15, -2}, {16, -2}, {17, 2}, {18, 2}, {19, 1}, {20, 1}}) 
{
   // Set the body type to dynamic
   element_shape_.dynamic = true;
   element_shape_.shape_type.square.width = 640;
   element_shape_.shape_type.square.height = 404;
   element_shape_.center = {-0.05f, -0.06f};
   SetHitbox(GetPosition().x, GetPosition().y);
   
   // Set Wormored's collision logic
   b2Filter filter;
   filter.categoryBits = CAT_BOSS;
   filter.maskBits = CAT_PLATFORM;
   GetBody()->GetFixtureList()->SetFilterData(filter);

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
      // right_facing_sensors_[i]->deactivate_sensor();
   }

   // Set state to idle
   // enemy_state_ = SLEEP;
   SetDirection(LEFT);
}

void Wormored::LoadMedia() {
   // Register as a correspondent
   PigeonPost::GetInstance().Register(GetName(), getptr());

   // Idle
   std::string path = enemy_path + "Wormored/idle.png";
   Texture *temp = RegisterTexture(path);
   GetStateContext()->RegisterState("idle", make_shared<Wormored_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 796.0f, 418.0f, 0.0f, 21, 1 / 24.0f, 2)));
   
   // Turn
   path = enemy_path + "Wormored/turn.png";
   temp = RegisterTexture(path);
   GetStateContext()->RegisterState("turn", make_shared<Enemy_Turn>(GetStateContext().get(), temp, make_shared<Animation>(temp, "turn", 796.0f, 418.0f, 0.0f, 29, 1.0f / 24.0f, 2)));

   // Attack
   path = enemy_path + "Wormored/attack.png";
   temp = RegisterTexture(path);
   GetStateContext()->RegisterState("attack", make_shared<Wormored_Attack>(GetStateContext().get(), temp, make_shared<Animation>(temp, "attack", 796.0f, 418.0f, 0.0f, 22, 1.0f / 24.0f, 2)));

   // Excrete
   path = enemy_path + "Wormored/excrete.png";
   temp = RegisterTexture(path);
   GetStateContext()->RegisterState("excrete", make_shared<Wormored_Excrete>(GetStateContext().get(), temp, make_shared<Animation>(temp, "excrete", 796.0f, 418.0f, 0.0f, 28, 1.0f / 24.0f, 2)));

   // Sleep
   path = enemy_path + "Wormored/sleep.png";
   temp = RegisterTexture(path);
   GetStateContext()->RegisterState("sleep", make_shared<Wormored_Excrete>(GetStateContext().get(), temp, make_shared<Animation>(temp, "sleep", 796.0f, 418.0f, 0.0f, 42, 1.0f / 24.0f, 3)));

   // Awake
   path = enemy_path + "Wormored/awake.png";
   temp = RegisterTexture(path);
   GetStateContext()->RegisterState("awake", make_shared<Wormored_Excrete>(GetStateContext().get(), temp, make_shared<Animation>(temp, "awake", 796.0f, 418.0f, 0.0f, 42, 1.0f / 24.0f, 3)));

   // Tongue
   path = enemy_path + "Wormored/tongue.png";
   temp = RegisterTexture(path);
   tongue_.GetStateContext()->RegisterState("extend", make_shared<Wormored_Excrete>(GetStateContext().get(), temp, make_shared<Animation>(temp, "extend", 555.0f, 37.0f, 0.0f, 1, 1.0f / 1.0f)));
}

void Wormored::Update(bool freeze) {   
   // Update State
   // ChangeState();

   // Move first
   // Move();

   // The animate
   // Animate();

   // // Render tongue
   // if (enemy_state_ == ATTACK) {
   //    tongue_texture->Render(get_anim_x() + 110.0f, get_anim_y() + 310.0f, 0.0f, GetAnimationByName("tongue"));
   // }

   // // Render wormored
   // if (anim) {
   //    anim->parent->Render(get_anim_x(), get_anim_y(), 0.0f, anim);
   // }
   TangibleElement::Update(freeze);
   DrawableElement::Update(freeze);
   tongue_.TangibleElement::Update(freeze);
   tongue_.DrawableElement::Update(freeze);
}

void Wormored::Move() {
   // Body
   b2Body *body = GetBody();

   if (GetCurrentEnemyState() == GetStateContext()->GetState("sleep")) {
      return;
   }

   // Constantly move wormored
   if (GetCurrentEnemyState() == GetStateContext()->GetState("idle") || GetCurrentEnemyState() == GetStateContext()->GetState("turn")) {
      if (GetDirection() == LEFT) {
         b2Vec2 vel = {-0.26f, 0.0f};
         body->SetLinearVelocity(vel);

         // Move each individual body part according to the map
         prev_frame = curr_frame;
         curr_frame = GetAnimationByName("idle")->curr_frame;
         if (curr_frame < 9) {
            left_facing_sensors_[0]->Update(0, body_1_heights_[curr_frame]);
            // left_facing_sensors_[0]->set_y(left_facing_sensors_[0]->GetPosition().y + body_1_heights_[curr_frame]);
         }
         if (curr_frame > 1 && curr_frame < 12) {
            left_facing_sensors_[1]->Update(0, body_2_heights_[curr_frame]);
            // left_facing_sensors_[1]->set_y(left_facing_sensors_[1]->GetPosition().y + body_2_heights_[curr_frame]);
         }
         if (curr_frame > 4 && curr_frame < 15) {
            left_facing_sensors_[2]->Update(0, body_3_heights_[curr_frame]);
            // left_facing_sensors_[2]->set_y(left_facing_sensors_[2]->GetPosition().y + body_3_heights_[curr_frame]);
         }
         if (curr_frame > 6 && curr_frame < 18) {
            left_facing_sensors_[3]->Update(0, body_4_heights_[curr_frame]);
            // left_facing_sensors_[3]->set_y(left_facing_sensors_[3]->GetPosition().y + body_4_heights_[curr_frame]);
         }
         if (curr_frame > 9 && curr_frame < 20) {
            left_facing_sensors_[4]->Update(0, body_5_heights_[curr_frame]);
            // left_facing_sensors_[4]->set_y(left_facing_sensors_[4]->GetPosition().y + body_5_heights_[curr_frame]);
         }
         if (curr_frame > 11 && curr_frame < 21) {
            left_facing_sensors_[5]->Update(0, body_6_heights_[curr_frame]);
            // left_facing_sensors_[5]->set_y(left_facing_sensors_[5]->GetPosition().y + body_6_heights_[curr_frame]);
         }
      } else if (GetDirection() == RIGHT) {
         b2Vec2 vel = {0.25f, 0.0f};
         body->SetLinearVelocity(vel);
      }
   }

   // TODO: on 5th frame, make tongue appear and start it moving
   if (GetCurrentEnemyState() == GetStateContext()->GetState("attack")) {
      body->SetLinearVelocity({0.0f, 0.0f});
   }
}

/*
void Wormored::ChangeState() {
   // Don't do anything if sleeping
   if (enemy_state_ == SLEEP) {
      return;
   }

   // Wait until done being awake to be idle
   if (enemy_state_ == AWAKE) {
      if (AnimationCompleted("awake")) {
         enemy_state_ = IDLE;
         std::vector<int> recipients;
         int id = ObjectManager::GetInstance().GetUID("platform_texture");
         int myid = ObjectManager::GetInstance().GetUID("wormored");
         if (id != -1) {
            recipients.push_back(id);
            Correspondence correspondence = Correspondence::CompileCorrespondence((void *) "CloseDoors", "string", recipients, myid);
            PigeonPost::GetInstance().Send(correspondence);
         }
      }
   }

   // Attack if wormored and player facing left
   std::shared_ptr<Player> player = Application::GetInstance().get_player();

   // Turn if needed
   if (enemy_state_ != DEATH && enemy_state_ != ATTACK && enemy_state_ != EXCRETE) {
      if (player->GetPosition().x < (GetPosition().x - get_width() / 2) && entity_direction == RIGHT) {
         entity_direction = LEFT;
         enemy_state_ = TURN;

         // Activate left side and deactivate right side
         for (int i = 0; i < 6; i++) {
            left_facing_sensors_[i]->activate_sensor();
            // right_facing_sensors_[i]->deactivate_sensor();
         }
      } else if (player->GetPosition().x > (GetPosition().x + get_width() / 2) && entity_direction == LEFT) {
         entity_direction = RIGHT;
         enemy_state_ = TURN;

         // Activate left side and deactivate right side
         for (int i = 0; i < 6; i++) {
            // right_facing_sensors_[i]->activate_sensor();
            left_facing_sensors_[i]->deactivate_sensor();
         }
      }
   }

   if (enemy_state_ != TURN) {
      if (entity_direction == LEFT && player->GetPosition().x < (GetPosition().x - get_width() / 2)) {
         // Add if check for timer
         if (attack_timer_.GetTime() > 3.5) {
            enemy_state_ = ATTACK;
            attack_timer_.Reset();
         } else {
            attack_timer_.Start();
         }
      }
   }

   if (enemy_state_ == ATTACK) {
      if (AnimationCompleted("attack")) {
         enemy_state_ = IDLE;
         attack_timer_.Reset();
      }
   }
}
*/

void Wormored::ProcessCorrespondence(const std::shared_ptr<Correspondence>& correspondence) {
   // Return if state not sleep
   if (GetCurrentEnemyState() != GetStateContext()->GetState("sleep")) {
      return;
   }
   
   // Create strying from message
   char *msg = (char *) (correspondence->GetMessage());
   std::string fmsg(msg);
   if (fmsg == "InitiateBattle") {
      GetStateContext()->SetState(GetStateContext()->GetState("awake"));
      attack_timer_.Start();
   }
}

Wormored::~Wormored() {
   for (int i = 0; i < 5; i++) {
      // delete left_facing_sensors_[i];
      // delete right_facing_sensors_[i];
   }
}

#endif