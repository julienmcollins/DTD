#include "Element.h"
#include "Timer.h"

// Constructor for element
Element::Element(int x, int y, int height, int width, Application *application) :
   alive(true), texture(this, 0), last_frame(0), fps(0), body(NULL) {

   // Set application
   application_ = application;

   // Set x and y
   x_pos_ = x;
   y_pos_ = y;

   // Set height and width
   height_ = height;
   width_ = width;

   // Start fps timer
   fps_timer.start();
}

// Set and get x
void Element::set_x(int new_x) {
    x_pos_ = new_x;
}

int Element::get_x() {
   if (body) {
      //printf("x = %f\n", body->GetPosition().x);
      x_pos_ = (int) ((100.0f * body->GetPosition().x) - (get_width() / 2.0f));
      return x_pos_;
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

int Element::get_y() {
   if (body) {
      //printf("y = %f\n", (100.0f * body->GetPosition().y) - (get_height() / 2.0f));
      y_pos_ = (int) ((100.0f * -body->GetPosition().y) - (get_height() / 2.0f));
      return y_pos_;
   } else {
      return y_pos_;
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

// Move function (does nothing)
void Element::move() {}

// Animate function
void Element::animate(Texture *tex, int reset) {
   // Tmp pointer
   Texture *curr;
   if (tex)
      curr = tex;
   else
      curr = &texture;

   // Modulate fps
   last_frame += (fps_timer.getDeltaTime() / 1000.0f);
   if (last_frame > fps) {
      if (curr->frame_ > curr->max_frame_) {
         curr->frame_ = reset;
      }
      curr->curr_clip_ = &curr->clips_[curr->frame_];
      ++curr->frame_;
      last_frame = 0.0f;
   }
}

// Draw function for immediate drawing
void Element::draw(Texture *tex, int reset) {
   // Call animate function
   animate(tex, reset);

   // Render
   if (tex)
      tex->render(tex->get_x(), tex->get_y(), tex->curr_clip_, 0.0,
            &tex->center_, tex->flip_);
   else
      texture.render(texture.get_x(), texture.get_y(), texture.curr_clip_, 0.0,
            &texture.center_, texture.flip_);
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
