#include "QuiteGoodMachine/Source/ObjectManager/Private/Element.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Timers/FPSTimer.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

// Constructor for element
Element::Element(std::string name, int x, int y, int width, int height) :
   Component(name),
   alive(true), body(NULL), flag_(false), element_shape(), 
   in_contact_down(false), in_contact_left(false), in_contact_right(false),
   texture_flipped(false) {

   // Set x and y
   x_pos_ = x;
   y_pos_ = y;

   // Set height and width
   height_ = height;
   width_ = width;

   // Set model and hitbox
   element_model = glm::mat4(1.0f);
   element_hitbox = glm::mat4(1.0f);

   // Set element model and hitbox
   element_model = glm::translate(element_model, glm::vec3(x, y, 0.0f));
   element_hitbox = glm::translate(element_hitbox, glm::vec3(x, y, 0.0f));
}

/********** SET AND GET HITBOX AND MODEL ***************/

void Element::set_hitbox_x(float x) {
   element_hitbox[3][0] = x / 100.0f;
   if (body) {
      body->SetTransform(b2Vec2(element_hitbox[3][0], element_hitbox[3][1]), hitbox_angle);
   }
}

void Element::set_hitbox_y(float y) {
   element_hitbox[3][1] = -(y / 100.0f);
   if (body) {
      body->SetTransform(b2Vec2(element_hitbox[3][0], element_hitbox[3][1]), hitbox_angle);
   }
}

void Element::set_model_x(float x) {
   element_model[3][0] = x;
}

void Element::set_model_y(float y) {
   element_model[3][1] = y;
}

float Element::get_hitbox_x() {
   element_hitbox[3][0] = body->GetPosition().x * 100.0f;
   return element_hitbox[3][0];
}

float Element::get_hitbox_y() {
   element_hitbox[3][1] = -(body->GetPosition().y * 100.0f);
   return element_hitbox[3][1];
}

float Element::get_model_x() {
   return element_model[3][0];
}

float Element::get_model_y() {
   return element_model[3][1];
}

/*******************************************************/

// Set and get x
void Element::set_x(int new_x) {
   if (!body) {
      x_pos_ = new_x;
   } else {
      float nbx = (float) (new_x / 100.0f);
      body->SetTransform(b2Vec2(nbx, body->GetPosition().y), body->GetAngle());
   }
}

float Element::get_x() {
   if (body) {
      x_pos_ = 100.0f * body->GetPosition().x;
      return x_pos_;
   } else {
      return x_pos_;
   }
}

float Element::get_tex_x() {
   if (body) {
      x_pos_ = (100.0f * body->GetPosition().x) - (get_width() / 2.0f);
      return x_pos_;
   } else {
      return x_pos_;
   }
}

void Element::sub_tex_x(int sub) {
   if (!body) {
      x_pos_ -= sub;
   } else {
      float subx = (float) ((sub + (GetAnimationFromState()->texture_width / 2.0f)) / 100.0f);
      body->SetTransform(b2Vec2(body->GetPosition().x - subx, body->GetPosition().y), body->GetAngle());
   }
}

// Set and get y
void Element::set_y(int new_y) {
   if (!body) {
      y_pos_ = new_y;
   } else {
      float nby = (float) -(new_y / 100.0f);
      body->SetTransform(b2Vec2(body->GetPosition().x, nby), body->GetAngle());
   }
}

float Element::get_y() {
   if (body) {
      y_pos_ = 100.0f * -body->GetPosition().y;
      return y_pos_;
   } else {
      return y_pos_;
   }
}

float Element::get_tex_y() {
   if (body) {
      y_pos_ = (100.0f * -body->GetPosition().y) - (get_height() / 2.0f);
      return y_pos_;
   } else {
      return y_pos_;
   }
}

float Element::get_height() const {
   if (height_ == 0)
      return 0.0f;
   else
      return height_;
}

float Element::get_width() const {
   if (width_ == 0)
      return 0.0f;
   else
      return width_;
}

float Element::get_anim_x() {
   if (body) {
      x_pos_ = (100.0f * body->GetPosition().x) - (GetAnimationFromState()->texture_width / 2.0f);
      return x_pos_;
   } else {
      return x_pos_;
   }
}

float Element::get_anim_y() {
   if (body) {
      y_pos_ = (100.0f * -body->GetPosition().y) - (GetAnimationFromState()->texture_height / 2.0f);
      return y_pos_;
   } else {
      return y_pos_;
   }
}

Animation *Element::GetAnimationByName(std::string name) {
   if (animations.find(name) != animations.end()) {
      return animations[name];
   }
   return nullptr;
}

void Element::FlipAllAnimations() {
   // Iterate through map and flip all animations
   for (std::unordered_map<std::string, Animation *>::iterator it = animations.begin(); it != animations.end(); ++it) {
      it->second->flipAnimation();
   }

   // Set all animations flipped to true
   texture_flipped = true;
}

bool Element::TextureFlipped() {
   return texture_flipped;
}

bool Element::AnimationCompleted(std::string name) {
   if (GetAnimationByName(name)->curr_frame > (GetAnimationByName(name)->max_frame - 1)) {
      // Reset animation
      GetAnimationByName(name)->completed = false;
      GetAnimationByName(name)->curr_frame = 0;
      return true;
   } else {
      return false;
   }
}

// Load media does nothing
bool Element::LoadMedia() {
   return true;
}

// Setup box2d
void Element::SetHitbox(int x, int y, SHAPE_TYPE type, int group) {
   // If dynamic is set, set body to dynamic
   if (element_shape.dynamic) {
      body_def.type = b2_dynamicBody;
   }

   // Set initial position and set fixed rotation
   float x_temp = float(x) * Application::GetInstance().to_meters_;
   float y_temp = -float(y) * Application::GetInstance().to_meters_;
   body_def.position.Set(x_temp, y_temp);
   body_def.fixedRotation = true;

   // Attach body to world
   body = Application::GetInstance().world_.CreateBody(&body_def);

   // Set box dimensions
   if (type == SQUARE) {
      int height = element_shape.shape_type.square.height;
      int width = element_shape.shape_type.square.width;
      int t_h = (height == 0) ? get_height() : height;
      int t_w = (width == 0) ? get_width() : width;
      float width_box = (t_w / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;
      float height_box = (t_h / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;
      box.SetAsBox(width_box, height_box, element_shape.center, element_shape.shape_type.square.angle);
      fixture_def.shape = &box;
   } else if (type == CIRCLE) {
      circle.m_radius = element_shape.shape_type.circle.radius;
      circle.m_p = element_shape.center;
      fixture_def.shape = &circle;
   }

   // Set various fixture definitions and create fixture
   fixture_def.density = element_shape.density;
   fixture_def.friction = 1.8f;
   fixture_def.userData = this;
   fixture_def.filter.categoryBits = CAT_ENEMY;
   fixture_def.filter.maskBits = CAT_PLAYER | CAT_PROJECTILE | CAT_PLATFORM | CAT_ENEMY | CAT_SENSOR;
   body->CreateFixture(&fixture_def);

   // Set user data so it can react
   body->SetUserData(this);
}

void Element::CreateHitbox(float x, float y) {
   body_def.type = b2_dynamicBody;
   body_def.position.Set(x, y);
   body_def.fixedRotation = true;
   body = Application::GetInstance().world_.CreateBody(&body_def);
}

void Element::SetCollision(uint16 collision_types, b2Fixture *fixture) {
   b2Filter filter;
   filter.categoryBits = body->GetFixtureList()->GetFilterData().categoryBits;
   filter.maskBits = collision_types;
   if (!fixture) {
      body->GetFixtureList()->SetFilterData(filter);
   } else {
      fixture->SetFilterData(filter);
   }
}

// Is alive function
bool Element::is_alive() {
   // Check to see if alive is false
   if (alive == false) {
      return alive;
   }

   // Out of bounds?
   if (get_x() < -200) {
      return false;
   } else if (get_x() > 2000) {
      return false;
   } else if (get_y() < -200) {
      return false;
   } else if (get_y() > 1155) {
      return false;
   }

   // Return true otherwise
   return true;
}

// Render function
void Element::Render(Texture *texture, int x, int y) {
   // Render based on texture
   int t_x = x == 0 ? get_tex_x() : x;
   int t_y = y == 0 ? get_tex_y() : y;
   // texture->Render(t_x, t_y, texture->curr_clip_);
}

// Move function (does nothing)
void Element::Move() {}

// Animate function
void Element::Animate(Texture *tex, int reset, int max, int start) {
   return;
}

// Update function for basic stuff just calls Render
void Element::Update(bool freeze) {
   // Simply Render the texture
   // texture.Render(get_tex_x(), get_tex_y());
}

// Destructor
Element::~Element() {
   // Destroy body
   if (body) {
      // TODO: fix this at some point?
      Application::GetInstance().world_.DestroyBody(body);
      body = nullptr;
   }

   // Free textures
   for (auto i = extra_sheets.begin(); i != extra_sheets.end(); i++) {
      if (i->second) {
         i->second->Free();
      }
   }

   // Free animations
   for (auto i = animations.begin(); i != animations.end(); i++) {
      delete i->second;
   }
}

/*************** SENSOR CLASS *************************/
Sensor::Sensor(float height, float width, Entity *entity, CONTACT contact_type, float center_x, float center_y, float density, bool set_as_body) :
   Element("", center_x, center_y, height, width), sensor_contact(contact_type), owner_(entity), density_(density) {}

void Sensor::initialize(float width, float height, float center_x, float center_y, uint16 category, uint16 mask, bool is_sensor) {
   // Create box shape
   box.SetAsBox(width, height, {center_x, center_y}, 0.0f);

   // Create fixture
   fixture_def.shape = &box;
   fixture_def.density = density_;
   fixture_def.isSensor = is_sensor;
   fixture_def.friction = 0.0f;
   fixture_def.userData = this;

   // Set filter
   filter.categoryBits = category;
   filter.maskBits = mask; //| CAT_PLAYER | CAT_PROJECTILE | CAT_ENEMY | CAT_BOSS;
   fixture_def.filter = filter;

   // Attach fixture
   fixture_ = owner_->body->CreateFixture(&fixture_def);
}

void Sensor::activate_sensor() {
   filter.maskBits = CAT_PLATFORM | CAT_PLAYER | CAT_PROJECTILE | CAT_ENEMY;
   fixture_->SetFilterData(filter);
}

void Sensor::deactivate_sensor() {
   filter.maskBits = 0;
   fixture_->SetFilterData(filter);
}

/*************************** BodyPart ************************************************/
BodyPart::BodyPart(Entity *owning_entity, float x_rel_to_owner, float y_rel_to_owner, float width, float height, bool is_fixture, uint16 category) :
   Sensor(height, width, owning_entity, CONTACT_UP, x_rel_to_owner, y_rel_to_owner, 0.0f), 
   x_rel(x_rel_to_owner), y_rel(y_rel_to_owner), is_fixture_(is_fixture) {

   // Set defaults
   owner_ = owning_entity;

   // If it's not a fixture, initialize
   if (!is_fixture) {
      initialize(width, height, 0.0f, 0.0f, category);
   }
}

void BodyPart::initialize(float width, float height, float center_x, float center_y, uint16 category) {
   type_ = owner_->type();

   // Set x and y positions relative to owner
   set_x(owner_->get_x() + x_rel);
   set_y(owner_->get_y() + x_rel);

   // Create the main body
   // TODO: Add custom shapes other than squares as well
   body_def.type = b2_dynamicBody;

   float x = (float) get_x() / 100.0f;
   float y = (float) get_y() / 100.0f;

   if (!is_fixture_) {
      body_def.position.Set(x, y);
      body_def.fixedRotation = true;
      body = Application::GetInstance().world_.CreateBody(&body_def);
   }

   box.SetAsBox(width / 200.0f, height / 200.0f, b2Vec2(0.0f, 0.0f), 0.0f);
   fixture_def.shape = &box;
   fixture_def.density = 1000.0f;
   fixture_def.friction = 0.0f;
   fixture_def.userData = this;

   if (!is_fixture_) {
      fixture_ = body->CreateFixture(&fixture_def);
      body->SetUserData(this);
   } else {
      fixture_ = owner_->body->CreateFixture(&fixture_def);
   }

   // Set filter
   b2Filter filter;
   filter.groupIndex = 0;
   filter.categoryBits = category;
   filter.maskBits = CAT_ENEMY | CAT_PROJECTILE | CAT_PLATFORM | CAT_PLAYER;
   fixture_->SetFilterData(filter);
}

void BodyPart::Update(int x_offset, int y_offset) {
   set_x(owner_->get_x() + x_rel + x_offset);
}