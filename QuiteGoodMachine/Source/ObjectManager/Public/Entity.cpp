#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Object.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Timers/FPSTimer.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Global.h"
#include "QuiteGoodMachine/Source/MemoryManager/Private/ObjectManager.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Level.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h"

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/PlayerStates.h"

#include <cmath>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <Box2D/Box2D.h>

#define BOUNDED(var) (var > -0.0000001f && var < 0.0000001f)
#define KH Application::GetInstance().key_handler

using namespace std;

/*************************** ENTITY IMPLEMENTATIONS ******************************/

// Entity constructor which will provide basic establishment for all entities
Entity::Entity(std::string name, glm::vec3 initial_position, glm::vec3 size)
   : TangibleElement(name, initial_position, size)
   , DrawableElement(name, initial_position, size)
   , PositionalElement(name, initial_position, size)
{
}

// Update function for all entities. For now all it does is call move
void Entity::Update(bool freeze) {
   // Call move first
   Move();

   // Render at center of draw
   DrawState *draw_state = static_cast<DrawState*>(GetStateContext()->GetCurrentState().get());
   float x = (body_->GetPosition().x * 100.f) - (draw_state->GetAnimation()->texture_width / 2.0f);
   float y = (body_->GetPosition().y * -100.f) - (draw_state->GetAnimation()->texture_height / 2.0f);
   SetPosition(glm::vec3(x, y, 0.f));

   // Call element draw
   DrawableElement::Update(freeze);
}

// Create projectile
Projectile* Entity::CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y,
     bool owner, bool damage, float force_x, float force_y) {
   
   // First, create a new projectile
   Projectile *proj;

   // Create based on direction
   if (GetDirection() == RIGHT) {
      proj = new Projectile("none", GetPosition().x + GetSize().x + delta_x_r, GetPosition().y + delta_y, 
            0.0f, 0.0f, owner, damage, force_x, force_y, this);
   } else {
      proj = new Projectile("none", GetPosition().x + delta_x_l, GetPosition().y + delta_y, owner,
            0.0f, 0.0f, damage, force_x, force_y, this);
   }

   // Set shot direction
   proj->shot_dir = GetDirection();

   // Return projectile reference
   return proj;
}


// Destructor
Entity::~Entity() {}

/////////////////////////////////////////////////////////////////////////
/******************* PLAYER BODY PARTS *********************************/
/////////////////////////////////////////////////////////////////////////

/********* HEAD **************/

PlayerHead::PlayerHead(Player *player, float x_rel, float y_rel) :
   BodyPart(player, x_rel, y_rel, 14, 44) {

   // Initialize the body part
   float width = get_width() / 200.0f;
   float height = get_height() / 200.0f;
   float x = get_x() / 100.0f;
   float y = get_y() / 100.0f;
   Sensor::initialize(width, height, x, y, CAT_PLAYER, CAT_PLATFORM, true);
}

void PlayerHead::StartContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         player->contacts_[Player::HEAD] = 1;
      } else if (element->type() == "Projectile" || !element->is_enemy()) {
         player->TakeDamage(10);
      }
   }
}

void PlayerHead::EndContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         player->contacts_[Player::HEAD] = 0;
      }
   }
}

/********* ARM ****************/

PlayerArm::PlayerArm(Player *player, float x_rel, float y_rel, int width, int height, std::string type) :
   BodyPart(player, x_rel, y_rel, width, height), type_(type) {

   // Initialize the body part
   Sensor::initialize(width / 200.0f, height / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER, CAT_PLATFORM, true);
}

void PlayerArm::StartContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftArm") {
            player->contacts_[Player::LEFT_ARM] = 1;
            // std::cout << "PlayerArm::StartContact() - left arm in contact\n";
         } else {
            player->contacts_[Player::RIGHT_ARM] = 1;
            // std::cout << "PlayerArm::StartContact() - right arm in contact\n";
         }
      } else if (element->type() == "Projectile" || !element->is_enemy()) {
         player->TakeDamage(10);
      }
   }
}

void PlayerArm::EndContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftArm") {
            player->contacts_[Player::LEFT_ARM] = 0;
            // std::cout << "PlayerArm::StartContact() - lost contact left arm\n";
         } else {
            player->contacts_[Player::RIGHT_ARM] = 0;
            // std::cout << "PlayerArm::StartContact() - lost contact right arm\n";
         }
      }
   }
}

/********* HAND ****************/

PlayerHand::PlayerHand(Player *player, float x_rel, float y_rel, std::string type) :
   BodyPart(player, x_rel, y_rel, 7, 6), type_(type) {

   // Initialize the body part
   Sensor::initialize(get_width() / 200.0f, get_height() / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER, CAT_PLATFORM, true);
}

void PlayerHand::StartContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftHand") {
            player->contacts_[Player::LEFT_HAND] = 1;
            // std::cout << "PlayerHand::StartContact() - in contact with left hand\n";
         } else {
            player->contacts_[Player::RIGHT_HAND] = 1;
         }
      } else if (element->type() == "Projectile" || !element->is_enemy()) {
         player->TakeDamage(10);
      }
   }
}

void PlayerHand::EndContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftHand") {
            player->contacts_[Player::LEFT_HAND] = 0;
            // std::cout << "PlayerHand::EndContact() - lost contact with left hand\n";
         } else {
            player->contacts_[Player::RIGHT_HAND] = 0;
         }
      }
   }
}

/********* LEG *****************/

PlayerLeg::PlayerLeg(Player *player, float x_rel, float y_rel, int width, int height, std::string type) :
   BodyPart(player, x_rel, y_rel, width, height), type_(type) {

   // Initialize the body part
   Sensor::initialize(width / 200.0f, height / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER, CAT_PLATFORM | CAT_BOSS, true);
}

void PlayerLeg::StartContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (element->type() == "Platform" && element->GetName() == "platform_2" && Level::GetInstance().level == Application::FORESTBOSS) {
            std::vector<int> recipients;
            int id = ObjectManager::GetInstance().GetUID("wormored");
            int myid = ObjectManager::GetInstance().GetUID("player");
            if (id != -1) {
               recipients.push_back(id);
               Correspondence correspondence = Correspondence::CompileCorrespondence((void *) "InitiateBattle", "string", recipients, myid);
               PigeonPost::GetInstance().Send(correspondence);
            }
         }
         if (sub_type() == "PlayerLeftLeg") {
            player->contacts_[Player::LEFT_LEG] = 1;
            // std::cout << "PlayerLeg::StartContact() - in contact left leg: " << element->type() << std::endl;
         } else {
            player->contacts_[Player::RIGHT_LEG] = 1;
            // std::cout << "PlayerLeg::StartContact() - in contact right leg: " << element->type() << std::endl;
         }
         player->has_jumped_ = 0;
         // owner_->GetAnimationByName("jump")->reset_frame = 0;
         // owner_->GetAnimationByName("jump")->curr_frame = 0;
         // owner_->GetAnimationByName("double_jump")->reset_frame = 0;
         // owner_->GetAnimationByName("double_jump")->curr_frame = 0;
         // owner_->GetAnimationByName("running_jump")->reset_frame = 0;
         // owner_->GetAnimationByName("running_jump")->curr_frame = 0;
      } else if (element->type() == "Projectile" || !element->is_enemy()) {
         player->TakeDamage(10);
      }
   }
}

void PlayerLeg::EndContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftLeg") {
            player->contacts_[Player::LEFT_LEG] = 0;
            // std::cout << "PlayerLeg::EndContact() - lost contact left leg\n";
         } else {
            player->contacts_[Player::RIGHT_LEG] = 0;
            // std::cout << "PlayerLeg::EndContact() - lost contact right leg\n";
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////
/******************** PLAYER IMPLEMENTATIONS ***************************/
/////////////////////////////////////////////////////////////////////////

// Initializ the player by calling it's constructor
Player::Player() 
   : Entity("player", glm::vec3(960.f, 412.f, 0.f), glm::vec3(31.f, 104.f, 0.f))
   , PositionalElement("player", glm::vec3(960.f, 412.f, 0.f), glm::vec3(31.f, 104.f, 0.f))
   , shooting(false)
   , arm_delta_x(12)
   , arm_delta_y(64)
   , arm_delta_shoot_x(12)
   , arm_delta_shoot_y(51)
   , arm_("arm", glm::vec3(960.f, 412.f, 0.f), glm::vec3(9.f, 27.f, 0.f))
   , immunity_duration_(1.0f)
   , edge_duration_(0.025f)
   , fall_duration_(0.015f)
   ,eraser(nullptr)
   , num_of_projectiles(0) 
{

   // Set entity direction
   SetDirection(RIGHT);

   // Setup Box2D
   // Set body type
   body_def_.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x = 600.0f * Application::GetInstance().to_meters_;
   float y = -412.5f * Application::GetInstance().to_meters_;
   body_def_.position.Set(x, y);
   body_def_.fixedRotation = true;

   // Set transform
   draw_model_ = glm::translate(draw_model_, glm::vec3(x, y, 0.0f));

   // Attach body to world
   body_ = Application::GetInstance().world_.CreateBody(&body_def_);

   // Set box dimensions
   float width = (GetSize().x / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;// - 0.11f;
   float height = (GetSize().y / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;// - 0.11f;
   const b2Vec2 center = {(PC_OFF_X - GetSize().x) / 2.0f * Application::GetInstance().to_meters_, 
                          PC_OFF_Y * Application::GetInstance().to_meters_};
   shape_.SetAsBox(width, height);

   // Set various fixture definitions and create fixture
   fixture_def_.shape = &shape_;
   fixture_def_.density = 1.25f;
   fixture_def_.friction = 0.0f;
   fixture_def_.userData = this;
   main_fixture_ = body_->CreateFixture(&fixture_def_);

   // Set sensors to non interactive with da enemy
   b2Filter filter;
   filter.categoryBits = CAT_PLAYER;
   filter.maskBits = CAT_PLATFORM | CAT_PROJECTILE | CAT_ENEMY | CAT_BOSS;
   b2Fixture *fixture_list = body_->GetFixtureList();
   fixture_list->SetFilterData(filter);

   // ADD BODY PARTS FOR LEFT BODY
   // player_body_right_.push_back(new PlayerHead(this, 11.5f, 26));
   // player_body_right_.push_back(new PlayerArm(this, 3.5, -7, 6, 22, "PlayerLeftArm"));
   // player_body_right_.push_back(new PlayerArm(this, 19.5, -7, 6, 22, "PlayerRightArm"));
   // // player_body_right_.push_back(new PlayerHand(this, 6, -41, "PlayerLeftHand"));
   // // player_body_right_.push_back(new PlayerHand(this, 17, -41, "PlayerRightHand"));
   // player_body_right_.push_back(new PlayerLeg(this, 5.5, -33, 6, 34, "PlayerLeftLeg"));
   // player_body_right_.push_back(new PlayerLeg(this, 18.5, -33, 6, 34, "PlayerRightLeg"));

   // ADD BODY PARTS FOR RIGHT BODY
   // player_body_left_.push_back(new PlayerHead(this, 11.5f, 26));
   player_body_left_.push_back(new PlayerArm(this, -19.5, -7, 6, 22, "PlayerLeftArm"));
   player_body_left_.push_back(new PlayerArm(this, 19.5, -7, 6, 22, "PlayerRightArm"));
   // player_body_left_.push_back(new PlayerHand(this, 6, -41, "PlayerLeftHand"));
   // player_body_left_.push_back(new PlayerHand(this, 17, -41, "PlayerRightHand"));
   player_body_left_.push_back(new PlayerLeg(this, -11, -33, 2, 34, "PlayerLeftLeg"));
   player_body_left_.push_back(new PlayerLeg(this, 11, -33, 2, 34, "PlayerRightLeg"));

   // Deactivate the left
   // for (int i = 0; i < player_body_left_.size(); i++) {
   //    player_body_left_[i]->Sensor::deactivate_sensor();
   // }

   // Set health. TODO: set health in a better way
   SetHealth(30);

   // TEMPORARY SOLUTION
   //textures["arm_throw"]->completed_ = true;

   /************* PLAYER *******************************************/
   // Set in contact = false
   for (int i = 0; i < 5; i++) {
      contacts_[i] = 0;
   }

   // Start the immunity timer
   immunity_timer_.Start();
}

// Update function
void Player::Update(bool freeze) {
   // Apply artificial force of gravity
   const b2Vec2 sim_grav = {0.0f, SIM_GRAV};
   GetBody()->ApplyForceToCenter(sim_grav, true);

   // // Animate the function
   // Animate();

   // // Update player if not frozen
   // if (!freeze) {
   //    Move();
   // }

   // // Adjust deltas first
   // adjust_deltas();

   // // Render arm if idle, Render shooting if not
   // if (player_state_ != PUSH && player_state_ != JUMP_AND_PUSH && player_state_ != BALANCE && player_state_ != DEATH) {
   //    if (!shooting) {
   //       std::string arm_type;
   //       if (get_player_state() == RUN) {
   //          arm_type = "running_arm";
   //       } else if (get_player_state() == DOUBLE_JUMP) {
   //          arm_type = "double_jump_arm";
   //       } else {
   //          arm_type = "idle_arm";
   //       }
   //       arm_sheet->Render(get_anim_x() + get_width() + arm_delta_x, 
   //          get_anim_y() + arm_delta_y, 0.0f, GetAnimationByName(arm_type));
   //    } else {
   //       arm_sheet->Render(get_anim_x() + get_width() + arm_delta_shoot_x,
   //          get_anim_y() + arm_delta_shoot_y, 0.0f, GetAnimationByName("arm_throw"));
   //    }
   // }

   // // Render player
   // sprite_sheet->Render(get_anim_x(), get_anim_y(), 0.0f, GetAnimationFromState());

   // Call tangible element update and drawableelement updates last
   TangibleElement::Update(freeze);
   DrawableElement::Update(freeze);
}

#define STAND GetStateContext()->GetState("stand")
#define RUN GetStateContext()->GetState("running")
#define BALANCE GetStateContext()->GetState("balance")
#define JUMP GetStateContext()->GetState("jump")
#define RUN_AND_JUMP GetStateContext()->GetState("running_jump")
#define DOUBLE_JUMP GetStateContext()->GetState("double_jump")
#define JUMP_AND_PUSH GetStateContext()->GetState("jump_push")
#define PUSH GetStateContext()->GetState("push")

// Adjust delta function
void Player::adjust_deltas() {
   shared_ptr<StateInterface> player_state_ = GetStateContext()->GetCurrentState();
   if (player_state_ == STAND) {
      if (GetDirection() == RIGHT) {
         arm_delta_x = 2;
         arm_delta_y = 44;
         arm_delta_shoot_x = 3;
         arm_delta_shoot_y = 39;
      } else {
         arm_delta_x = -14;
         arm_delta_y = 44;
         arm_delta_shoot_x = -51;
         arm_delta_shoot_y = 39;
      }
   } else if (player_state_ == RUN && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      if (GetDirection() == RIGHT) {
         arm_delta_x = 2;
         arm_delta_y = 44;
         arm_delta_shoot_x = 8;
         arm_delta_shoot_y = 39;
      } else {
         arm_delta_x = -14;
         arm_delta_y = 44;
         arm_delta_shoot_x = -52;
         arm_delta_shoot_y = 39;
      }
   } else if (player_state_ == RUN_AND_JUMP || player_state_ == JUMP || player_state_ == DOUBLE_JUMP) {
      // Adjust deltas
      if (GetDirection() == RIGHT) {
         arm_delta_x = 3;
         arm_delta_y = 43;
         arm_delta_shoot_x = 4;
         arm_delta_shoot_y = 34;
      } else {
         arm_delta_x = -14;
         arm_delta_y = 43;
         arm_delta_shoot_x = -51;
         arm_delta_shoot_y = 34;
      }
   }
}

#if 0

// Animate based on state
void Player::Animate(Texture *tex, int reset, int max, int start) {
   // Shooting animation
   if (shooting) {
      arm_sheet->Animate(GetAnimationByName("arm_throw"));
   } else {
      arm_sheet->Animate(GetAnimationByName("idle_arm"));
   }

   // Choose animation based on what state player is in
   if (player_state_ == STAND) {
      // Choose a random number if at least one of the animations is complete
      // std::cout << curr_idle_animation->curr_frame << " " << curr_idle_animation->max_frame << std::endl;
      if (curr_idle_animation->curr_frame >= curr_idle_animation->max_frame) {
         rand_idle = rand() % 100 + 1;

         // Set curr_idle_texture
         if (rand_idle <= 98) {
            curr_idle_animation = GetAnimationByName("tap");
         }
         if (rand_idle == 99) {
            curr_idle_animation = GetAnimationByName("look");
         }
         if (rand_idle == 100) {
            curr_idle_animation = GetAnimationByName("kick");
         }
      }

      // Choose based on random number
      if (rand_idle <= 98) {
         sprite_sheet->Animate(GetAnimationByName("tap"));
      }
      if (rand_idle == 99) {
         sprite_sheet->Animate(GetAnimationByName("look"));
      }
      if (rand_idle == 100) {
         sprite_sheet->Animate(GetAnimationByName("kick"));
      }
   } else if (player_state_ == RUN && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      sprite_sheet->Animate(GetAnimationByName("running"));
      arm_sheet->Animate(GetAnimationByName("running_arm"));
   } else if (player_state_ == STOP && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      sprite_sheet->Animate(GetAnimationByName("running"));
   } else if (player_state_ == JUMP) {
      sprite_sheet->Animate(GetAnimationByName("jump"), GetAnimationByName("jump")->reset_frame, GetAnimationByName("jump")->stop_frame);
   } else if (player_state_ == RUN_AND_JUMP) {
      sprite_sheet->Animate(GetAnimationByName("running_jump"), GetAnimationByName("running_jump")->reset_frame, GetAnimationByName("running_jump")->stop_frame);
   } else if (player_state_ == DOUBLE_JUMP) {
      sprite_sheet->Animate(GetAnimationByName("double_jump"), GetAnimationByName("double_jump")->reset_frame, GetAnimationByName("double_jump")->stop_frame);
      arm_sheet->Animate(GetAnimationByName("double_jump_arm"), GetAnimationByName("double_jump_arm")->reset_frame, GetAnimationByName("double_jump_arm")->stop_frame);
   } else if (player_state_ == PUSH) {
      sprite_sheet->Animate(GetAnimationByName("push"));
   } else if (player_state_ == JUMP_AND_PUSH) {
      sprite_sheet->Animate(GetAnimationByName("jump_push"));
   } else if (player_state_ == BALANCE) {
      sprite_sheet->Animate(GetAnimationByName("balance"));
   } else if (player_state_ == DEATH) {
      sprite_sheet->Animate(GetAnimationByName("death"), 19, 19);
   }
}

// Change player state
void Player::change_player_state() {
   // TODO: Ask shane for an on the edge of a platform sprite sheet which is tested
   // by seeing if the main body is in contact with the platform but the feet sensors arent.
   // Velocities
   float vel_x = body->GetLinearVelocity().x;
   float vel_y = body->GetLinearVelocity().y;
   
   // Key touches
   bool right = Application::GetInstance().current_key_states_[SDL_SCANCODE_RIGHT];
   bool left = Application::GetInstance().current_key_states_[SDL_SCANCODE_LEFT];
   bool up = Application::GetInstance().current_key_states_[SDL_SCANCODE_UP];

   /*
   // Remove velocities if not pressing left or right
   if ((!left && entity_direction == LEFT) || (!right && entity_direction == RIGHT)) {
      body->SetLinearVelocity({0.0f, body->GetLinearVelocity().y});
   }
   */

   // Special push state
   if (contacts_[LEFT_ARM] || contacts_[RIGHT_ARM]) {
      if ((left || right) && (up || !(contacts_[LEFT_LEG] || contacts_[RIGHT_LEG]))) {
         player_state_ = JUMP_AND_PUSH;
      } else if (!left && !right && (up || !(contacts_[LEFT_LEG] || contacts_[RIGHT_LEG]))) {
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

   if (fall_timer_.GetTime() / (double) CLOCKS_PER_SEC > fall_duration_) {
      player_state_ = JUMP;
      fall_timer_.Stop();
   }

   // Might be a hack, but essentially only let it switch if double jump is completed
   if (has_jumped_ == 2) {
      player_state_ = DOUBLE_JUMP;
      return;
   }

   // Change state to balance if one leg on and the other one not
   if ((contacts_[LEFT_LEG] && !contacts_[RIGHT_LEG] && entity_direction == RIGHT) || (!contacts_[LEFT_LEG] && contacts_[RIGHT_LEG] && entity_direction == LEFT)) {
      edge_timer_.Start();
      if (edge_timer_.GetTime() / (double) CLOCKS_PER_SEC > edge_duration_) {
         player_state_ = BALANCE;
      } else {
         player_state_ = STAND;
      }
      return;
   } else {
      edge_timer_.Stop();
   }

   // Special stand state
   if ((!right && !left) && BOUNDED(vel_x) && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      // Set state to stand
      player_state_ = STAND;

      // Return
      return;
   }

   // Special stop state
   if ((!right && !left) && !BOUNDED(vel_x) && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG]) && player_state_ != PUSH) {
      // Set state
      player_state_ = STOP;

      // Return
      return;
   }

   // Special fall state, TODO: add falling animation
   // if ((right || left) && BOUNDED(vel_x) && !(contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
   //    // Set state
   //    player_state_ = JUMP;

   //    // Return
   //    return;
   // }

   // Check for non-zero y-vel
   if (!(contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      // Run and jump or just jump
      if (!BOUNDED(vel_x)) {
         player_state_ = RUN_AND_JUMP;
      } else {
         fall_timer_.Start();
      }
   } else if (!BOUNDED(vel_x)) {
      player_state_ = RUN;
   } else {
      player_state_ = STAND;
   }
}

#endif

// Movement logic of the player. Done through keyboard.
void Player::Move() {
   // Body pointer
   b2Body *body = GetBody();

   // Current anim pointer
   shared_ptr<StateInterface> player_state_ = GetStateContext()->GetCurrentState();

   // Check if shooting
   if (shooting) {
      if (GetAnimationByName("arm_throw")->curr_frame > 0) {
         num_of_projectiles++;
         if (num_of_projectiles == 1) {
            //CreateProjectile("player_projectile", 21.0f, 12.0f, 38, -12, 41, 1, 10, 0.0f, 0.0f);
         }
      } else {
         num_of_projectiles = 0;
      }
   }

   // Player running left
   if (KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) {
      // Check for midair
      if (player_state_ == RUN || player_state_ == STAND || player_state_ == BALANCE) {
         b2Vec2 vel = {-5.5f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      } else if (player_state_ == JUMP || player_state_ == RUN_AND_JUMP || player_state_ == DOUBLE_JUMP) {
         b2Vec2 vel = {body->GetLinearVelocity().x, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
         if (body->GetLinearVelocity().x > -4.0f) {
            const b2Vec2 force = {-5.4f, 0};
            body->ApplyForce(force, body->GetPosition(), true);
         }
      }
   } else if (KH.GetKeyReleased(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) { // Handle release of left key
      b2Vec2 vel = {0.0f, body->GetLinearVelocity().y};
      body->SetLinearVelocity(vel);
   }

   // Deal with basic movement for now
   if (KH.GetKeyPressed(KEY_RIGHT) && !KH.GetKeyPressed(KEY_LEFT)) {
      // Set to jump and run if not on the ground
      if (player_state_ == RUN || player_state_ == STAND || player_state_ == BALANCE) {
         b2Vec2 vel = {5.5f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      } else if (player_state_ == JUMP || player_state_ == RUN_AND_JUMP || player_state_ == DOUBLE_JUMP) {
         b2Vec2 vel = {body->GetLinearVelocity().x, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
         if (body->GetLinearVelocity().x < 4.0f) {
            const b2Vec2 force = {5.4f, 0};
            body->ApplyForce(force, body->GetPosition(), true);
         }
      }
   } else if (KH.GetKeyReleased(KEY_RIGHT) && !KH.GetKeyPressed(KEY_LEFT)) {
      b2Vec2 vel = {0.0f, body->GetLinearVelocity().y};
      body->SetLinearVelocity(vel);
   }

   // Player jumping
   if (has_jumped_ < 2) {
      if (KH.GetKeyPressedOnce(KEY_UP)) {
         if (has_jumped_ < 1) {
            // Apply an impulse
            const b2Vec2 force = {0, 2.8f};
            body->ApplyLinearImpulse(force, body->GetPosition(), true);

            // Force set contacts to null
            contacts_[LEFT_LEG] = 0;
            contacts_[RIGHT_LEG] = 0;

            // Set the flags
            ++has_jumped_;
         } else {
            // Set y velocity to 0
            b2Vec2 vel = {body->GetLinearVelocity().x, 0.0f};
            body->SetLinearVelocity(vel);

            // Apply an impulse
            const b2Vec2 force = {0, 2.4f};
            body->ApplyLinearImpulse(force, body->GetPosition(), true);

            // Force set contacts to null
            contacts_[LEFT_LEG] = 0;
            contacts_[RIGHT_LEG] = 0;

            // Set the flags
            ++has_jumped_;
         }
      }
   } 
}

// Start contact function
void Player::StartContact(Element *element) {
   if (element && (element->type() == "Projectile" || (element->is_enemy()))) {
      TakeDamage(10);
   }
}

// End contact function (does nothing for now)
void Player::EndContact(Element *element) {
   return;
}

// Load media function
void Player::LoadMedia() {
   // Register as correspondent
   PigeonPost::GetInstance().Register(GetName(), getptr());
   string p_path = "Media/Sprites/Player/player_master_sheet.png";
   Texture *temp = RegisterTexture(p_path);

   // Register animations with state GetContext()
   GetStateContext()->RegisterState("stand", make_shared<Player_Stand>(GetStateContext().get(), temp, make_shared<Animation>(temp, "tap", 61.f, 106.f, 212.f, 12, 1.f / 24.f),
                                                                                                      make_shared<Animation>(temp, "look", 61.f, 106.f, 848.f, 20, 1.f / 24.f),
                                                                                                      make_shared<Animation>(temp, "kick", 61.f, 106.f, 742.f, 17, 1.0f / 24.f)));
   GetStateContext()->RegisterState("running", make_shared<Player_Run>(GetStateContext().get(), temp, make_shared<Animation>(temp, "running", 61.f, 106.f, 318.f, 15, 1.f / 30.f)));
   GetStateContext()->RegisterState("jump", make_shared<Player_Jump>(GetStateContext().get(), temp, make_shared<Animation>(temp, "jump", 61.f, 106.f, 424.f, 15, 1.f / 24.f)));
   GetStateContext()->RegisterState("double_jump", make_shared<Player_DoubleJump>(GetStateContext().get(), temp, make_shared<Animation>(temp, "double_jump", 61.f, 106.f, 106.f, 11, 1.f / 24.f)));
   GetStateContext()->RegisterState("push", make_shared<Player_Push>(GetStateContext().get(), temp, make_shared<Animation>(temp, "push", 61.f, 106.f, 636.f, 16, 1.f / 20.f)));
   GetStateContext()->RegisterState("running_jump", make_shared<Player_RunningJump>(GetStateContext().get(), temp, make_shared<Animation>(temp, "running_jump", 61.f, 106.f, 530.f, 15, 1.f / 24.f)));
   GetStateContext()->RegisterState("jump_push", make_shared<Player_JumpPush>(GetStateContext().get(), temp, make_shared<Animation>(temp, "jump_push", 61.f, 106.f, 0.f, 8, 1.f / 20.f)));
   GetStateContext()->RegisterState("balance", make_shared<Player_Balance>(GetStateContext().get(), temp, make_shared<Animation>(temp, "balance", 128.f, 106.f, 1060.f, 19, 1.f / 20.f)));
   GetStateContext()->RegisterState("death", make_shared<Player_Death>(GetStateContext().get(), temp, make_shared<Animation>(temp, "death", 107.f, 106.f, 954.f, 20, 1.f / 20.f)));

   // Instantiate sprite sheet for arms
   string arm_path = "Media/Sprites/Player/arm_master_sheet.png";
   temp = RegisterTexture(arm_path);
   arm_.GetStateContext()->RegisterState("idle", make_shared<Arm_Idle>(GetStateContext().get(), temp, make_shared<Animation>(temp, "idle", 10.f, 27.f, 0.f, 1, 1.f / 30.f)));
   arm_.GetStateContext()->RegisterState("running", make_shared<Arm_Running>(GetStateContext().get(), temp, make_shared<Animation>(temp, "running", 9.f, 27.f, 54.f, 15, 1.f / 30.f)));
   arm_.GetStateContext()->RegisterState("double_jump", make_shared<Arm_DoubleJump>(GetStateContext().get(), temp, make_shared<Animation>(temp, "double_jump", 9.f, 27.f, 0.f, 8, 1.f / 24.f)));
   arm_.GetStateContext()->RegisterState("shooting", make_shared<Arm_Shooting>(GetStateContext().get(), temp, make_shared<Animation>(temp, "shooting", 44.f, 33.f, 81.f, 9, 1.f / 20.f)));

   // Set initial and reset
   GetStateContext()->SetResetState(GetStateContext()->GetState("stand"));
   GetStateContext()->SetInitialState(GetStateContext()->GetState("stand"));
}

// Create projectile
Projectile* Player::CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y,
     bool owner, bool damage, float force_x, float force_y) {

   // // First, create a new projectile
   // Projectile *proj;

   // // Create based on direction
   // if (GetDirection() == RIGHT) {
   //    proj = new Projectile(name, GetPosition().x + GetSize().x + delta_x_r, GetPosition().y + delta_y, 
   //          width, height, 1, 10, 10.4f, 0.0f, this);
   // } else {
   //    proj = new Projectile(name, GetPosition().x + delta_x_l, GetPosition().y + delta_y,
   //          width, height, 1, 10, 10.4f, 0.0f, this);
   // }

   // // Set shot direction
   // proj->shot_dir = GetDirection();

   // // Return projectile reference
   // return proj;
}

// Take damage function
void Player::TakeDamage(int damage) {
   // Now check that a certain threshold has been reached
   float delta = (float) immunity_timer_.GetDeltaTime() / 1000.0f;
   if (delta > immunity_duration_) {
      std::vector<int> recs;
      int myid = ObjectManager::GetInstance().GetUID("player");
      int id = ObjectManager::GetInstance().GetUID("PlayerLife" + std::to_string(GetHealth() / 10));
      if (id != -1) {
         recs.push_back(id);
         Correspondence cor = Correspondence::CompileCorrespondence((void *) "LifeLost", "string", recs, myid);
         PigeonPost::GetInstance().Send(cor);
      }
      SetHealth(GetHealth() - damage);
      if (GetHealth() == 0) {
         GetStateContext()->SetState(GetStateContext()->GetState("death"));
         Application::GetInstance().death_timer_.Start();
      }
   }
}

// Virtual destructor
Player::~Player() {}