#include <iostream>
#include "Entity.h"
#include "Object.h"
#include "Texture.h"
#include "Application.h"

/******************** Object Implementations **********************/

// Constructor
Object::Object(int x, int y, int height, int width, Entity* owner, Application *application) :
   Element(x, y, height, width, application), owning_entity(owner) {}

/***************** Platform Implementations *************************/

// Constructor
Platform::Platform(int x, int y, Application *application) :
   Object(x, y, 0, 0, NULL, application) {

   // Set position
   float x_pos = x * application->to_meters_;
   float y_pos = -y * application->to_meters_;
   body_def.position.Set(x_pos, y_pos);
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
   body->CreateFixture(&box, 0.0f);

   // Set user data
   body->SetUserData(this);
}

// doNothing
//void Platform::doNothing() {}

/************** Projectile Implementations  *************************/

// Constructor
Projectile::Projectile(int x, int y, int height, int width,
      bool owner, bool damage, Entity *entity, Application *application) :
   Object(x, y, height, width, entity, application), 
   owner_(owner), damage_(damage) {

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
   body->CreateFixture(&fixture_def);

   // Give it an x direction impulse
   b2Vec2 force;
   if (owning_entity->entity_direction == RIGHT) {
      force = {10.4f, 0};
   } else {
      force = {-10.4f, 0};
   }
   body->ApplyForce(force, body->GetPosition(), true);
   
   // Now add it to the things the world needs to render
   tmp->getProjectileVector()->push_back(this);

   // Finally, set the user data
   body->SetUserData(this);
}

// Update function
void Projectile::update() {
   // Update projectile
   if (texture.frame_ > texture.max_frame_) {
      texture.frame_ = 0;
   }
   texture.curr_clip_ = &texture.clips_[texture.frame_];
   ++texture.frame_;

   // If enemy is shooting to the left, flip the texture
   if (shot_dir == LEFT) {
      texture.render(get_x(), get_y(), texture.curr_clip_, 0.0, NULL, SDL_FLIP_HORIZONTAL);
   } else {
      texture.render(get_x(), get_y(), texture.curr_clip_, 0.0, NULL, SDL_FLIP_NONE);
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
Projectile::~Projectile() {
   // Delete object
   body->GetWorld()->DestroyBody(body);
}
