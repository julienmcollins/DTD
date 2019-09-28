#include "Element.h"
#include "Entity.h"
#include "Application.h"
#include "Timer.h"

// Constructor for element
Element::Element(int x, int y, int height, int width, Application *application) :
   alive(true), texture(this, 0), body(NULL), flag_(false), element_shape(), 
   in_contact_down(false), in_contact_left(false), in_contact_right(false) {

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
      float nbx = (float) (new_x / 100.0f);
      body->SetTransform(b2Vec2(nbx, body->GetPosition().y), body->GetAngle());
   }
}

void Element::set_tex_x(int new_x) {
   if (!body) {
      x_pos_ = new_x;
   } else {
      float nbx = (float) ((new_x + (get_width() / 2.0f)) / 100.0f);
      body->SetTransform(b2Vec2(nbx, body->GetPosition().y), body->GetAngle());
   }
}

float Element::get_x() {
   if (body) {
      x_pos_ = 100.0f * body->GetPosition().x;
      return x_pos_;
   } else {
      return x_pos_;
   }
}

float Element::get_tex_x() {
   if (body) {
      x_pos_ = (100.0f * body->GetPosition().x) - (get_width() / 2.0f);
      return x_pos_;
   } else {
      return x_pos_;
   }
}

// Add and sub x
void Element::add_x(int add) {
   if (!body) {
      x_pos_ += add;
   } else {
      float addx = (float) (add / 100.0f);
      //std::cout << "pos y before = " << body->GetPosition().y << std::endl;
      body->SetTransform(b2Vec2(body->GetPosition().x, body->GetPosition().y + addx), body->GetAngle());
      //std::cout << "pos y after = " << body->GetPosition().y << std::endl;
   }}

void Element::sub_x(int sub) {
   if (!body) {
      x_pos_ -= sub;
   } else {
      float subx = (float) (sub / 100.0f);
      body->SetTransform(b2Vec2(body->GetPosition().x - subx, body->GetPosition().y), body->GetAngle());
   }
}

void Element::sub_tex_x(int sub) {
   if (!body) {
      x_pos_ -= sub;
   } else {
      float subx = (float) ((sub + (get_width() / 2.0f)) / 100.0f);
      body->SetTransform(b2Vec2(body->GetPosition().x - subx, body->GetPosition().y), body->GetAngle());
   }
}

// Set and get y
void Element::set_y(int new_y) {
   if (!body) {
      y_pos_ = new_y;
   } else {
      float nby = (float) -(new_y / 100.0f);
      body->SetTransform(b2Vec2(body->GetPosition().x, nby), body->GetAngle());
   }
}

void Element::set_tex_y(int new_y) {
   if (!body) {
      y_pos_ = new_y;
   } else {
      float nby = (float) -((new_y + (get_height() / 2.0f)) / 100.0f);
      body->SetTransform(b2Vec2(body->GetPosition().x, nby), body->GetAngle());
   }
}

float Element::get_y() {
   if (body) {
      y_pos_ = 100.0f * -body->GetPosition().y;
      return y_pos_;
   } else {
      return y_pos_;
   }
}

float Element::get_tex_y() {
   if (body) {
      y_pos_ = (100.0f * -body->GetPosition().y) - (get_height() / 2.0f);
      return y_pos_;
   } else {
      return y_pos_;
   }
}

// Add and sub y
void Element::add_y(int add) {
   if (!body) {
      y_pos_ += add;
   } else {
      float addy = (float) (add / 100.0f);
      std::cout << "pos y before = " << body->GetPosition().y << std::endl;
      body->SetTransform(b2Vec2(body->GetPosition().x, body->GetPosition().y - addy), body->GetAngle());
      std::cout << "pos y after = " << body->GetPosition().y << std::endl;
   }
}

void Element::sub_y(int sub) {
    y_pos_ -= sub;
}

// Set and get height
void Element::set_height(int new_height) {
    height_ = new_height;
}

float Element::get_height() const {
   if (height_ == 0)
      return texture.getHeight();
   else
      return height_;
}

// Set and get width
void Element::set_width(int new_width) {
    width_ = new_width;
}

float Element::get_width() const {
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
void Element::load_image(int w, int h, int frame_num, float fps, std::string name, std::string file, bool &success, int rows) {
   textures.emplace(name, Texture(this, frame_num - 1, fps));
   if (!textures[name].loadFromFile(file)) {
      std::cerr << "Failed to load " << file << std::endl;
      success = false;
   } else {
      // Allocate space
      textures[name].clips_ = new SDL_Rect[frame_num];
      SDL_Rect *temp = textures[name].clips_;

      int columns = (frame_num + rows - 1) / rows;

      // Set sprites
      int k = 0;
      for (int i = 0; i < rows; i++) {
         for (int j = 0; j < columns; j++) {
            if (k == frame_num) {
               break;
            }
            temp[k].x = j * w;
            temp[k].y = i * h;
            temp[k].w = w;
            temp[k].h = h;
            k++;
         }
         if (k == frame_num) {
            break;
         }
      }

      // Set to 0
      textures[name].curr_clip_ = &temp[0];
   }
}

// Setup box2d
void Element::set_hitbox(int x, int y, SHAPE_TYPE type, int group) {
   // If dynamic is set, set body to dynamic
   if (element_shape.dynamic) {
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
   if (type == SQUARE) {
      int height = element_shape.shape_type.square.height;
      int width = element_shape.shape_type.square.width;
      int t_h = (height == 0) ? get_height() : height;
      int t_w = (width == 0) ? get_width() : width;
      float width_box = (t_w / 2.0f) * get_application()->to_meters_ - 0.02f;
      float height_box = (t_h / 2.0f) * get_application()->to_meters_ - 0.02f;
      box.SetAsBox(width_box, height_box, element_shape.center, element_shape.shape_type.square.angle);
      fixture_def.shape = &box;
   } else if (type == CIRCLE) {
      circle.m_radius = element_shape.shape_type.circle.radius;
      circle.m_p = element_shape.center;
      fixture_def.shape = &circle;
   }

   // Set various fixture definitions and create fixture
   fixture_def.density = element_shape.density;
   fixture_def.friction = 1.8f;
   fixture_def.userData = this;
   body->CreateFixture(&fixture_def);

   // Set user data so it can react
   body->SetUserData(this);

   // Set filter
   b2Filter filter;
   filter.groupIndex = 0;
   filter.categoryBits = CAT_ENEMY;
   filter.maskBits = CAT_ENEMY | CAT_PROJECTILE | CAT_PLATFORM | CAT_PLAYER;
   body->GetFixtureList()->SetFilterData(filter);

   // Run the load media function
   if (load_media() == false) {
      std::cout << "Quit in here! " << this->type() << std::endl;
      get_application()->set_quit();
   }
}

void Element::create_hitbox(float x, float y) {
   body_def.type = b2_dynamicBody;
   body_def.position.Set(x, y);
   body_def.fixedRotation = true;
   body = get_application()->world_.CreateBody(&body_def);
}

void Element::set_collision(uint16 collision_types, b2Fixture *fixture) {
   b2Filter filter;
   filter.categoryBits = body->GetFixtureList()->GetFilterData().categoryBits;
   filter.maskBits = collision_types;
   if (!fixture) {
      body->GetFixtureList()->SetFilterData(filter);
   } else {
      fixture->SetFilterData(filter);
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
   texture->render(texture->get_x(), texture->get_y(), texture->curr_clip_, texture->angle, 
         &texture->center_, texture->flip_);
}

// Render function
void Element::render(Texture *texture, int x, int y) {
   // Render based on texture
   int t_x = x == 0 ? get_tex_x() : x;
   int t_y = y == 0 ? get_tex_y() : y;
   texture->render(t_x, t_y, texture->curr_clip_, texture->angle, &texture->center_, texture->flip_);
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
   texture.render(get_tex_x(), get_tex_y());
}

// Get application
Application *Element::get_application() {
   return application_;
}

// Destructor
Element::~Element() {
   if (body) {
      application_->world_.DestroyBody(body);
      body = nullptr;
   }

   // Free textures
   for (auto i = textures.begin(); i != textures.end(); i++) {
      i->second.free();
   }

   // Free texture
   texture.free();
}

/*************** SENSOR CLASS *************************/
Sensor::Sensor(float height, float width, Entity *entity, CONTACT contact_type, float center_x, float center_y, float density, bool set_as_body) :
   Element(center_x, center_y, height, width, nullptr), sensor_contact(contact_type), owner_(entity), density_(density) {}

void Sensor::initialize(float width, float height, float center_x, float center_y, uint16 category) {
   // Set application
   application_ = owner_->get_application();

   // Create box shape
   box.SetAsBox(width, height, {center_x, center_y}, 0.0f);

   // Create fixture
   fixture_def.shape = &box;
   fixture_def.density = density_;
   fixture_def.friction = 0.0f;
   fixture_def.userData = this;

   // Set filter
   b2Filter filter;
   filter.categoryBits = category;
   filter.maskBits = CAT_PLATFORM | CAT_PLAYER | CAT_PROJECTILE | CAT_ENEMY | CAT_BOSS;
   fixture_def.filter = filter;

   // Attach fixture
   fixture_ = owner_->body->CreateFixture(&fixture_def);
}

void Sensor::activate_sensor() {
   b2Filter filter;
   filter.categoryBits = CAT_SENSOR;
   filter.maskBits = CAT_PLATFORM | CAT_PLAYER | CAT_PROJECTILE;
   fixture_->SetFilterData(filter);
}

void Sensor::deactivate_sensor() {
   b2Filter filter;
   filter.categoryBits = CAT_SENSOR;
   filter.maskBits = CAT_PLATFORM;
   fixture_->SetFilterData(filter);
}

/*************************** BodyPart ************************************************/
BodyPart::BodyPart(Entity *owning_entity, float x_rel_to_owner, float y_rel_to_owner, float width, float height, Application *application, bool is_fixture, uint16 category) :
   Sensor(height, width, owning_entity, CONTACT_UP, x_rel_to_owner, y_rel_to_owner, 0.0f), 
   x_rel(x_rel_to_owner), y_rel(y_rel_to_owner), is_fixture_(is_fixture) {

   // Set defaults
   owner_ = owning_entity;

   // If it's not a fixture, initialize
   if (!is_fixture) {
      initialize(width, height, 0.0f, 0.0f, category);
   }
}

void BodyPart::initialize(float width, float height, float center_x, float center_y, uint16 category) {
   application_ = owner_->get_application();
   type_ = owner_->type();

   // Set x and y positions relative to owner
   set_x(owner_->get_x() + x_rel);
   set_y(owner_->get_y() + x_rel);

   // Create the main body
   // TODO: Add custom shapes other than squares as well
   body_def.type = b2_dynamicBody;

   float x = (float) get_x() / 100.0f;
   float y = (float) get_y() / 100.0f;

   if (!is_fixture_) {
      body_def.position.Set(x, y);
      body_def.fixedRotation = true;
      body = get_application()->world_.CreateBody(&body_def);
   }

   box.SetAsBox(width / 200.0f, height / 200.0f, b2Vec2(0.0f, 0.0f), 0.0f);
   fixture_def.shape = &box;
   fixture_def.density = 1000.0f;
   fixture_def.friction = 0.0f;
   fixture_def.userData = this;

   if (!is_fixture_) {
      fixture_ = body->CreateFixture(&fixture_def);
      body->SetUserData(this);
   } else {
      fixture_ = owner_->body->CreateFixture(&fixture_def);
   }

   // Set filter
   b2Filter filter;
   filter.groupIndex = 0;
   filter.categoryBits = category;
   filter.maskBits = CAT_ENEMY | CAT_PROJECTILE | CAT_PLATFORM | CAT_PLAYER;
   fixture_->SetFilterData(filter);
}

void BodyPart::update(int x_offset, int y_offset) {
   set_x(owner_->get_x() + x_rel + x_offset);
   // set_y(owner_->get_y() + y_rel + y_offset);
   // set_y(get_y() + y_offset);

   //add_x(x_offset);
   // std::cout << "y_offset = " << y_offset << std::endl;
   // std::cout << "get_y() before = " << get_y() << std::endl;
   //add_y(y_offset);
   // std::cout << "get_y() after = " << get_y() << std::endl;
   //std::cout << "res = " << owner_->get_y() + y_rel + y_offset << std::endl;
}