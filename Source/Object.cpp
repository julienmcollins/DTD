#include <iostream>
#include "Entity.h"
#include "Object.h"
#include "Texture.h"
#include "Application.h"

/******************** Object Implementations **********************/

// Constructor
Object::Object(int x, int y, int height, int width, Entity* owner, Application *application) :
   Element(x, y, height, width, application), owning_entity(owner), shift(false) {}

/***************** Platform Implementations *************************/

// Constructor
Platform::Platform(int x, int y, int height, int width, Application *application) :
   Object(x, y, height, width, NULL, application) {

   // Set position
   float x_pos = x * application->to_meters_;
   float y_pos = -y * application->to_meters_;
   body_def.position.Set(x_pos, y_pos);
   body_def.fixedRotation = true;
}

// Setup function for Box2D stuff
void Platform::setup() {
   // Create body
   body = get_application()->world_.CreateBody(&body_def);

   // Set box
   float box_x = (get_width() / 2.0f) * get_application()->to_meters_ - 0.01f;
   float box_y = (get_height() / 2.0f) * get_application()->to_meters_ - 0.01f;
   box.SetAsBox(box_x, box_y);

   // Create fixture
   fixture_def.shape = &box;
   fixture_def.density = 0.0f;
   fixture_def.userData = this;
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
      Entity *entity, Application *application) :
   Object(x, y, normal.height, normal.width, entity, application), 
   owner_(owner), damage_(damage), normal_(normal), hit_(hit) {

   // Temp pointer to App
   Application *tmp = get_application();

   // Set body type
   body_def.type = b2_dynamicBody;

   // Set initial position and set fixed rotation
   float x_pos, y_pos;
   x_pos = x * tmp->to_meters_;
   y_pos = -y * tmp->to_meters_;
   body_def.position.Set(x_pos, y_pos);
   body_def.fixedRotation = false;

   // Attach body to world
   body = tmp->world_.CreateBody(&body_def);

   // Set box dimensions
   float width_dim = (get_width() / 2.0f) * tmp->to_meters_ - 0.02f;// - 0.11f;
   float height_dim = (get_height() / 2.0f) * tmp->to_meters_ - 0.02f;// - 0.11f;
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
   
   // Now add it to the things the world needs to render
   tmp->getProjectileVector()->push_back(this);
   
   // Set object state
   object_state_ = ALIVE;

   // Finally, set the user data
   body->SetUserData(this);
}

// Update function
void Projectile::update() {
   // Check the status
   if (object_state_ == ALIVE) {
      Element::animate(&textures["normal"]);
      if (shot_dir == LEFT) {
         textures["normal"].render(get_x(), get_y(), textures["normal"].curr_clip_, 0.0, NULL, SDL_FLIP_HORIZONTAL);
      } else {
         textures["normal"].render(get_x(), get_y(), textures["normal"].curr_clip_, 0.0, NULL, SDL_FLIP_NONE);
      }
   } else if (object_state_ == DEAD) {
      if (textures["explode"].frame_ >= hit_.frames) {
         alive = false;
         return;
      }
      Element::animate(&textures["explode"]);
      if (shot_dir == LEFT) {
         textures["explode"].render(get_x(), get_y(), textures["explode"].curr_clip_, 0.0, NULL, SDL_FLIP_HORIZONTAL);
      } else {
         textures["explode"].render(get_x(), get_y(), textures["explode"].curr_clip_, 0.0, NULL, SDL_FLIP_NONE); 
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
   Entity *entity, Application *application) :
   Projectile(x, y, 1, 10, 10.4f, 0.0f, normal, hit, entity, application) {

   // Load media for some reason
   load_media();
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
bool Eraser::load_media() {
   // Success flag
   bool success = true;

   // Normal eraser
   load_image(textures, this, 21, 12, 3, 1.0f / 20.0f, "normal", "images/player/eraser.png", success);

   // Exploading eraser
   load_image(textures, this, 21, 12, 4, 1.0f / 20.0f, "explode", "images/player/eraser_break.png", success);

   // Return success
   return success;
}

/*************** ENEMY PROJECTILE SUBCLASS ***************/
EnemyProjectile::EnemyProjectile(int x, int y, int damage, 
   float force_x, float force_y,
   const TextureData &normal, const TextureData &hit,
   Entity *entity, Application *application) :
   Projectile(x, y, 0, 10, force_x, force_y, normal, hit, entity, application) {

   // Load media for some reason
   load_media();
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
bool EnemyProjectile::load_media() {
   // SUccess flag
   bool success = true;

   // Normal projectile
   std::string normal = "images/enemies/" + owning_entity->type() + "/projectile.png";
   load_image(textures, this, normal_.width, normal_.height, normal_.frames, 1.0f / 20.0f, "normal", normal, success);

   // Hit projectile
   std::string hit = "images/enemies/" + owning_entity->type() + "/hit.png";
   load_image(textures, this, hit_.width, hit_.height, hit_.frames, 1.0f / 20.0f, "explode", hit, success);

   // Return success
   return success;   
}