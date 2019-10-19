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
   float x_pos = x * Application::get_instance().to_meters_;
   float y_pos = -y * Application::get_instance().to_meters_;
   body_def.position.Set(x_pos, y_pos);
   body_def.fixedRotation = true;
}

// Setup function for Box2D stuff
void Platform::setup() {
   // Create body
   body = Application::get_instance().world_.CreateBody(&body_def);

   // Set box
   float box_x = (get_width() / 2.0f) * Application::get_instance().to_meters_ - 0.01f;
   float box_y = (get_height() / 2.0f) * Application::get_instance().to_meters_ - 0.01f;
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
Projectile::Projectile(int x, int y, bool owner, int damage, 
      float force_x, float force_y,
      const TextureData &normal, const TextureData &hit,
      Entity *entity) :
   Object(x, y, normal.height, normal.width, entity), 
   owner_(owner), damage_(damage), normal_(normal), hit_(hit) {

   // Set body type
   body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x_pos, y_pos;
   x_pos = x * Application::get_instance().to_meters_;
   y_pos = -y * Application::get_instance().to_meters_;
   body_def.position.Set(x_pos, y_pos);
   body_def.fixedRotation = false;

   // Attach body to world
   body = Application::get_instance().world_.CreateBody(&body_def);

   // Set box dimensions
   float width_dim = (get_width() / 2.0f) * Application::get_instance().to_meters_ - 0.02f;// - 0.11f;
   float height_dim = (get_height() / 2.0f) * Application::get_instance().to_meters_ - 0.02f;// - 0.11f;
   box.SetAsBox(width_dim, height_dim);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 1.0f;
   fixture_def.userData = this;
   body->CreateFixture(&fixture_def);

   // Give it an x direction impulse
   b2Vec2 force;
   if (owning_entity->entity_direction == RIGHT) {
      force = {force_x, force_y};
   } else {
      force = {-force_x, force_y};
   }
   body->ApplyForce(force, body->GetPosition(), true);
   
   // Now add it to the things the world needs to Render
   Application::get_instance().getProjectileVector()->push_back(this);
   
   // Set object state
   object_state_ = ALIVE;

   // Finally, set the user data
   body->SetUserData(this);
}

// Update function
void Projectile::update() {
   // Check the status
   if (object_state_ == ALIVE) {
      Element::animate(textures["normal"]);
      if (shot_dir == LEFT) {
         // textures["normal"].Render(get_tex_x(), get_tex_y(), textures["normal"].curr_clip_, 0.0, NULL, SDL_FLIP_HORIZONTAL);
      } else {
         // textures["normal"].Render(get_tex_x(), get_tex_y(), textures["normal"].curr_clip_, 0.0, NULL, SDL_FLIP_NONE);
      }
   } else if (object_state_ == DEAD) {
      if (body) {
         Application::get_instance().world_.DestroyBody(body);
         body = nullptr;
      }
      if (textures["explode"]->frame_ >= hit_.num_of_frames) {
         alive = false;
         return;
      }
      Element::animate(textures["explode"]);
      if (shot_dir == LEFT) {
         // textures["explode"].Render(get_tex_x(), get_tex_y(), textures["explode"].curr_clip_, 0.0, NULL, SDL_FLIP_HORIZONTAL);
      } else {
         // textures["explode"].Render(get_tex_x(), get_tex_y(), textures["explode"].curr_clip_, 0.0, NULL, SDL_FLIP_NONE); 
      }
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

// Destructor
Projectile::~Projectile() {}

/************* ERASER SUBCLASS *******************/
Eraser::Eraser(int x, int y, 
   const TextureData &normal, const TextureData &hit,
   Entity *entity) :
   Projectile(x, y, 1, 10, 10.4f, 0.0f, normal, hit, entity) {

   // Load media for some reason
   LoadMedia();

   // Set filter
   b2Filter filter;
   filter.groupIndex = -5;
   body->GetFixtureList()->SetFilterData(filter);
}

// Adjust start contact function
void Eraser::start_contact(Element *element) {
   if (element->is_enemy() || element->type() == "Platform") {
      object_state_ = DEAD;
      b2Vec2 stop = {0.0f, 0.0f};
      body->SetLinearVelocity(stop);
   }
}

// Load eraser media
bool Eraser::LoadMedia() {
   // Success flag
   bool success = true;

   // Instantiate data
   std::vector<TextureData> data;
   data.push_back(TextureData(3, 1.0f / 20.0f, "normal", Player::media_path + "eraser.png"));
   data.push_back(TextureData(4, 1.0f / 20.0f, "explode", Player::media_path + "eraser_break.png"));

   // Load resources
   success = RenderingEngine::get_instance().LoadResources(this, data);

   // Return success
   return success;
}

/*************** ENEMY PROJECTILE SUBCLASS ***************/
EnemyProjectile::EnemyProjectile(int x, int y, int damage, 
   float force_x, float force_y,
   const TextureData &normal, const TextureData &hit,
   Entity *entity) :
   Projectile(x, y, 0, 10, force_x, force_y, normal, hit, entity) {

   // Load media for some reason
   LoadMedia();
}

// Adjust start contact function
void EnemyProjectile::start_contact(Element *element) {
   if (element->type() == "Player" || element->type() == "Platform" || 
       element->type() == "Projectile" || element->is_enemy()) {
      object_state_ = DEAD;
      b2Vec2 stop = {0.0f, 0.0f};
      body->SetLinearVelocity(stop);
   }
}

// Load eraser media
bool EnemyProjectile::LoadMedia() {
   // SUccess flag
   bool success = true;

   // Instantiate data
   normal_.path = Enemy::media_path + owning_entity->type() + "/projectile.png";
   hit_.path = Enemy::media_path + owning_entity->type() + "/hit.png";

   std::vector<TextureData> data;
   data.push_back(TextureData(normal_.num_of_frames, 1.0f / 20.0f, "normal", normal_.path));
   data.push_back(TextureData(hit_.num_of_frames, 1.0f / 20.0f, "explode", hit_.path));

   // Return success
   return success;   
}