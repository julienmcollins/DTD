#include "Element.h"

// Constructor for element
Element::Element(int x, int y, double height, double width, Application *application) :
   texture(this) {

   // Set application
   application_ = application;

   // Set x and y
   x_pos_ = x;
   y_pos_ = y;

   // Set height and width
   height_ = height;
   width_ = width;
}

// Set and get x
void Element::set_x(int new_x) {
    x_pos_ = new_x;
}

int Element::get_x() const {
   return x_pos_;
}

// Add and sub x
void Element::add_x(int add) {
    x_pos_ += add;
}

void Element::sub_x(int sub) {
    x_pos_ -= sub;
}

// Set and get y
void Element::set_y(int new_y) {
    y_pos_ = new_y;
}

int Element::get_y() const {
   return y_pos_;
}

// Add and sub y
void Element::add_y(int add) {
    y_pos_ += add;
}

void Element::sub_y(int sub) {
    y_pos_ -= sub;
}

// Set and get height
void Element::set_height(double new_height) {
    height_ = new_height;
}

double Element::get_height() const {
   if (height_ == 0)
      return texture.getHeight();
   else
      return height_;
}

// Set and get width
void Element::set_width(double new_width) {
    width_ = new_width;
}

double Element::get_width() const {
   if (width_ == 0)
      return texture.getWidth();
   else
      return width_;
}

// Get application
Application *Element::get_application() {
   return application_;
}
