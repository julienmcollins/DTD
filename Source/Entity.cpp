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

    for (int i = 0; i < 4; i++) {
        collision_sides[i] = false;
    }
    collision_sides[4] = true;

    // Set next y and x
    set_next_x(200);
    set_next_y(605);
}

// Gravity for the player
/*
void Player::fall() {
    // Apply gravity
    add_y(fall_);

    // Testing this method
    
    if (collision_sides[3]) {
        sub_y(fall_);
        collision_sides[3] = false;
    }
    if (collision_sides[1] && collided_platform_) {
        set_x(collided_platform_->get_x() - get_width() + 15);
        collision_sides[1] = false;
    }
    if (collision_sides[0] && collided_platform_) {
        set_x(collided_platform_->get_x() + collided_platform_->get_width() - 15);
        collision_sides[0] = false;
    }
    

    // Increase fall
    if (fall_ < 18.81) {
        // Increase next_y_
        add_next_y(fall_ * gravity_);

        // Increase fall speed
        fall_ *= gravity_;
    } else {
        add_next_y(fall_);
    }
}
*/

// Jumping physics
/*
void Player::jump() {
    // Start timer
    if (!jump_timer_.isStarted()) {
        jump_timer_.start();
    }

    // Start jumping physics
    final_vel_y_ = init_vel_y_ + ((-gravity_) * (jump_timer_.getTicks() / 20)); 
    final_vel_y_next_ = final_vel_y_ * 1.1;

    // Add velocity to position
    sub_y(final_vel_y_);
    sub_next_y(final_vel_y_next_);

    // Testing this method
    if (collision_sides[2]) {
        std::cout << "Collided with bottom of platform\n";
        //add_y(final_vel_y_);
        player_state_ = FALL;
        collision_sides[2] = false;
    }
    if (collision_sides[3] && collided_platform_) {
        std::cout << "Collided with top of platform\n";
        set_y(collided_platform_->get_y() - get_height());
        has_jumped_ = false;
        player_state_ = FALL;
        collision_sides[3] = false;
    }

    // Add positional velocity
    if (player_directions_ == RIGHT) {
        final_vel_x_ = 2;
        add_x(final_vel_x_);
        add_next_x(final_vel_x_ * 2);
    } else if (player_directions_ == LEFT) {
        final_vel_x_ = -2;
        add_x(final_vel_x_);
        add_next_x(final_vel_x_ * 2);
    }

    // Check collide
    //check_collide();
}
*/

// Check collide function
/*
bool Player::check_collide() {
    // Check collision
    if (collision_sides[3] == true && collided_platform_) {
        // Collision with top of platform
        std::cout << "In collision of top!\n";
        set_y(collided_platform_->get_y() - get_height());
        //set_next_y(collided_platform_->get_y() - get_height() + 20);
        has_jumped_ = false;
        player_state_ = FALL;
        collision_sides[3] = false;
        return true;
    } else if (collision_sides[2] == true && collided_platform_) {
        // Collision with bottom of platform
        std::cout << "In collision of bottom!\n";
        // The problem I'm having is that the jump continues even though I set the height
        // which means that it'll keep going up so I basically need to set the state to fall
        // which also means I have to fix the issues I have with fall
        set_y(collided_platform_->get_y() + collided_platform_->get_height());
        //final_vel_y_ = -29 + ((-gravity_) * (jump_timer_.getTicks() / 25)); 
        //sub_y(final_vel_y_);
        //fall_ = 6;
        player_state_ = FALL;
        collision_sides[2] = false;
        return true;
    } else if (collision_sides[1] == true && collided_platform_) {
        // Collision with left side of platform
        std::cout << "In collision of left!\n";
        set_x(collided_platform_->get_x() - get_width() + 15);
        collision_sides[1] = false;
        return true;
    } else if (collision_sides[0] == true && collided_platform_) {
        // Collision with right side of platform
        std::cout << "In collision of right!\n";
        set_x(collided_platform_->get_x() + collided_platform_->get_width() - 15);
        collision_sides[0] = false;
        return true;
    }

    return false;
}
*/

// Movement logic of the player. Done through keyboard.
void Player::move() {
    // Check collision
    //check_collide();

    // Deal with basic movement for now
    if (get_application()->current_key_states_[SDL_SCANCODE_RIGHT]) {
        // Update movement. Basic at this point but will make better
        add_x(5);
        
        /*
        // Testing this method
        if (collision_sides[1]) {
            sub_x(5);
            collision_sides[1] = false;
        }
        */

        // Set player direction to right
        player_directions_ = RIGHT;
    } if (get_application()->current_key_states_[SDL_SCANCODE_LEFT]) {
        // Update movement towards left. Same as above
        sub_x(5);

        /*
        // Testing this method
        if (collision_sides[0]) {
            add_x(5);
            collision_sides[0] = false;
        }
        */

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

            // Reset collision flag
            //collision_sides[3] = false;

            // Increase num_jumps_
            num_jumps_ = 1;
        }
    } if (get_application()->current_key_states_[SDL_SCANCODE_DOWN]) {
        // Need to revisit this
        //add_y(5);
    }

    // Check collision
    //check_collide();
}

// Get the state of the player
void Player::get_state() {
    switch(player_state_) {
        // Jumping case
        case JUMP:
            //std::cout << "In state jump\n";
            //jump();
            break;
        // Falling case
        case FALL:
            //std::cout << "In state fall\n";
            //fall();
            break;
        // Shooting case
        case SHOOT:
            //shoot();
            break;
     }
}

// Virtual destructor
Player::~Player() {}
