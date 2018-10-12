#include <iostream>
#include "Object.h"
#include "Texture.h"
#include "Entity.h"

/******************** Object Implementations **********************/

// Constructor
Object::Object(double x, double y, double height, double width) :
   x_pos_(x), y_pos_(y), height_(height), width_(width) {}

// Get and set x
double Object::get_x() const {
    return x_pos_;
}

void Object::set_x(double new_x) {
    x_pos_ = new_x;
}

// Get and set y
double Object::get_y() const {
    return y_pos_;
}

void Object::set_y(double new_y) {
    y_pos_ = new_y;
}

// Get and set height
double Object::get_height() const {
    return texture_.getHeight();
}

void Object::set_height(double new_height) {
    height_ = new_height;
}

// Get and set width
double Object::get_width() const {
    return texture_.getWidth();
}

void Object::set_width(double new_width) {
    width_ = new_width;
}

/***************** Platform Implementations *************************/

// Constructor
Platform::Platform(double x, double y) :
   Object(x, y, texture_.getHeight(), texture_.getWidth()) {}

// Default constructor
Platform::Platform() : 
    Object(0, 0, texture_.getHeight(), texture_.getWidth()) {}

// doNothing
void Platform::doNothing() {}
