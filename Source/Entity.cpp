#include <stdio.h>
#include <cmath>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <Box2D/Box2D.h>

#include "Entity.h"
#include "Object.h"
#include "Texture.h"
#include "Application.h"
#include "Timer.h"

/*************************** ENTITY IMPLEMENTATIONS ******************************/

// Entity constructor which will provide basic establishment for all entities
Entity::Entity(int x_pos, int y_pos, int height, int width, Application* application) : 
    has_jumped_(false), center_x_(0.0), center_y_(0.0) {
    height_ = height;
    width_ = width;
    application_main_ = application;
}

// Set and get x
void Entity::set_x(int new_x) {
    x_pos_ = new_x;
}

int Entity::get_x() const {
    return x_pos_;
}

void Entity::add_x(int add) {
    x_pos_ += add;
}

void Entity::sub_x(int sub) {
    x_pos_ -= sub;
}

// Set and get y
void Entity::set_y(int new_y) {
    y_pos_ = new_y;
}

int Entity::get_y() const {
    return y_pos_;
}

void Entity::add_y(int add) {
    y_pos_ += add;
}

void Entity::sub_y(int sub) {
    y_pos_ -= sub;
}

// Set and get height
void Entity::set_height(int new_height) {
    height_ = new_height;
}

float Entity::get_height() const {
    return height_;
}

// Set and get width
void Entity::set_width(int new_width) {
    width_ = new_width;
}

float Entity::get_width() const {
    return width_;
}

// Set and get centers
double Entity::get_center_x() {
    center_x_ = (get_width() / 2) + get_x();
    return center_x_;
}

void Entity::set_center_x(double new_center_x) {
    center_x_ = new_center_x;
}

double Entity::get_center_y() {
    center_y_ = (get_height() / 2) + get_y();
    return center_y_;
}

void Entity::set_center_y(double new_center_y) {
    center_y_ = new_center_y;
}

// Rendering function for all entities
void Entity::render(Texture *texture, SDL_Rect *clip) {
    // NEED TO TAKE INTO ACCOUNT THAT BOTTOM OF IMAGE ISN'T ALLIGNED WITH FEET
    set_x((100.0f * body_->GetPosition().x) - (get_width() / 2.0f));
    set_y((100.0f * -body_->GetPosition().y) - (get_height() / 2.0f));
    texture->render((x_pos_), (y_pos_), clip);
}

// Get application
Application* Entity::get_application() {
    return application_main_;
}

/*
// Get world factor
const float Entity::get_world_factor() {
   return application_main_->get_world_factor();
}
*/

// Update function for all entities. For now all it does is call move
void Entity::update() {
    move();
}

// Destructor
Entity::~Entity() {}

/******************** PLAYER IMPLEMENTATIONS ***************************/

// Initializ the player by calling it's constructor
Player::Player(Application* application) : 
    Entity(960, 512, 140, 45, application), player_state_(FALL), player_directions_(NEUTRAL) {
    // Setup Box2D
    bodyDef_.type = b2_dynamicBody;
    float x = 960.0f * application->to_meters_;
    float y = -512.5f * application->to_meters_;
    bodyDef_.position.Set(x, y);
    bodyDef_.fixedRotation = true;
    //bodyDef_.linearDamping = 0.8f;
    body_ = get_application()->world_.CreateBody(&bodyDef_);
    dynamicBox_.SetAsBox((get_width() / 2) * application->to_meters_ - 0.01f, 
                         (get_height() / 2) * application->to_meters_ - 0.01f);
    //printf("%f %f\n", get_width() / 200.0f - 0.01f, get_height() / 200.0f - 0.01f);
    fixtureDef_.shape = &dynamicBox_;
    fixtureDef_.density = 1.0f;
    fixtureDef_.friction = 0.75f;
    body_->CreateFixture(&fixtureDef_);
}

// Movement logic of the player. Done through keyboard.
void Player::move() {
   // Deal with basic movement for now
   if (get_application()->current_key_states_[SDL_SCANCODE_RIGHT]) {
      // Use Box2D version for moving
      const b2Vec2 force = {3.4f, 0};
      body_->ApplyForce(force, body_->GetPosition(), true);

   } if (get_application()->current_key_states_[SDL_SCANCODE_LEFT]) {
      // Use Box2D version for moving
      const b2Vec2 force = {-3.4f, 0};
      body_->ApplyForce(force, body_->GetPosition(), true);

   } if (get_application()->current_key_states_[SDL_SCANCODE_UP]) {
      if (!has_jumped_) {
         // Apply an impulse
         const b2Vec2 force = {0, 8.0f};
         body_->ApplyLinearImpulse(force, body_->GetPosition(), true);

         // Set the flags
         has_jumped_ = true;
      } else {
         if (body_->GetLinearVelocity().y == 0) {
            has_jumped_ = false;
         }
      }
   } if (get_application()->current_key_states_[SDL_SCANCODE_DOWN]) {
     // Need to revisit this
     //add_y(5);
   }
}

// Virtual destructor
Player::~Player() {}
