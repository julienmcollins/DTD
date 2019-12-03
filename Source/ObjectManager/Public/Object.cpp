#include "Source/ObjectManager/Private/Object.h"
#include "Source/ObjectManager/Private/Entity.h"
#include "Source/ObjectManager/Private/Enemy.h"

#include "Source/RenderingEngine/Private/RenderingEngine.h"
#include "Source/RenderingEngine/Private/Texture.h"

#include "Source/GameEngine/Private/Application.h"

#include <iostream>

/******************** Object Implementations **********************/

// Constructor
Object::Object(int x, int y, int height, int width, Entity* owner) :
   Element(x, y, height, width), owning_entity(owner), shift(false) {}

/***************** Platform Implementations *************************/

// Constructor
Platform::Platform(int x, int y, int height, int width) :
   Object(x, y, height, width, NULL) {

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
   float box_x = (get_width() / 2.0f) * Application::GetInstance().to_meters_ - 0.01f;
   float box_y = (get_height() / 2.0f) * Application::GetInstance().to_meters_ - 0.01f;
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
   Object(x, y, 0.0f, 0.0f, entity), 
   owner_(owner), damage_(damage), name(name) {

   // Load the media
   LoadMedia();

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
   float width_dim = (width / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;// - 0.11f;
   float height_dim = (height / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;// - 0.11f;
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
   if (owning_entity->entity_direction == RIGHT) {
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
void Projectile::update() {
   // Check the status
   if (object_state_ == ALIVE) {
      // Get normal animation and animate it
      sprite_sheet->Animate(GetAnimationFromState());

      // Render
      // std::cout << "Life " << get_anim_x() << " " << get_anim_y() << std::endl;
      sprite_sheet->Render(get_anim_x(), get_anim_y(), 0.0f, GetAnimationFromState());
   } else if (object_state_ == DEAD) {
      if (GetAnimationFromState()->curr_frame >= GetAnimationFromState()->max_frame) {
         alive = false;
         return;
      }

      // Get the hit animation and animate it
      sprite_sheet->Animate(GetAnimationFromState());

      // Render
      // std::cout << "Death " << get_anim_x() << " " << get_anim_y() << std::endl;
      sprite_sheet->Render(get_anim_x(), get_anim_y(), 0.0f, GetAnimationFromState());
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
   if (name == "player_projectile") { // Load eraser
      animations.emplace("player_projectile_normal", new Animation(sprite_sheet, "player_projectile_normal", 22.0f, 12.0f, 0.0f, 3, 1.0f / 20.0f));
      animations.emplace("player_projectile_hit", new Animation(sprite_sheet, "player_projectile_hit", 21.0f, 12.0f, 12.0f, 4, 1.0f / 20.0f));
   } else if (name == "fecreez_projectile") { // Load fecreez projectile
      animations.emplace("fecreez_projectile_normal", new Animation(sprite_sheet, "fecreez_projectile_normal", 24.0f, 15.0f, 24.0f, 8, 1.0f / 20.0f));
      animations.emplace("fecreez_projectile_hit", new Animation(sprite_sheet, "fecreez_projectile_hit", 24.0f, 15.0f, 39.0f, 8, 1.0f / 20.0f));
   } else if (name == "fruig_projectile") { // Load fruig projectile
      animations.emplace("fruig_projectile_normal", new Animation(sprite_sheet, "fruig_projectile_normal", 10.0f, 9.0f, 54.0f, 5, 1.0f / 20.0f));
      animations.emplace("fruig_projectile_hit", new Animation(sprite_sheet, "fruig_projectile_hit", 65.0f, 9.0f, 63.0f, 9, 1.0f / 20.0f));
   }

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