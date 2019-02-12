#include "Element.h"

// Constructor for element
Element::Element(int x, int y, int height, int width, Application *application) :
   texture(this, 0), body(NULL) {

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
   if (body) {
      //printf("x = %f\n", body->GetPosition().x);
      return (int) ((100.0f * body->GetPosition().x) - (get_width() / 2.0f));
   } else {
      return x_pos_;
   }
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
   if (body) {
      //printf("y = %f\n", (100.0f * body->GetPosition().y) - (get_height() / 2.0f));
      return (int) ((100.0f * -body->GetPosition().y) - (get_height() / 2.0f));
   } else {
      return x_pos_;
   }
}

// Add and sub y
void Element::add_y(int add) {
    y_pos_ += add;
}

void Element::sub_y(int sub) {
    y_pos_ -= sub;
}

// Set and get height
void Element::set_height(int new_height) {
    height_ = new_height;
}

int Element::get_height() const {
   if (height_ == 0)
      return texture.getHeight();
   else
      return height_;
}

// Set and get width
void Element::set_width(int new_width) {
    width_ = new_width;
}

int Element::get_width() const {
   if (width_ == 0)
      return texture.getWidth();
   else
      return width_;
}

// Is alive function
bool Element::is_alive() {
   // Out of bounds?
   if (get_x() < 0) {
      return false;
   } else if (get_x() > 1900) {
      return false;
   } else if (get_y() < 0) {
      return false;
   } else if (get_y() > 1055) {
      return false;
   }

   // Return true otherwise
   return true;
}

// Update function for basic stuff just calls render
void Element::update() {
   // Simply render the texture
   texture.render(get_x(), get_y());
}

// Get application
Application *Element::get_application() {
   return application_;
}
