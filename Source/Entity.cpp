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
    x_pos_ = x_pos;
    y_pos_ = y_pos;
    next_x_ = 0;
    next_y_ = 0;
    height_ = height;
    width_ = width;
    application_main_ = application;

    // Setup Box2D
    bodyDef_.type = b2_dynamicBody;
    bodyDef_.position.Set(0.0f, 4.0f);
    body_ = application_main_->world_.CreateBody(&bodyDef_);
    dynamicBox_.SetAsBox(1.0f, 1.0f);
    fixtureDef_.shape = &dynamicBox_;
    fixtureDef_.density = 1.0f;
    fixtureDef_.friction = 0.3f;
    body_->CreateFixture(&fixtureDef_);
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

// Set and get next x
void Entity::set_next_x(int new_x) {
    next_x_ = new_x;
}

int Entity::get_next_x() const {
    return next_x_;
}

void Entity::add_next_x(int add) {
    next_x_ += add;
}

void Entity::sub_next_x(int sub) {
    next_x_ -= sub;
}

// Set and get next y
void Entity::set_next_y(int new_y) {
    next_y_ = new_y;
}

int Entity::get_next_y() const {
    return next_y_;
}

void Entity::add_next_y(int add) {
    next_y_ += add;
}

void Entity::sub_next_y(int sub) {
    next_y_ -= sub;
}

// Set and get height
void Entity::set_height(int new_height) {
    height_ = new_height;
}

int Entity::get_height() const {
    return height_;
}

// Set and get width
void Entity::set_width(int new_width) {
    width_ = new_width;
}

int Entity::get_width() const {
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
void Entity::render() {
    texture_.render(x_pos_, y_pos_, NULL, 0.0, NULL, SDL_FLIP_NONE);
}

// Get application
Application* Entity::get_application() {
    return application_main_;
}

// Get world factor
const float Entity::get_world_factor() {
   return application_main_->get_world_factor();
}

// Update function for all entities. For now all it does is call move
void Entity::update() {
    move();
}

// Destructor
Entity::~Entity() {}

/******************** PLAYER IMPLEMENTATIONS ***************************/

// Initializ the player by calling it's constructor
Player::Player(Application* application) : 
    Entity(200, 600, 200, 100, application), init_vel_x_(20), init_vel_y_(20), init_vel_y_next_(20), final_vel_x_(0), final_vel_y_(0), gravity_(1.05), fall_(5), player_state_(FALL), player_directions_(NEUTRAL), num_jumps_(0) {
    mass_ = 0.0;

    // Set next y and x
    set_next_x(200);
    set_next_y(605);
}

// Movement logic of the player. Done through keyboard.
void Player::move() {
    // Deal with basic movement for now
    if (get_application()->current_key_states_[SDL_SCANCODE_RIGHT]) {
        // Update movement. Basic at this point but will make better
        add_x(5);
        
        // Set player direction to right
        player_directions_ = RIGHT;
    } if (get_application()->current_key_states_[SDL_SCANCODE_LEFT]) {
        // Update movement towards left. Same as above
        sub_x(5);

        // Set player direction to left
        player_directions_ = LEFT;
    } if (!has_jumped_ && get_application()->current_key_states_[SDL_SCANCODE_UP]) {
        if (!has_jumped_) {
            // Set player directions and states
            // For simple jump, directions is neutral and state is jump
            player_directions_ = NEUTRAL;
            player_state_ = JUMP;

            // Reset the final velocities of each component
            final_vel_y_ = 0;
            final_vel_x_ = 0;

            // Set the flags
            has_jumped_ = true;
            
            // Reset the timer
            jump_timer_.stop();

            // Increase num_jumps_
            num_jumps_ = 1;
        }
    } if (get_application()->current_key_states_[SDL_SCANCODE_DOWN]) {
        // Need to revisit this
        //add_y(5);
    }
}

// Virtual destructor
Player::~Player() {}
