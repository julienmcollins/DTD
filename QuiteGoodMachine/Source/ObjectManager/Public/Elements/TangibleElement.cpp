#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/TangibleElement.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include <string>
#include <Box2D/Box2D.h>

#include "OpenGLIncludes.h"

TangibleElement::TangibleElement(std::string name, glm::vec3 initial_position, glm::vec3 size)
   : PositionalElement(name, initial_position, size)
   , current_direction_(NEUTRAL) 
{
   // Initial
   hitbox_model_ = glm::mat4(1.0f);

   // Translate over
   hitbox_model_ = glm::translate(hitbox_model_, initial_position);
}

void TangibleElement::Update(bool freeze) {
   // As of now, this only calls the move functions
   Move();

   // Set position relative to body
   SetPosition(glm::vec3(body_->GetPosition().x * 100.f, body_->GetPosition().y * -100.f, 0.f));
}

void TangibleElement::SetCollision(uint16 collision_types, b2Fixture *fixture) {
   b2Filter filter;
   filter.categoryBits = body_->GetFixtureList()->GetFilterData().categoryBits;
   filter.maskBits = collision_types;
   if (!fixture) {
      body_->GetFixtureList()->SetFilterData(filter);
   } else {
      fixture->SetFilterData(filter);
   }
}

void TangibleElement::SetHitbox(float x, float y, SHAPE_TYPE type, int group) {
   // If dynamic is set, set body to dynamic
   if (element_shape_.dynamic) {
      body_def_.type = b2_dynamicBody;
   }

   // Set initial position and set fixed rotation
   float x_temp = float(x) * Application::GetInstance().to_meters_;
   float y_temp = -float(y) * Application::GetInstance().to_meters_;
   body_def_.position.Set(x_temp, y_temp);
   body_def_.fixedRotation = true;

   // Attach body to world
   body_ = Application::GetInstance().world_.CreateBody(&body_def_);

   // Set shape_ dimensions
   if (type == SQUARE) {
      int height = element_shape_.shape_type.square.height;
      int width = element_shape_.shape_type.square.width;
      int t_h = (height == 0) ? GetSize().y : height;
      int t_w = (width == 0) ? GetSize().x : width;
      float width_box = (t_w / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;
      float height_box = (t_h / 2.0f) * Application::GetInstance().to_meters_ - 0.02f;
      shape_.SetAsBox(width_box, height_box, element_shape_.center, element_shape_.shape_type.square.angle);
      fixture_def_.shape = &shape_;
   } else if (type == CIRCLE) {
      circle_.m_radius = element_shape_.shape_type.circle.radius;
      circle_.m_p = element_shape_.center;
      fixture_def_.shape = &circle_;
   }

   // Set various fixture definitions and create fixture
   fixture_def_.density = element_shape_.density;
   fixture_def_.friction = 1.8f;
   fixture_def_.userData = this;
   fixture_def_.filter.categoryBits = CAT_ENEMY;
   fixture_def_.filter.maskBits = CAT_PLAYER | CAT_PROJECTILE | CAT_PLATFORM | CAT_ENEMY | CAT_SENSOR;
   body_->CreateFixture(&fixture_def_);

   // Set user data so it can react
   body_->SetUserData(this);
}

void TangibleElement::CreateHitbox(float x, float y) {
   body_def_.type = b2_dynamicBody;
   body_def_.position.Set(x, y);
   body_def_.fixedRotation = true;
   body_ = Application::GetInstance().world_.CreateBody(&body_def_);
}

void TangibleElement::Move() {}

uint8 TangibleElement::CheckCurrentContact() {
   return contacts_;
}

void TangibleElement::SetCurrentContact(uint8 contact) {
   contacts_ |= contact;
}

std::string TangibleElement::GetType() {
   return "TangibleElement";
}

TangibleElement::~TangibleElement() {}