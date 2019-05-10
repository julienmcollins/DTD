#include "Element.h"
#include "Application.h"
#include "Timer.h"

// Constructor for element
Element::Element(int x, int y, int height, int width, Application *application) :
   alive(true), texture(this, 0), body(NULL) {

   // Set application
   application_ = application;

   // Set x and y
   x_pos_ = x;
   y_pos_ = y;

   // Set height and width
   height_ = height;
   width_ = width;

   // Start fps timer
   //fps_timer.start();
}

// Set and get x
void Element::set_x(int new_x) {
   if (!body) {
      x_pos_ = new_x;
   } else {
      float nbx = (float) ((new_x + (get_width() / 2.0f)) / 100.0f);
      body->SetTransform(b2Vec2(nbx, body->GetPosition().y), body->GetAngle());
   }
}

int Element::get_x() {
   if (body) {
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
   if (!body) {
      y_pos_ = new_y;
   } else {
      float nby = (float) -((new_y + (get_height() / 2.0f)) / 100.0f);
      body->SetTransform(b2Vec2(body->GetPosition().x, nby), body->GetAngle());
   }
}

int Element::get_y() {
   if (body) {
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

// Load media does nothing
bool Element::load_media() {
   return true;
}

// Loads an image
void Element::load_image(std::unordered_map<std::string, Texture> &textures, Element *element, int w, int h, int frame_num, float fps, std::string name, std::string file, bool &success) {
   textures.emplace(name, Texture(element, frame_num - 1, fps));
   if (!textures[name].loadFromFile(file)) {
      std::cerr << "Failed to load " << file << std::endl;
      success = false;
   } else {
      // Allocate space
      textures[name].clips_ = new SDL_Rect[frame_num];
      SDL_Rect *temp = textures[name].clips_;

      // Set sprites
      for (int i = 0; i < frame_num; i++) {
         temp[i].x = i * w;
         temp[i].y = 0;
         temp[i].w = w;
         temp[i].h = h;
      }

      // Set to 0
      textures[name].curr_clip_ = &temp[0];
   }
}

// Setup box2d
void Element::set_hitbox(int x, int y, bool dynamic, int height, int width) {
   // If dynamic is set, set body to dynamic
   if (dynamic) {
      body_def.type = b2_dynamicBody;
   }

   // Set initial position and set fixed rotation
   float x_temp = float(x) * get_application()->to_meters_;
   float y_temp = -float(y) * get_application()->to_meters_;
   body_def.position.Set(x_temp, y_temp);
   body_def.fixedRotation = true;

   // Attach body to world
   body = get_application()->world_.CreateBody(&body_def);

   // Set box dimensions
   int t_h = (height == 0) ? get_height() : height;
   int t_w = (width == 0) ? get_width() : width;
   float width_box = (t_w / 2.0f) * get_application()->to_meters_ - 0.02f;
   float height_box = (t_h / 2.0f) * get_application()->to_meters_ - 0.02f;
   box.SetAsBox(width_box, height_box);

   // Set various fixture definitions and create fixture
   fixture_def.shape = &box;
   fixture_def.density = 1.0f;
   fixture_def.friction = 1.8f;
   body->CreateFixture(&fixture_def);

   // Set user data so it can react
   body->SetUserData(this);

   // Run the load media function
   if (load_media() == false) {
      std::cout << "In here! " << type() << std::endl;
      get_application()->set_quit();
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

// Render solely based on texture
void Element::texture_render(Texture *texture) {
   texture->render(texture->get_x(), texture->get_y(), texture->curr_clip_, 0.0, 
         &texture->center_, texture->flip_);
}

// Render function
void Element::render(Texture *texture) {
   // Render based on texture
   texture->render(get_x(), get_y(), texture->curr_clip_, 0.0, &texture->center_, texture->flip_);
}

// Move function (does nothing)
void Element::move() {}

// Animate function
void Element::animate(Texture *tex, int reset, int max, int start) {
   // Tmp pointer
   Texture *curr;
   if (tex) {
      curr = tex;
   } else {
      curr = &texture;
   }

   // Change max depending on which one is specified
   int temp_max = (max == 0) ? curr->max_frame_ : max;
   int temp_start = (start == 0) ? 0 : start;

   // Modulate fps
   curr->last_frame += (curr->fps_timer.getDeltaTime() / 1000.0f);
   if (curr->last_frame > curr->fps) {
      if (curr->frame_ > temp_max) {
         curr->frame_ = reset;
         curr->completed_ = true; // ISSUE: it will mark completed based on reset frame not max frame
      } else if (curr->frame_ <= reset + 1) {
         curr->completed_ = false;
      }
      curr->curr_clip_ = &curr->clips_[curr->frame_];
      ++curr->frame_;
      curr->last_frame = 0.0f;
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

// get texture
Texture *Element::get_texture() {
   return &texture;
}

// Get current clip
SDL_Rect *Element::get_curr_clip() {
   Texture *tmp = get_texture();
   return tmp->curr_clip_;
}

// Update function for basic stuff just calls render
void Element::update(bool freeze) {
   // Simply render the texture
   texture.render(get_x(), get_y());
}

// Get application
Application *Element::get_application() {
   return application_;
}
