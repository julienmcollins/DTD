#include <stdio.h>
#include <cmath>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "Box2D/Box2D.h"
#include <iostream>

#include "Entity.h"
#include "Texture.h"
#include "Application.h"
#include "Timer.h"

/*************************** ENTITY IMPLEMENTATIONS ******************************/

// Entity constructor which will provide basic establishment for all entities
Entity::Entity(int x_pos, int y_pos, int height, int width, Application* application) : 
    has_jumped_(false), has_collided_(false) {
    x_pos_ = x_pos;
    y_pos_ = y_pos;
    height_ = height;
    width_ = width;
    application_main_ = application;
}

// Set and get x
void Entity::set_x(int new_x) {
    x_pos_ = new_x;
}

int Entity::get_x() {
    return x_pos_;
}

// Set and get y
void Entity::set_y(int new_y) {
    y_pos_ = new_y;
}

int Entity::get_y() {
    return y_pos_;
}

// Set and get height
void Entity::set_height(int new_height) {
    height_ = new_height;
}

int Entity::get_height() {
    return height_;
}

// Set and get width
void Entity::set_width(int new_width) {
    width_ = new_width;
}

int Entity::get_width() {
    return width_;
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

/******************** PLAYER IMPLEMENTATIONS ***************************/

// Initializ the player by calling it's constructor
Player::Player(Application* application) : 
    Entity(200, 200, 200, 100, application), mass_(0.0), init_vel_x_(20), init_vel_y_(20), final_vel_x_(0), final_vel_y_(0), has_jumped_(false), has_collided_(false), gravity_(1.05), fall_(5), player_state_(FALL), player_directions_(NEUTRAL) {}

// Gravity for the player
void Player::fall() {
    // Apply gravity
    get_y() += fall_;

    if (fall_ < 18.81) {
        // Increase fall speed
        fall_ *= gravity_;
    }
}

// Jumping physics
void Player::jump() {
    // Stop the jump
    if ((get_y() + height_) >= application_main_->get_ground().get_y() && !has_jumped_) {
        player_state_ = FALL;
    }

    // Start timer
    if (!jump_timer_.isStarted()) {
        jump_timer_.start();
    }

    // Start jumping physics
    final_vel_y_ = init_vel_y_ + ((-gravity_) * (jump_timer_.getTicks() / 20)); 

    // Add velocity to position
    get_y() -= final_vel_y_;
    
    // Add positional velocity
    if (player_directions_ == RIGHT) {
        final_vel_x_ = 2;
        get_x() += final_vel_x_;
    } else if (player_directions_ == LEFT) {
        final_vel_x_ = -2;
        get_x() += final_vel_x_;
    }
}

/*
// Collision physics
void Player::collide() {
    // Reset height
    if ((y_pos_ + height_) >= application_main_->get_ground().get_y()) {
        y_pos_ = application_main_->get_ground().get_y() - height_;
        has_collided_ = true;
    }

    if (has_collided_) {
        // Reset jump
        has_jumped_ = false;
        has_collided_ = false;
    }
}
*/

// Collide function (nothing for now)
void collide(Entity* entity) {}

// Movement logic of the player. Done through keyboard.
void Player::move() {
    // Deal with basic movement for now
    if (get_application()->current_key_states_[SDL_SCANCODE_RIGHT]) {
        // Update movement. Basic at this point but will make better
        get_x() += 5;

        // Set player direction to right
        player_directions_ = RIGHT;
    } if (get_application()->current_key_states_[SDL_SCANCODE_LEFT]) {
        // Update movement towards left. Same as above
        get_x() -= 5;

        // Set player direction to left
        player_directions_ = LEFT;
    } if (!has_jumped_ && get_application()->current_key_states_[SDL_SCANCODE_UP]) {
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
    } if (get_application()->current_key_states_[SDL_SCANCODE_DOWN]) {
        // Need to revisit this
        y_pos_ += 5;
    }
}

// Get the state of the player
void Player::get_state() {
    switch(player_state_) {
        // Jumping case
        case JUMP:
            jump();
            break;
        // Falling case
        case FALL:
            fall();
            break;
        // Shooting case
        case SHOOT:
            //shoot();
            break;
     }
}
