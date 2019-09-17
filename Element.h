#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <Box2D/Box2D.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "Texture.h"
#include "Timer.h"

// Dependencies
class Application;
class Entity;

// Shape struct
typedef struct Shape {
   union ShapeType {
      ShapeType(int w, int h, float a) : square{.width = w, .height = h, .angle = a} {}
      ShapeType(float a) : circle{.radius = a} {}
      struct Square {
         int width = 0;
         int height = 0;
         float angle = 0.0f;
      } square;
      struct Circle {
         float radius = 0.0f;
      } circle;
   } shape_type;

   // Flag for movement enabled shape
   bool dynamic = false;

   // Specify the center of the shape
   b2Vec2 center {0.0f, 0.0f};

   // Density of the shape
   float density = 1.0f;

   Shape() : shape_type(0, 0, 0.0f) {}
} Shape;

// This will be the base class for all elements in the game, including players and objects
class Element {
   public:
      // Constructor
      Element(int x, int y, int height, int width, Application *application);

      // Setters
      void set_x(int new_x);
      void set_y(int new_y);
      void set_height(int new_height);
      void set_width(int new_width);

      // Getters
      int get_x();
      int get_y();
      int get_height() const;
      int get_width() const;

      // Adders
      void add_x(int add);
      void sub_x(int sub);
      void add_y(int add);
      void sub_y(int add);

      // Enum for directions
      enum DIRS {
         LEFT,
         RIGHT,
         NEUTRAL
      };

      // ENUM for contact type
      enum CONTACT {
         CONTACT_LEFT,
         CONTACT_RIGHT,
         CONTACT_UP,
         CONTACT_DOWN
      };

      /***** BOX2D Variables *********/

      b2Body *body;
      b2BodyDef body_def;
      b2PolygonShape box;
      b2CircleShape circle;
      b2FixtureDef fixture_def;

      enum COLLISIONS {
         CAT_PLAYER = 0x0001,
         CAT_PLATFORM = 0x0002,
         CAT_PROJECTILE = 0x0004,
         CAT_ENEMY = 0x0008
      };

      enum ANTI_COLLISIONS {
         ACAT_PLAYER = 0xFFFE,
         ACAT_PLATFORM = 0xFFFD,
         ACAT_PROJECTILE = 0xFFFB,
         ACAT_ENEMY = 0xFFF7
      };

      // Set collisions
      void set_collision(uint16 collision_types);

      /*******************************/

      /****** CUSTOM SHAPE STUFF *****/

      Shape element_shape;

      enum SHAPE_TYPE {
         SQUARE,
         CIRCLE
      };

      /*******************************/

      // virtual load media
      virtual bool load_media();
      void load_image(std::unordered_map<std::string, Texture> &textures, Element *element, int w, int h, int frame_num, float fps, std::string name, std::string file, bool &success);

      // Set hitboxes
      void set_hitbox(int x, int y, SHAPE_TYPE type = SQUARE, int group = -1);

      // Check to see if it's still alive
      virtual bool is_alive();
      bool alive;

      // Render function
      void texture_render(Texture *texture);
      void render(Texture *texture, int x = 0, int y = 0);

      // Update function for all elements
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);
      virtual void update(bool freeze = false);

      // Draw function if immediate drawing is desired
      void draw(Texture *tex = NULL, int reset = 0);

      // Texture related stuff
      virtual Texture *get_texture();
      SDL_Rect *get_curr_clip();

      // Texture for each element
      Texture texture;

      // Hash map of texture names to textures
      std::unordered_map<std::string, Texture> textures;

      // Get application
      Application *get_application();

      // Get type of object
      virtual std::string type() {
         return "Element";
      }

      // Check if enemy
      virtual bool is_enemy() {
         return false;
      }

      // Check if marked for destroy
      bool is_marked_for_destroy() const {
         return mark_for_immediate_destroy_;
      }

      // Apply force
      void apply_force(const b2Vec2 force) {
         body->ApplyForceToCenter(force, true);
      }

      // Start and end contact
      virtual void start_contact(Element *element = NULL) {}
      virtual void end_contact(Element *element = NULL) {}

      // Contact flag
      // TODO: change in contact to in contact left and right
      bool in_contact;
      bool in_contact_left;
      bool in_contact_right;
      bool in_contact_down;

      // Element destructor
      virtual ~Element();
   protected:
      // Flag for various things
      bool flag_;

      // Flag for destroying object immediately
      bool mark_for_immediate_destroy_;
   private:
      // X and Y locations
      int x_pos_;
      int y_pos_;

      // Height and widths
      int height_;
      int width_;

      // Application pointer
      Application *application_;
};


// Sensor class
class Sensor : public Element {
   public:
      // Construct the sensor object
      Sensor(float height, float width, Entity *entity, CONTACT contact_type, float center_x = 0.0f, float center_y = 0.0f, float density = 0.0f);

      // Start contact function
      virtual void start_contact(Element *element = NULL) {};
      virtual void end_contact(Element *element = NULL) {};

      // Change type
      virtual std::string type() {
         return "Sensor";
      }

      // Only one contact at a time
      CONTACT sensor_contact;
   protected:
      Entity *entity_;
      b2CircleShape circle_;
};

#endif
