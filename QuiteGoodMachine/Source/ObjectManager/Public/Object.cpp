#include "QuiteGoodMachine/Source/ObjectManager/Private/Object.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Enemy.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/TangibleElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include <iostream>

/******************** Object Implementations **********************/

// Constructor
Object::Object(std::string name, int x, int y, int width, int height, Entity* owner) :
   Element(name, x, y, width, height), owning_entity(owner), shift(false) {}

/***************** Platform Implementations *************************/

// Constructor
Platform::Platform(std::string name, int x, int y, int width, int height) :
   Object(name, x, y, width, height, NULL) {

   // Set position
   float x_pos = x * Application::GetInstance().to_meters_;
   float y_pos = -y * Application::GetInstance().to_meters_;
   body_def.position.Set(x_pos, y_pos);
   body_def.fixedRotation = true;
}

// Setup function for Box2D stuff
void Platform::setup() {
   // Create body
   body = Application::GetInstance().world_.CreateBody(&body_def);

   // Set box
   float box_x = (get_width() / 2.0f) * Application::GetInstance().to_meters_;
   float box_y = (get_height() / 2.0f) * Application::GetInstance().to_meters_;
   box.SetAsBox(box_x, box_y);

   // Create fixture
   fixture_def.shape = &box;
   fixture_def.density = 0.0f;
   fixture_def.friction = 0.0f;
   fixture_def.userData = this;
   fixture_def.filter.categoryBits = CAT_PLATFORM;
   fixture_def.filter.maskBits = CAT_PLAYER | CAT_PROJECTILE | CAT_ENEMY | CAT_BOSS | CAT_SENSOR;
   body->CreateFixture(&fixture_def);

   // Set user data
   body->SetUserData(this);
}

// Virtual destructor
Platform::~Platform() {
}

/************** Projectile Implementations  *************************/

// Constructor
Projectile::Projectile(std::string name, int x, int y, float width, float height,
      bool owner, int damage, float force_x, float force_y, Entity *entity) :
   Object(name, x, y, 0.0f, 0.0f, entity), 
   owner_(owner), damage_(damage), normal_hit_diff_x_(0.0f), normal_hit_diff_y_(0.0f), name(name) {

   // Load the media
   LoadMedia();

   // Calculate difference
   // TODO: Find a better way to fix difference between normal and hit projectiles
   normal_hit_diff_x_ = GetAnimationByName(name + "_hit")->half_width - GetAnimationByName(name + "_normal")->half_width;
   normal_hit_diff_y_ = GetAnimationByName(name + "_hit")->half_height - GetAnimationByName(name + "_normal")->half_height;

   // Set body type
   body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x_pos, y_pos;
   x_pos = x * Application::GetInstance().to_meters_;
   y_pos = -y * Application::GetInstance().to_meters_;
   body_def.position.Set(x_pos, y_pos);
   body_def.fixedRotation = false;

   // Attach body to world
   body = Application::GetInstance().world_.CreateBody(&body_def);

   // Set box dimensions
   float width_dim = ((width - 2.0f) / 2.0f) * Application::GetInstance().to_meters_;
   float height_dim = ((height - 4.0f) / 2.0f) * Application::GetInstance().to_meters_;
   box.SetAsBox(width_dim, height_dim);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 1.0f;
   fixture_def.userData = this;
   fixture_def.filter.categoryBits = CAT_PROJECTILE;
   fixture_def.filter.maskBits = CAT_ENEMY | CAT_PLATFORM | CAT_PLAYER | CAT_BOSS | CAT_SENSOR;
   body->CreateFixture(&fixture_def);

   // Give it an x direction impulse
   b2Vec2 force;
   if (owning_entity->GetDirection() == TangibleElement::RIGHT) {
      force = {force_x, force_y};
   } else {
      force = {-force_x, force_y};
      FlipAllAnimations();
      texture_flipped = true;
   }
   body->ApplyForce(force, body->GetPosition(), true);
   
   // Now add it to the things the world needs to Render
   Application::GetInstance().getProjectileVector()->push_back(this);
   
   // Set object state
   object_state_ = ALIVE;

   // Finally, set the user data
   body->SetUserData(this);
}

// Update function
void Projectile::Update() {
   // Check the status
   if (object_state_ == ALIVE) {
      // Get normal animation and animate it
      sprite_sheet->Animate(GetAnimationFromState());

      // Render
      sprite_sheet->Render(get_hitbox_x(), get_hitbox_y(), 0.0f, GetAnimationFromState(), true);
   } else if (object_state_ == DEAD) {
      if (GetAnimationFromState()->curr_frame >= GetAnimationFromState()->max_frame) {
         alive = false;
         return;
      }

      // Get the hit animation and animate it
      sprite_sheet->Animate(GetAnimationFromState());

      // Render
      sprite_sheet->Render(get_hitbox_x(), get_hitbox_y() + normal_hit_diff_y_, 0.0f, GetAnimationFromState(), true);
   }
}

// Get owner of projectile
bool Projectile::get_owner() const {
   return owner_;
}  

// Get damage of projectile
int Projectile::get_damage() const {
   return damage_;
}

Animation *Projectile::GetAnimationFromState() {
   if (object_state_ == ALIVE) {
      return GetAnimationByName(name + "_normal");
   }

   if (object_state_ == DEAD) {
      return GetAnimationByName(name + "_hit");
   }
}

bool Projectile::LoadMedia() {
   // std::cout << "Loading projectile media\n";
   std::string proj_path = Application::sprite_path + "Projectiles/projectile_master_sheet.png";
   sprite_sheet = RenderingEngine::GetInstance().LoadTexture("projectile_master_sheet", proj_path.c_str());

   /* Load specific projectiles */
   animations.emplace("player_projectile_normal", new Animation(sprite_sheet, "player_projectile_normal", 23.0f, 14.0f, 0.0f, 3, 1.0f / 20.0f));
   animations.emplace("player_projectile_hit", new Animation(sprite_sheet, "player_projectile_hit", 23.0f, 14.0f, 14.0f, 4, 1.0f / 20.0f));
   animations.emplace("fecreez_projectile_normal", new Animation(sprite_sheet, "fecreez_projectile_normal", 26.0f, 16.0f, 28.0f, 8, 1.0f / 20.0f));
   animations.emplace("fecreez_projectile_hit", new Animation(sprite_sheet, "fecreez_projectile_hit", 26.0f, 17.0f, 44.0f, 8, 1.0f / 20.0f));
   animations.emplace("fruig_projectile_normal", new Animation(sprite_sheet, "fruig_projectile_normal", 12.0f, 11.0f, 61.0f, 5, 1.0f / 20.0f));
   animations.emplace("fruig_projectile_hit", new Animation(sprite_sheet, "fruig_projectile_hit", 67.0f, 11.0f, 72.0f, 9, 1.0f / 20.0f));

   // Load animations
   RenderingEngine::GetInstance().LoadResources(this);
}

// Adjust start contact function
void Projectile::StartContact(Element *element) {
   if (element->type() == "Player" || element->type() == "Platform" || element->is_enemy()) {
      object_state_ = DEAD;
      b2Vec2 stop = {0.0f, 0.0f};
      body->SetLinearVelocity(stop);
   }
}

// Destructor
Projectile::~Projectile() {}