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
Entity::Entity(int x_pos, int y_pos, double height, double width) : 
   Element(x_pos, y_pos, height, width),
   has_jumped_(0), health(0), shift_(false), entity_direction(NEUTRAL),
   prev_entity_dir(NEUTRAL) {
}

// Update function for all entities. For now all it does is call move
void Entity::update(bool freeze) {
   move();
}

// Get direction
int Entity::get_dir() const {
   return entity_direction;
}

// Create projectile
Projectile* Entity::CreateProjectile(int delta_x_r, int delta_x_l, int delta_y,
     bool owner, bool damage, 
     float force_x, float force_y,
     const TextureData &normal, const TextureData &hit) {
   // First, create a new projectile
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new Projectile(get_tex_x() + get_width() + delta_x_r, get_tex_y() + delta_y, 
            owner, damage, force_x, force_y, normal, hit, this);
   } else {
      proj = new Projectile(get_tex_x() + delta_x_l, get_tex_y() + delta_y, owner,
            damage, force_x, force_y, normal, hit, this);
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
   Sensor::initialize(width, height, x, y, CAT_PLAYER);
}

void PlayerHead::start_contact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         player->contacts_[Player::HEAD] = 1;
      } else if (element->type() == "EnemyProjectile" || !element->is_enemy()) {
         player->take_damage(10);
      }
   }
}

void PlayerHead::end_contact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         player->contacts_[Player::HEAD] = 0;
      }
   }
}

/********* ARM ****************/

PlayerArm::PlayerArm(Player *player, float x_rel, float y_rel, int width, int height, std::string type) :
   BodyPart(player, x_rel, y_rel, width, height), type_(type) {

   // Initialize the body part
   Sensor::initialize(width / 200.0f, height / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER);
}

void PlayerArm::start_contact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftArm") {
            player->contacts_[Player::LEFT_ARM] = 1;
            //std::cout << "PlayerArm::start_contact() - left arm in contact\n";
         } else {
            player->contacts_[Player::RIGHT_ARM] = 1;
            //std::cout << "PlayerArm::start_contact() - right arm in contact\n";
         }
      } else if (element->type() == "EnemyProjectile" || !element->is_enemy()) {
         player->take_damage(10);
      }
   }
}

void PlayerArm::end_contact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftArm") {
            player->contacts_[Player::LEFT_ARM] = 0;
            //std::cout << "PlayerArm::start_contact() - lost contact left arm\n";
         } else {
            player->contacts_[Player::RIGHT_ARM] = 0;
            //std::cout << "PlayerArm::start_contact() - lost contact right arm\n";
         }
      }
   }
}

/********* HAND ****************/

PlayerHand::PlayerHand(Player *player, float x_rel, float y_rel, std::string type) :
   BodyPart(player, x_rel, y_rel, 7, 6), type_(type) {

   // Initialize the body part
   Sensor::initialize(get_width() / 200.0f, get_height() / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER);
}

void PlayerHand::start_contact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftHand") {
            player->contacts_[Player::LEFT_HAND] = 1;
            std::cout << "PlayerHand::start_contact() - in contact with left hand\n";
         } else {
            player->contacts_[Player::RIGHT_HAND] = 1;
         }
      } else if (element->type() == "EnemyProjectile" || !element->is_enemy()) {
         player->take_damage(10);
      }
   }
}

void PlayerHand::end_contact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftHand") {
            player->contacts_[Player::LEFT_HAND] = 0;
            std::cout << "PlayerHand::end_contact() - lost contact with left hand\n";
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
   Sensor::initialize(width / 200.0f, height / 200.0f, x_rel / 100.0f, y_rel / 100.0f, CAT_PLAYER);
}

void PlayerLeg::start_contact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftLeg") {
            player->contacts_[Player::LEFT_LEG] = 1;
            //std::cout << "PlayerLeg::start_contact() - in contact left leg\n";
         } else {
            player->contacts_[Player::RIGHT_LEG] = 1;
            //std::cout << "PlayerLeg::start_contact() - in contact right leg\n";
         }
         owner_->has_jumped_ = 0;
         owner_->textures["jump"]->reset_frame = 0;
         owner_->textures["jump"]->frame_ = 0;
         owner_->textures["double_jump"]->reset_frame = 0;
         owner_->textures["double_jump"]->frame_ = 0;
         owner_->textures["running_jump"]->reset_frame = 0;
         owner_->textures["running_jump"]->frame_ = 0;
      }
   } else if (element->type() == "EnemyProjectile" || !element->is_enemy()) {
      player->take_damage(10);
   }
}

void PlayerLeg::end_contact(Element *element) {
   // Dynamic cast owner to player
   Player *player = dynamic_cast<Player *>(owner_);

   // Set contacts of player
   if (element) {
      if (element->type() == "Platform" || element->type() == "Mosqueenbler" || element->type() == "Wormored") {
         if (sub_type() == "PlayerLeftLeg") {
            player->contacts_[Player::LEFT_LEG] = 0;
            //std::cout << "PlayerLeg::end_contact() - lost contact left leg\n";
         } else {
            player->contacts_[Player::RIGHT_LEG] = 0;
            //std::cout << "PlayerLeg::end_contact() - lost contact right leg\n";
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
   Entity(960, 412, 104, 37), player_state_(STAND),
   shooting(false), arm_delta_x(12), arm_delta_y(64),
   arm_delta_shoot_x(12), arm_delta_shoot_y(51), prev_pos_x_(0.0f), prev_pos_y_(0.0f),
   immunity_duration_(0.5f), key(NONE), last_key_pressed(NONE), lock_dir_left(false),
   lock_dir_right(false), lock_dir_up(false), rand_idle(0) {

   // Set entity direction
   entity_direction = RIGHT;

   // Setup Box2D
   // Set body type
   body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x = 600.0f * Application::get_instance().to_meters_;
   float y = -412.5f * Application::get_instance().to_meters_;
   body_def.position.Set(x, y);
   body_def.fixedRotation = true;

   // Attach body to world
   body = Application::get_instance().world_.CreateBody(&body_def);

   // Set box dimensions
   float width = (get_width() / 2.0f) * Application::get_instance().to_meters_ - 0.02f;// - 0.11f;
   float height = (get_height() / 2.0f) * Application::get_instance().to_meters_ - 0.02f;// - 0.11f;
   //printf("width = %d, height = %d\n", get_width(), get_height());
   const b2Vec2 center = {(PC_OFF_X - get_width()) / 2.0f * Application::get_instance().to_meters_, 
                          PC_OFF_Y * Application::get_instance().to_meters_};
   box.SetAsBox(width, height, center, 0.0f);

   // TODO: ADD FIXTURES TO THIS AS SENSORS
   //left_sensor_ = new PlayerSensor(0.4f, 0.0f, this, CONTACT_LEFT, -0.05f);
   //right_sensor_ = new PlayerSensor(0.4f, 0.0f, this, CONTACT_RIGHT, 0.30f);
   //bottom_sensor = new PlayerSensor(0.0f, 0.15f, this, CONTACT_DOWN, 0.125f, -0.5f);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 0.0f;
   fixture_def.userData = this;
   body->CreateFixture(&fixture_def);

   // Set user data
   //body->SetUserData(this);

   // Set sensors to non interactive with da enemy
   b2Filter filter;
   filter.categoryBits = 0;
   filter.maskBits = 0;
   b2Fixture *fixture_list = body->GetFixtureList();
   fixture_list->SetFilterData(filter);

   // ADD BODY PARTS FOR LEFT BODY
   player_body_right_.push_back(new PlayerHead(this, 11.5f, 26));
   player_body_right_.push_back(new PlayerArm(this, 3.5, -7, 6, 22, "PlayerLeftArm"));
   player_body_right_.push_back(new PlayerArm(this, 19.5, -7, 6, 22, "PlayerRightArm"));
   // player_body_right_.push_back(new PlayerHand(this, 6, -41, "PlayerLeftHand"));
   // player_body_right_.push_back(new PlayerHand(this, 17, -41, "PlayerRightHand"));
   player_body_right_.push_back(new PlayerLeg(this, 5.5, -33, 6, 34, "PlayerLeftLeg"));
   player_body_right_.push_back(new PlayerLeg(this, 18.5, -33, 6, 34, "PlayerRightLeg"));

   // ADD BODY PARTS FOR RIGHT BODY
   player_body_left_.push_back(new PlayerHead(this, 11.5f, 26));
   player_body_left_.push_back(new PlayerArm(this, 3.5, -7, 6, 22, "PlayerLeftArm"));
   player_body_left_.push_back(new PlayerArm(this, 19.5, -7, 6, 22, "PlayerRightArm"));
   // player_body_left_.push_back(new PlayerHand(this, 6, -41, "PlayerLeftHand"));
   // player_body_left_.push_back(new PlayerHand(this, 17, -41, "PlayerRightHand"));
   player_body_left_.push_back(new PlayerLeg(this, 5.5, -33, 6, 34, "PlayerLeftLeg"));
   player_body_left_.push_back(new PlayerLeg(this, 18.5, -33, 6, 34, "PlayerRightLeg"));

   // Deactivate the left
   for (int i = 0; i < player_body_left_.size(); i++) {
      player_body_left_[i]->Sensor::deactivate_sensor();
   }

   // Set health. TODO: set health in a better way
   health = 300;

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

// Get texture based on state
Texture *Player::get_texture() {
   // Return run or stop texture (aka running texture)
   if (player_state_ == RUN || player_state_ == STOP) {
      return textures["running"];
   }

   // Return jump texture
   if (player_state_ == JUMP) {
      return textures["jump"];
   }

   // Return run and jump texture
   if (player_state_ == RUN_AND_JUMP) {
      return textures["running_jump"];
   }

   // Return double jump
   if (player_state_ == DOUBLE_JUMP) {
      return textures["double_jump"];
   }

   // Return push texture
   if (player_state_ == PUSH) {
      return textures["push"];
   }

   // Return jump and push texture
   if (player_state_ == JUMP_AND_PUSH) {
      return textures["jump_push"];
   }

   // Return balance
   if (player_state_ == BALANCE) {
      return textures["balance"];
   }

   // Death
   if (player_state_ == DEATH) {
      return textures["death"];
   }

   // Check which idle is being used and return it
   if (rand_idle <= 98) {
      return textures["tap"];
   }
   if (rand_idle == 99) {
      return textures["look"];
   }
   if (rand_idle == 100) {
      return textures["kick"];
   }
}

// Get player state
Player::STATE Player::get_player_state() {
   return player_state_;
}

// Process keyboard input
void Player::process_input(const Uint8 *key_state) {  
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
      // Set shooting to true
      shooting = true;

      // Create eraser
      if (textures["arm_throw"]->completed_) {
         TextureData normal = {0, 0.0f, "", ""};
         TextureData hit = {0, 0.0f, "", ""};
         CreateProjectile(38, -12, 41, 1, 10, 0.0f, 0.0f, normal, hit);
         textures["arm_throw"]->completed_ = false;
      }
   }
}

// Update function
void Player::update(bool freeze) {
   //std::cout << "State: " << player_state_ << " (0: STAND, 1: RUN, 2: JUMP, 3: DOUBLE_JUMP, 4: STOP, 5: RUN_AND_JUMP, 6: PUSH)" << std::endl;
   //std::cout << "X = " << body->GetLinearVelocity().x << " Y = " << body->GetLinearVelocity().y << std::endl;
   //std::cout << in_contact_down << std::endl;
   //std::cout << "x_pos = " << get_x() << " y_pos = " << get_y() << std::endl;
   //std::cout << "Key = " << key << std::endl;

   // Apply artificial force of gravity
   const b2Vec2 sim_grav = {0.0f, SIM_GRAV};
   body->ApplyForceToCenter(sim_grav, true);

   // Process key inputs
   process_input(Application::get_instance().current_key_states_);

   // Animate the function
   animate();

   // Update player if not frozen
   if (!freeze) {
      move();
   }

   // Adjust deltas first
   adjust_deltas();

   // Render player
   Texture *playertexture = get_texture();

   // Render arm if idle, Render shooting if not
   if (player_state_ != PUSH && player_state_ != JUMP_AND_PUSH && player_state_ != BALANCE && player_state_ != DEATH) {
      if (!shooting) {
         // if (get_player_state() == RUN) {
         //    textures["running_arm"].Render(get_tex_x() + get_width() +
         //       arm_delta_x, get_tex_y() + arm_delta_y,
         //       textures["running_arm"].curr_clip_, 0.0,
         //       &textures["running_arm"].center_, textures["running_arm"].flip_);
         // } else if (get_player_state() == DOUBLE_JUMP) {
         //    textures["double_jump_arm"].Render(get_tex_x() + get_width() +
         //       arm_delta_x, get_tex_y() + arm_delta_y, textures["double_jump_arm"].curr_clip_, 0.0,
         //       &textures["double_jump_arm"].center_, textures["double_jump_arm"].flip_);
         // } else {
         //    //std::cout << textures["idle_arm"].max_frame_ << std::endl;
         //    textures["idle_arm"].Render(get_tex_x() + get_width() + 
         //       arm_delta_x, get_tex_y() + arm_delta_y, textures["idle_arm"].curr_clip_, 0.0, 
         //       &textures["idle_arm"].center_, textures["idle_arm"].flip_);
         // }
      } else {
         // textures["arm_throw"].Render(get_tex_x() + get_width() + 
         //    arm_delta_shoot_x, get_tex_y() + arm_delta_shoot_y, 
         //    textures["arm_throw"].curr_clip_, 0.0, 
         //    &textures["arm_throw"].center_, textures["arm_throw"].flip_);
      }
   }

   // Render player
   Render(playertexture);
}

// Adjust delta function
void Player::adjust_deltas() {
   if (player_state_ == STAND) {
      if (entity_direction == RIGHT) {
         arm_delta_x = -3;
         arm_delta_y = 47;
         arm_delta_shoot_x = -3;
         arm_delta_shoot_y = 39;
      } else {
         arm_delta_x = -20;
         arm_delta_y = 47;
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
void Player::animate(Texture *tex, int reset, int max, int start) {
   // Shooting animation
   if (shooting) {
      textures["arm_throw"]->last_frame += 
      (textures["arm_throw"]->fps_timer.getDeltaTime() / 1000.0f);
      if (textures["arm_throw"]->last_frame > Application::get_instance().animation_update_time_) {
         if (textures["arm_throw"]->frame_ > 6) {
            textures["arm_throw"]->frame_ = 0;
            shooting = false;
            textures["arm_throw"]->completed_ = true;
         }
         textures["arm_throw"]->curr_clip_ = &textures["arm_throw"]->clips_[textures["arm_throw"]->frame_];
         ++textures["arm_throw"]->frame_;
         textures["arm_throw"]->last_frame = 0.0f;
      }
      Element::animate(textures["eraser"]);
   } else {
      Element::animate(textures["idle_arm"], 0);
   }

   // Choose animation based on what state player is in
   if (player_state_ == STAND) {
      // Choose a random number if at least one of the animations is complete
      if (curr_idle_texture->frame_ > curr_idle_texture->max_frame_) {
         rand_idle = rand() % 100 + 1;

         // Set curr_idle_texture
         if (rand_idle <= 98) {
            curr_idle_texture = textures["tap"];
         }
         if (rand_idle == 99) {
            curr_idle_texture = textures["look"];
         }
         if (rand_idle == 100) {
            curr_idle_texture = textures["kick"];
         }
      }

      // Choose based on random number
      if (rand_idle <= 98) {
         Element::animate(textures["tap"]);
      }
      if (rand_idle == 99) {
         Element::animate(textures["look"]);
      }
      if (rand_idle == 100) {
         Element::animate(textures["kick"]);
      }
   } else if (player_state_ == RUN && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      Element::animate(textures["running"]);
      Element::animate(textures["running_arm"]);
   } else if (player_state_ == STOP && (contacts_[LEFT_LEG] || contacts_[RIGHT_LEG])) {
      Element::animate(textures["running"], 20);
   } else if (player_state_ == JUMP) {
      Element::animate(textures["jump"], textures["jump"]->reset_frame, textures["jump"]->stop_frame);
   } else if (player_state_ == RUN_AND_JUMP) {
      Element::animate(textures["running_jump"], textures["running_jump"]->reset_frame, textures["running_jump"]->stop_frame);
   } else if (player_state_ == DOUBLE_JUMP) {
      Element::animate(textures["double_jump"], textures["double_jump"]->reset_frame, textures["double_jump"]->stop_frame);
      Element::animate(textures["double_jump_arm"], textures["double_jump_arm"]->reset_frame, textures["double_jump_arm"]->stop_frame);
   } else if (player_state_ == PUSH) {
      Element::animate(textures["push"]);
   } else if (player_state_ == JUMP_AND_PUSH) {
      Element::animate(textures["jump_push"]);
   } else if (player_state_ == BALANCE) {
      Element::animate(textures["balance"]);
   } else if (player_state_ == DEATH) {
      Element::animate(textures["death"], textures["death"]->reset_frame, textures["death"]->stop_frame);
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
   bool right = Application::get_instance().current_key_states_[SDL_SCANCODE_RIGHT];
   bool left = Application::get_instance().current_key_states_[SDL_SCANCODE_LEFT];
   bool up = Application::get_instance().current_key_states_[SDL_SCANCODE_UP];

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
void Player::move() {
   // Death 
   if (player_state_ == DEATH) {
      // TODO: FIX DEATH CLIPPING THROUGH PLATFORM
      if (!shift_) {
         sub_x(10);
         body->SetLinearVelocity({0.0f, body->GetLinearVelocity().y});
         shift_ = true;
      }
      if (textures["death"]->frame_ >= 20) {
         if (((float) Application::get_instance().death_timer_.getTicks() / 1000.0f) >= 3.0f) {
            alive = false;
            Application::get_instance().death_timer_.stop();
         }
         textures["death"]->reset_frame = 19;
         textures["death"]->stop_frame = 19;
      }
      return;
   }

   // Deactivate and activate bodies
   if (entity_direction == LEFT && !right_deactivated_) {
      for (int i = 0; i < player_body_left_.size(); i++) {
         player_body_right_[i]->Sensor::deactivate_sensor();
         player_body_left_[i]->Sensor::activate_sensor();
      }
      right_deactivated_ = true;
      left_deactivated_ = false;
   } else if (entity_direction == RIGHT && !left_deactivated_) {
      for (int i = 0; i < player_body_right_.size(); i++) {
         player_body_left_[i]->Sensor::deactivate_sensor();
         player_body_right_[i]->Sensor::activate_sensor();
      }
      left_deactivated_ = true;
      right_deactivated_ = false;
   }

   // Reset frames
   if (player_state_ != DOUBLE_JUMP) {
      textures["double_jump_arm"]->reset_frame = 0;
   } else {
      textures["double_jump_arm"]->reset_frame = textures["double_jump_arm"]->max_frame_;
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
      textures["running_arm"]->frame_ = textures["running"]->frame_;

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
      if (entity_direction == LEFT) {
         for (auto i = textures.begin(); i != textures.end(); i++) {
            i->second->has_flipped_ = true;
            // i->second.flip_ = SDL_FLIP_HORIZONTAL;
         }
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
      if (entity_direction == RIGHT) {
         for (auto i = textures.begin(); i != textures.end(); i++) {
            i->second->has_flipped_ = false;
            // i->second.flip_ = SDL_FLIP_NONE;
         }
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
            // Set state
            if (player_state_ == RUN) {
               //player_state_ = RUN_AND_JUMP;
               textures["running_jump"]->reset_frame = 14;
               textures["running_jump"]->stop_frame = 14;
            } else {
               //player_state_ = JUMP;
               textures["jump"]->reset_frame = textures["jump"]->max_frame_;
               textures["jump"]->stop_frame = textures["jump"]->max_frame_;
            }

            // Apply an impulse
            const b2Vec2 force = {0, 2.8f};
            body->ApplyLinearImpulse(force, body->GetPosition(), true);

            // Set the flags
            ++has_jumped_;
         } else {
            // Set state
            if (player_state_ == RUN) {
               //player_state_ = RUN_AND_JUMP;
               textures["running_jump"]->reset_frame = 14;
               textures["running_jump"]->stop_frame = 14;
            } else {
               //player_state_ = JUMP;
               textures["double_jump"]->reset_frame = textures["double_jump"]->max_frame_;
               textures["double_jump"]->stop_frame = textures["double_jump"]->max_frame_;
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
void Player::start_contact(Element *element) {
   if (element && (element->type() == "Projectile" || (element->is_enemy()))) {
      take_damage(10);
   }
}

// End contact function (does nothing for now)
void Player::end_contact(Element *element) {
   return;
}

// Load media function
bool Player::LoadMedia() {
   // Temp flag
   bool success = true;

   // Vector for images
   std::vector<TextureData> data;
   data.push_back(TextureData(17, 1.0f / 24.0f, "kick", media_path + "idle_kick_no_arm.png"));
   data.push_back(TextureData(12, 1.0f / 24.0f, "tap", media_path + "idle_tap_no_arm.png"));
   data.push_back(TextureData(20, 1.0f / 24.0f, "look", media_path + "idle_look_no_arm.png"));
   data.push_back(TextureData(15, 1.0f / 24.0f, "jump", media_path + "idle_jump_no_arm.png"));
   data.push_back(TextureData(15, 1.0f / 30.0f, "running", media_path + "running_no_arm.png"));
   data.push_back(TextureData(11, 1.0f / 24.0f, "double_jump", media_path + "double_jump.png"));
   data.push_back(TextureData(15, 1.0f / 24.0f, "running_jump", media_path + "running_jump_no_arm.png"));
   data.push_back(TextureData(5, 1.0f / 20.0f, "idle_arm", media_path + "idle_arm.png"));
   data.push_back(TextureData(9, 1.0f / 20.0f, "arm_throw", media_path + "arm_throw.png"));
   data.push_back(TextureData(15, 1.0f / 30.0f, "running_arm", media_path + "running_arm.png"));
   data.push_back(TextureData(8, 1.0f / 24.0f, "double_jump_arm", media_path + "double_jump_arm.png"));
   data.push_back(TextureData(16, 1.0f / 20.0f, "push", media_path + "push.png"));
   data.push_back(TextureData(8, 1.0f / 20.0f, "jump_push", media_path + "jump_push.png"));
   data.push_back(TextureData(19, 1.0f / 20.0f, "balance", media_path + "balance.png"));
   data.push_back(TextureData(20, 1.0f / 20.0f, "death", media_path + "death.png"));

   // Instantiate sprite sheet
   std::string player_path = media_path + "player_master_sheet.png";
   sprite_sheet = RenderingEngine::get_instance().LoadTexture("player_master_sheet", player_path.c_str());
   sprite_sheet->animations.emplace("jump_push", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 0.0f, 8, 1.0f / 20.0f));
   sprite_sheet->animations.emplace("double_jump", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 104.0f / 2394.0f, 11, 1.0f / 24.0f));
   sprite_sheet->animations.emplace("tap", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 208.0f / 2394.0f, 12, 1.0f / 24.0f));
   sprite_sheet->animations.emplace("running", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 312.0f / 2394.0f, 15, 1.0f / 30.0f));
   sprite_sheet->animations.emplace("jump", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 416.0f / 2394.0f, 15, 1.0f / 24.0f));
   sprite_sheet->animations.emplace("running_jump", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 520.0f / 2394.0f, 15, 1.0f / 24.0f));
   sprite_sheet->animations.emplace("push", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 624.0f / 2394.0f, 16, 1.0f / 20.0f));
   sprite_sheet->animations.emplace("kick", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 728.0f / 2394.0f, 17, 1.0f / 24.0f));
   sprite_sheet->animations.emplace("look", new Animation(2394.0f, 1144.0f, 59.0f, 104.0f, 832.0f / 2394.0f, 20, 1.0f / 24.0f));
   sprite_sheet->animations.emplace("death", new Animation(2394.0f, 1144.0f, 105.0f, 104.0f, 936.0f / 2394.0f, 20, 1.0f / 20.0f));
   sprite_sheet->animations.emplace("balance", new Animation(2394.0f, 1144.0f, 89.0f, 104.0f, 1040.0f / 1144.0f, 19, 1.0f / 20.0f));

   // Load the resources
   success = RenderingEngine::get_instance().LoadResources(this, data);

   // Set current idle texture to tap
   curr_idle_texture = textures["tap"];

   // Return success
   return success;
}

// Create projectile
Projectile* Player::CreateProjectile(int delta_x_r, int delta_x_l, int delta_y,
     bool owner, bool damage, 
     float force_x, float force_y,
     const TextureData &normal, const TextureData &hit) {
   // First, create a new projectile
   Projectile *proj;

   // Create based on direction
   if (entity_direction == RIGHT) {
      proj = new Eraser(get_tex_x() + get_width() + delta_x_r, get_tex_y() + delta_y, 
            normal, hit, this);
   } else {
      proj = new Eraser(get_tex_x() + delta_x_l, get_tex_y() + delta_y, normal, hit, this);
   }

   // Set shot direction
   proj->shot_dir = entity_direction;

   // Return projectile reference
   return proj;
}

// Take damage function
void Player::take_damage(int damage) {
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
         Application::get_instance().death_timer_.start();
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

   // Load data
   std::vector<TextureData> data;
   data.push_back(TextureData(21, 1.0 / 20.0f, "alive", Player::media_path + "hp_idle.png"));
   data.push_back(TextureData(19, 1.0 / 20.0f, "dead", Player::media_path + "hp_hit.png"));
   success = RenderingEngine::get_instance().LoadResources(this, data);

   // Return success
   return success;
}

// Animate
void Hitmarker::animate() {
   if (state == ALIVE) {
      Element::animate(textures["alive"]);
   } else if (state == DEAD) {
      Element::animate(textures["dead"], 18);
   }
}

// Get texture
Texture *Hitmarker::get_texture() {
   if (state == ALIVE) {
      return textures["alive"];
   }
   if (state == DEAD) {
      return textures["dead"];
   }
}

// Update function
void Hitmarker::update(bool freeze) {
   // Animate and Render
   animate();
   Texture *tex = get_texture();
   Render(tex);
}