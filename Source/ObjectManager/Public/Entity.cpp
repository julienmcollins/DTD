#include "Source/ObjectManager/Private/Entity.h"
#include "Source/ObjectManager/Private/Object.h"
#include "Source/ObjectManager/Private/Timer.h"

#include "Source/RenderingEngine/Private/RenderingEngine.h"
#include "Source/RenderingEngine/Private/Texture.h"

#include "Source/GameEngine/Private/Application.h"

#include <cmath>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <Box2D/Box2D.h>

#define BOUNDED(var) (var > -0.0000001f && var < 0.0000001f)

const std::string Player::media_path = Application::sprite_path + "Player/";

/*************************** ENTITY IMPLEMENTATIONS ******************************/

// Entity constructor which will provide basic establishment for all entities
Entity::Entity(int x_pos, int y_pos, double width, double height) : 
   Element(x_pos, y_pos, width, height),
   has_jumped_(0), health(0), shift_(false), entity_direction(NEUTRAL),
   prev_entity_dir(NEUTRAL) {
}

// Update function for all entities. For now all it does is call move
void Entity::Update(bool freeze) {
   Move();
}

// Get direction
int Entity::get_dir() const {
   return entity_direction;
}

// Create projectile
Projectile* Entity::CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y,
     bool owner, bool damage, float force_x, float force_y) {
   
   // First, create a new projectile
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new Projectile("none", get_tex_x() + get_width() + delta_x_r, get_tex_y() + delta_y, 
            0.0f, 0.0f, owner, damage, force_x, force_y, this);
   } else {
      proj = new Projectile("none", get_tex_x() + delta_x_l, get_tex_y() + delta_y, owner,
            0.0f, 0.0f, damage, force_x, force_y, this);
   }

   // Set shot direction
   proj->shot_dir = entity_direction;

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
   Sensor::initialize(width, height, x, y, CAT_PLAYER, true);
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
   Sensor::initialize(width / 200.0f, height / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER, true);
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
   Sensor::initialize(get_width() / 200.0f, get_height() / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER, true);
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
   Sensor::initialize(width / 200.0f, height / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER, true);
}

void PlayerLeg::StartContact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element && player) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftLeg") {
            player->contacts_[Player::LEFT_LEG] = 1;
            // std::cout << "PlayerLeg::StartContact() - in contact left leg: " << element->type() << std::endl;
         } else {
            player->contacts_[Player::RIGHT_LEG] = 1;
            // std::cout << "PlayerLeg::StartContact() - in contact right leg: " << element->type() << std::endl;
         }
         owner_->has_jumped_ = 0;
         owner_->GetAnimationByName("jump")->reset_frame = 0;
         owner_->GetAnimationByName("jump")->curr_frame = 0;
         owner_->GetAnimationByName("double_jump")->reset_frame = 0;
         owner_->GetAnimationByName("double_jump")->curr_frame = 0;
         owner_->GetAnimationByName("running_jump")->reset_frame = 0;
         owner_->GetAnimationByName("running_jump")->curr_frame = 0;
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
Player::Player() : 
   // The new sprite is going to be 37 wide (the character itself)
   // TODO: Load in new smaller sprite sheet
   Entity(960, 412, 37, 104), player_state_(STAND),
   shooting(false), arm_delta_x(12), arm_delta_y(64),
   arm_delta_shoot_x(12), arm_delta_shoot_y(51), prev_pos_x_(0.0f), prev_pos_y_(0.0f),
   immunity_duration_(0.5f), key(NONE), last_key_pressed(NONE), lock_dir_left(false),
   lock_dir_right(false), lock_dir_up(false), rand_idle(0), eraser(nullptr), num_of_projectiles(0) {

   // Set entity direction
   entity_direction = RIGHT;

   // Setup Box2D
   // Set body type
   body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x = 600.0f * Application::GetInstance().to_meters_;
   float y = -412.5f * Application::GetInstance().to_meters_;
   body_def.position.Set(x, y);
   body_def.fixedRotation = true;

   // Set transform
   element_model = glm::translate(element_model, glm::vec3(x, y, 0.0f));

   // Attach body to world
   body = Application::GetInstance().world_.CreateBody(&body_def);

   // Set box dimensions
   float width = (get_width() / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;// - 0.11f;
   float height = (get_height() / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;// - 0.11f;
   const b2Vec2 center = {(PC_OFF_X - get_width()) / 2.0f * Application::GetInstance().to_meters_, 
                          PC_OFF_Y * Application::GetInstance().to_meters_};
   box.SetAsBox(width, height, {0.0f, 0.0f}, 0.0f);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 0.0f;
   fixture_def.userData = this;
   main_fixture = body->CreateFixture(&fixture_def);

   // Set sensors to non interactive with da enemy
   b2Filter filter;
   filter.categoryBits = CAT_PLAYER;
   filter.maskBits = CAT_PLATFORM | CAT_PROJECTILE | CAT_ENEMY | CAT_BOSS;
   b2Fixture *fixture_list = body->GetFixtureList();
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
   player_body_left_.push_back(new PlayerLeg(this, -10.5, -33, 3, 34, "PlayerLeftLeg"));
   player_body_left_.push_back(new PlayerLeg(this, 10.5, -33, 3, 34, "PlayerRightLeg"));

   // Deactivate the left
   // for (int i = 0; i < player_body_left_.size(); i++) {
   //    player_body_left_[i]->Sensor::deactivate_sensor();
   // }

   // Set health. TODO: set health in a better way
   health = 30;

   // TEMPORARY SOLUTION
   //textures["arm_throw"]->completed_ = true;

   // Create hitmarkers
   for (int i = 0; i < 3; i++) {
      hit_markers.push_back(new Hitmarker(76 * i, 0));
   }

   /************* PLAYER *******************************************/
   // Set in contact = false
   for (int i = 0; i < 5; i++) {
      contacts_[i] = 0;
   }

   // Start the immunity timer
   immunity_timer_.start();
}

Animation *Player::GetAnimationFromState() {
   // Return run or stop texture (aka running texture)
   if (player_state_ == RUN || player_state_ == STOP) {
      return GetAnimationByName("running");
   }

   // Return jump texture
   if (player_state_ == JUMP) {
      return GetAnimationByName("jump");
   }

   // Return run and jump texture
   if (player_state_ == RUN_AND_JUMP) {
      return GetAnimationByName("running_jump");
   }

   // Return double jump
   if (player_state_ == DOUBLE_JUMP) {
      return GetAnimationByName("double_jump");
   }

   // Return push texture
   if (player_state_ == PUSH) {
      return GetAnimationByName("push");
   }

   // Return jump and push texture
   if (player_state_ == JUMP_AND_PUSH) {
      return GetAnimationByName("jump_push");
   }

   // Return balance
   if (player_state_ == BALANCE) {
      return GetAnimationByName("balance");
   }

   // Death
   if (player_state_ == DEATH) {
      return GetAnimationByName("death");
   }

   // Check which idle is being used and return it
   if (rand_idle <= 98) {
      return GetAnimationByName("tap");
   }
   if (rand_idle == 99) {
      return GetAnimationByName("look");
   }
   if (rand_idle == 100) {
      return GetAnimationByName("kick");
   }
}

// Get player state
Player::STATE Player::get_player_state() {
   return player_state_;
}

// Process keyboard input
void Player::ProcessInput(const Uint8 *key_state) {  
   // Set previous direction
   prev_entity_dir = entity_direction;

   // Set key to none by default
   key = NONE;

   // Process left key
   if (key_state[SDL_SCANCODE_LEFT]) {
      if (!lock_dir_right) {
         key = KEY_LEFT;
         entity_direction = LEFT;
         lock_dir_left = true;
      }
   } else {
      lock_dir_left = false;
   }

   // Process right key
   if (key_state[SDL_SCANCODE_RIGHT]) {
      if (!lock_dir_left) {
         key = KEY_RIGHT;
         entity_direction = RIGHT;
         lock_dir_right = true;
      }
   } else {
      lock_dir_right = false;
   }

   // Process up key
   if (key_state[SDL_SCANCODE_UP]) {
      if (!lock_dir_up) {
         key = KEY_UP;
         lock_dir_up = true;
         //std::cout << "LOCK JUMP\n";
      }
   } else {
      //std::cout << "UNLOCK JUMP\n";
      lock_dir_up = false;
   }

   // Process down key
   if (key_state[SDL_SCANCODE_DOWN]) {
      key = KEY_DOWN;
   }

   // Process space key
   if (key_state[SDL_SCANCODE_SPACE]) {
      shooting = true;
   } else {
      if (GetAnimationByName("arm_throw")->completed) {
         shooting = false;
      }
   }
}

// Update function
void Player::Update(bool freeze) {
   // Apply artificial force of gravity
   const b2Vec2 sim_grav = {0.0f, SIM_GRAV};
   body->ApplyForceToCenter(sim_grav, true);

   // Process key inputs
   ProcessInput(Application::GetInstance().current_key_states_);

   // Animate the function
   Animate();

   // Update player if not frozen
   if (!freeze) {
      Move();
   }

   // Adjust deltas first
   adjust_deltas();

   // Render arm if idle, Render shooting if not
   if (player_state_ != PUSH && player_state_ != JUMP_AND_PUSH && player_state_ != BALANCE && player_state_ != DEATH) {
      if (!shooting) {
         std::string arm_type;
         if (get_player_state() == RUN) {
            arm_type = "running_arm";
         } else if (get_player_state() == DOUBLE_JUMP) {
            arm_type = "double_jump_arm";
         } else {
            arm_type = "idle_arm";
         }
         arm_sheet->Render(get_anim_x() + get_width() + arm_delta_x, 
            get_anim_y() + arm_delta_y, 0.0f, GetAnimationByName(arm_type));
      } else {
         arm_sheet->Render(get_anim_x() + get_width() + arm_delta_shoot_x,
            get_anim_y() + arm_delta_shoot_y, 0.0f, GetAnimationByName("arm_throw"));
      }
   }

   // Render player
   sprite_sheet->Render(get_anim_x(), get_anim_y(), 0.0f, GetAnimationFromState());
}

// Adjust delta function
void Player::adjust_deltas() {
   if (player_state_ == STAND) {
      if (entity_direction == RIGHT) {
         arm_delta_x = -4;
         arm_delta_y = 44;
         arm_delta_shoot_x = -3;
         arm_delta_shoot_y = 39;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 44;
         arm_delta_shoot_x = -57;
         arm_delta_shoot_y = 39;
      }
   } else if (player_state_ == RUN && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      if (entity_direction == RIGHT) {
         arm_delta_x = -4;
         arm_delta_y = 44;
         arm_delta_shoot_x = 2;
         arm_delta_shoot_y = 39;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 44;
         arm_delta_shoot_x = -58;
         arm_delta_shoot_y = 39;
      }
   } else if (player_state_ == STOP && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
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
   } else if (player_state_ == RUN_AND_JUMP || player_state_ == JUMP || player_state_ == DOUBLE_JUMP) {
      // Adjust deltas
      if (entity_direction == RIGHT) {
         arm_delta_x = -3;
         arm_delta_y = 43;
         arm_delta_shoot_x = -2;
         arm_delta_shoot_y = 34;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 43;
         arm_delta_shoot_x = -57;
         arm_delta_shoot_y = 34;
      }
   }
}

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

   // Might be a hack, but essentially only let it switch if double jump is completed
   if (has_jumped_ == 2) {
      player_state_ = DOUBLE_JUMP;
      return;
   }

   // Change state to balance if one leg on and the other one not
   if ((contacts_[LEFT_LEG] && !contacts_[RIGHT_LEG]) || (!contacts_[LEFT_LEG] && contacts_[RIGHT_LEG])) {
      player_state_ = BALANCE;
      return;
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
   if ((right || left) && BOUNDED(vel_x) && !(contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      // Set state
      player_state_ = JUMP;

      // Return
      return;
   }

   // Check for non-zero y-vel
   if (!(contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      // Run and jump or just jump
      if (!BOUNDED(vel_x)) {
         player_state_ = RUN_AND_JUMP;
      } else {
         player_state_ = JUMP;
      }
   } else if (!BOUNDED(vel_x)) {
      player_state_ = RUN;
   } else {
      player_state_ = STAND;
   }
}

// Movement logic of the player. Done through keyboard.
void Player::Move() {
   // Death 
   if (player_state_ == DEATH) {
      // TODO: FIX DEATH CLIPPING THROUGH PLATFORM
      if (!shift_) {
         body->SetLinearVelocity({0.0f, body->GetLinearVelocity().y});
         shift_ = true;
      }
      if (GetAnimationByName("death")->curr_frame >= 19) {
         if (((float) Application::GetInstance().death_timer_.getTicks() / 1000.0f) >= 3.0f) {
            std::cout << "In death\n";
            alive = false;
            Application::GetInstance().death_timer_.stop();
         }
         // GetAnimationByName("death")->reset_frame = 19;
         // GetAnimationByName("death")->stop_frame = 19;
      }

      // Set collision to only platform
      SetCollision(CAT_PLATFORM, main_fixture);
      return;
   }

   // Deactivate and activate bodies
   // if (entity_direction == LEFT && !right_deactivated_) {
   //    for (int i = 0; i < player_body_left_.size(); i++) {
   //       player_body_right_[i]->Sensor::deactivate_sensor();
   //       player_body_left_[i]->Sensor::activate_sensor();
   //    }
   //    right_deactivated_ = true;
   //    left_deactivated_ = false;
   // } else if (entity_direction == RIGHT && !left_deactivated_) {
   //    for (int i = 0; i < player_body_right_.size(); i++) {
   //       player_body_left_[i]->Sensor::deactivate_sensor();
   //       player_body_right_[i]->Sensor::activate_sensor();
   //    }
   //    left_deactivated_ = true;
   //    right_deactivated_ = false;
   // }

   // Check if shooting
   if (shooting) {
      if (GetAnimationByName("arm_throw")->curr_frame > 0) {
         num_of_projectiles++;
         if (num_of_projectiles == 1) {
            CreateProjectile("player_projectile", 21.0f, 12.0f, 38, -12, 41, 1, 10, 0.0f, 0.0f);
         }
      } else {
         num_of_projectiles = 0;
      }
   }

   // Reset frames
   if (player_state_ != DOUBLE_JUMP) {
      GetAnimationByName("double_jump_arm")->reset_frame = 0;
   } else {
      GetAnimationByName("double_jump_arm")->reset_frame = GetAnimationByName("double_jump_arm")->max_frame;
   }

   // Check for changing directions on second jump
   if (key == KEY_RIGHT && prev_entity_dir == LEFT) {    
      if (has_jumped_ > 0) {
         b2Vec2 vel = {0.0f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      }
   }
   if (key == KEY_LEFT && prev_entity_dir == RIGHT) {
      if (has_jumped_ > 0) {
         b2Vec2 vel = {0.0f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      }
   }

   // If not running or running and jumping, then set linear velocity to 0
   if (player_state_ != RUN) {
      // Reset running arm
      GetAnimationByName("running_arm")->curr_frame = GetAnimationByName("running")->curr_frame;
      if (player_state_ != RUN_AND_JUMP && player_state_ != JUMP && player_state_ != PUSH && player_state_ != JUMP_AND_PUSH && player_state_ != DOUBLE_JUMP) {
         b2Vec2 vel = {0, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
         //player_state_ = STOP;
      }
   }

   // Player running left
   if (key == KEY_LEFT) {
      // TESTING
      contacts_[RIGHT_ARM] = 0;

      // Check for flag
      if (!TextureFlipped()) {
         FlipAllAnimations();
         texture_flipped = true;
      }

      // Check for midair
      if (player_state_ == RUN || player_state_ == STAND || player_state_ == BALANCE) {
         //player_state_ = RUN;
         b2Vec2 vel = {-5.5f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      } else if (player_state_ == JUMP || player_state_ == RUN_AND_JUMP || player_state_ == DOUBLE_JUMP) {
         b2Vec2 vel = {body->GetLinearVelocity().x, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
         if (body->GetLinearVelocity().x > -4.0f) {
            const b2Vec2 force = {-5.4f, 0};
            body->ApplyForce(force, body->GetPosition(), true);
         }
      } else if ((player_state_ == JUMP_AND_PUSH || player_state_ == PUSH) && entity_direction == RIGHT) {
         contacts_[RIGHT_ARM] = 0;
      }
   } 

   // Deal with basic movement for now
   if (key == KEY_RIGHT) {
      // TESTING
      contacts_[LEFT_ARM] = 0;
      
      // Check for flag
      if (TextureFlipped()) {
         FlipAllAnimations();
         texture_flipped = false;
      }

      // Set to jump and run if not on the ground
      if (player_state_ == RUN || player_state_ == STAND || player_state_ == BALANCE) {
         //player_state_ = RUN;
         b2Vec2 vel = {5.5f, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
      } else if (player_state_ == JUMP || player_state_ == RUN_AND_JUMP || player_state_ == DOUBLE_JUMP) {
         b2Vec2 vel = {body->GetLinearVelocity().x, body->GetLinearVelocity().y};
         body->SetLinearVelocity(vel);
         if (body->GetLinearVelocity().x < 4.0f) {
            const b2Vec2 force = {5.4f, 0};
            body->ApplyForce(force, body->GetPosition(), true);
         }
      } else if ((player_state_ == JUMP_AND_PUSH || player_state_ == PUSH) && entity_direction == LEFT) {
         contacts_[LEFT_ARM] = 0;
      }
   }

   // Player jumping
   if (key == KEY_UP) {
      if (has_jumped_ < 2) {
         if (has_jumped_ == 0) {
            if (player_state_ == RUN) {
               GetAnimationByName("running_jump")->reset_frame = 14;
               GetAnimationByName("running_jump")->stop_frame = 14;
            } else {
               GetAnimationByName("jump")->reset_frame = GetAnimationByName("jump")->max_frame;
               GetAnimationByName("jump")->stop_frame = GetAnimationByName("jump")->max_frame;
            }

            // Apply an impulse
            const b2Vec2 force = {0, 2.8f};
            body->ApplyLinearImpulse(force, body->GetPosition(), true);

            // Set the flags
            ++has_jumped_;
         } else {
            if (player_state_ == RUN) {
               GetAnimationByName("running_jump")->reset_frame = 14;
               GetAnimationByName("running_jump")->stop_frame = 14;
            } else {
               GetAnimationByName("double_jump")->reset_frame = GetAnimationByName("double_jump")->max_frame;
               GetAnimationByName("double_jump")->stop_frame = GetAnimationByName("double_jump")->max_frame;
            }

            // Set y velocity to 0
            b2Vec2 vel = {body->GetLinearVelocity().x, 0.0f};
            body->SetLinearVelocity(vel);

            //std::cout << body->GetLinearVelocity().y << std::endl;

            // Apply an impulse
            const b2Vec2 force = {0, 2.4f};
            body->ApplyLinearImpulse(force, body->GetPosition(), true);

            // Set the flags
            ++has_jumped_;
         }
      }
   } 

   // Update player state
   change_player_state();

   // Set previous position
   prev_pos_x_ = body->GetPosition().x; 
   prev_pos_y_ = body->GetPosition().y;
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
bool Player::LoadMedia() {
   // Temp flag
   bool success = true;

   // Instantiate sprite sheet for main player body
   std::string player_path = media_path + "player_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("player_master_sheet", player_path.c_str());
   animations.emplace("jump_push", new Animation(sprite_sheet, "jump_push", 61.0, 106.0, 0.0, 8, 1.0 / 20.0));
   animations.emplace("double_jump", new Animation(sprite_sheet, "double_jump", 61.0, 106.0, 106.0, 11, 1.0 / 24.0));
   animations.emplace("tap", new Animation(sprite_sheet, "tap", 61.0, 106.0, 212.0, 12, 1.0 / 24.0));
   animations.emplace("running", new Animation(sprite_sheet, "running", 61.0, 106.0, 318.0, 15, 1.0 / 30.0));
   animations.emplace("jump", new Animation(sprite_sheet, "jump", 61.0, 106.0, 424.0, 15, 1.0 / 24.0));
   animations.emplace("running_jump", new Animation(sprite_sheet, "running_jump", 61.0, 106.0, 530.0, 15, 1.0 / 24.0));
   animations.emplace("push", new Animation(sprite_sheet, "push", 61.0, 106.0, 636.0, 16, 1.0 / 20.0));
   animations.emplace("kick", new Animation(sprite_sheet, "kick", 61.0, 106.0, 742.0, 17, 1.0 / 24.0));
   animations.emplace("look", new Animation(sprite_sheet, "look", 61.0, 106.0, 848.0, 20, 1.0 / 24.0));
   animations.emplace("death", new Animation(sprite_sheet, "death", 107.0, 106.0, 954.0, 20, 1.0 / 20.0));
   animations.emplace("balance", new Animation(sprite_sheet, "balance", 128.0, 106.0, 1060.0, 19, 1.0 / 20.0));

   // Instantiate sprite sheet for arms
   std::string arm_path = media_path + "arm_master_sheet.png";
   arm_sheet = RenderingEngine::GetInstance().LoadTexture("arm_master_sheet", arm_path.c_str());
   animations.emplace("idle_arm", new Animation(arm_sheet, "idle_arm", 10.0, 27.0, 0.0, 1, 1.0 / 30.0));
   animations.emplace("double_jump_arm", new Animation(arm_sheet, "double_jump_arm", 9.0, 27.0, 27.0, 8, 1.0 / 24.0));
   animations.emplace("running_arm", new Animation(arm_sheet, "running_arm", 9.0, 27.0, 54.0, 15, 1.0 / 30.0));
   animations.emplace("arm_throw", new Animation(arm_sheet, "arm_throw", 44.0, 33.0, 81.0, 9, 1.0 / 20.0));
   RenderingEngine::GetInstance().LoadResources(this);

   // Set current idle texture to tap
   curr_idle_animation = GetAnimationByName("tap");

   // Return success
   return success;
}

// Create projectile
Projectile* Player::CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y,
     bool owner, bool damage, float force_x, float force_y) {

   // First, create a new projectile
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new Projectile(name, get_tex_x() + get_width() + delta_x_r, get_tex_y() + delta_y, 
            width, height, 1, 10, 15.4f, 0.0f, this);
   } else {
      proj = new Projectile(name, get_tex_x() + delta_x_l, get_tex_y() + delta_y,
            width, height, 1, 10, 15.4f, 0.0f, this);
   }

   // Set shot direction
   proj->shot_dir = entity_direction;

   // Return projectile reference
   return proj;
}

// Take damage function
void Player::TakeDamage(int damage) {
   // Now check that a certain threshold has been reached
   float delta = (float) immunity_timer_.getDeltaTime() / 1000.0f;
   if (delta > immunity_duration_) {
      // Take damage
      if (health == 30) {
         hit_markers[0]->state = Hitmarker::DEAD;
      } else if (health == 20) {
         hit_markers[1]->state = Hitmarker::DEAD;
      } else if (health == 10) {
         hit_markers[2]->state = Hitmarker::DEAD;
      }
      health -= damage;
      if (health == 0) {
         player_state_ = DEATH;
         Application::GetInstance().death_timer_.start();
      }
   }
}

// Virtual destructor
Player::~Player() {
   // Delete hit markers
   for (int i = 0; i < hit_markers.size(); i++) {
      delete hit_markers[i];
   }
}

///////////////////////////////////////////////////////
/////////////// HITMARKER CLASS ///////////////////////
///////////////////////////////////////////////////////
Hitmarker::Hitmarker(int x, int y) :
   Element(x, y, 103, 76), state(ALIVE) {
   
   // Load media
   LoadMedia();
}

// Load media function
bool Hitmarker::LoadMedia() {
   bool success = true;

   std::string hitmarker = Application::GetInstance().sprite_path + "Player/hitmarker_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("hitmarker_master_sheet", hitmarker.c_str());
   animations.emplace("alive", new Animation(sprite_sheet, "alive", 76.0f, 103.0f, 0.0f, 21, 1.0f / 20.0f));
   animations.emplace("dead", new Animation(sprite_sheet, "dead", 76.0f, 103.0f, 103.0f, 19, 1.0f / 20.0f));
   RenderingEngine::GetInstance().LoadResources(this);

   // Return success
   return success;
}

// Animate
void Hitmarker::Animate() {
   if (state == ALIVE) {
      sprite_sheet->Animate(GetAnimationFromState());
   } else if (state == DEAD) {
      sprite_sheet->Animate(GetAnimationFromState(), 18);
   }
}

// Get texture
Animation *Hitmarker::GetAnimationFromState() {
   if (state == ALIVE) {
      return GetAnimationByName("alive");
   }
   if (state == DEAD) {
      return GetAnimationByName("dead");
   }
}

// Update function
void Hitmarker::Update(bool freeze) {
   // Animate and Render
   Animate();
   sprite_sheet->Render(get_anim_x(), get_anim_y(), 0.0f, GetAnimationFromState());
}