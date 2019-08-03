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

      // virtual load media
      virtual bool load_media();
      void load_image(std::unordered_map<std::string, Texture> &textures, Element *element, int w, int h, int frame_num, float fps, std::string name, std::string file, bool &success);

      // Set hitboxes
      void set_hitbox(int x, int y, bool dynamic = false, int height = 0, int width = 0);

      // Check to see if it's still alive
      virtual bool is_alive();
      bool alive;

      // Render function
      void texture_render(Texture *texture);
      void render(Texture *texture);

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

      /***** BOX2D Variables *********/

      b2Body *body;
      b2BodyDef body_def;
      b2PolygonShape box;
      b2FixtureDef fixture_def;

      /*******************************/

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

      // Get type of object
      virtual std::string type() {
         return "Element";
      }

      // Check if enemy
      virtual bool is_enemy() {
         return false;
      }

      // Apply force
      void apply_force(const b2Vec2 force) {
         body->ApplyForceToCenter(force, true);
      }

      // Start and end contact
      virtual void start_contact(Element *element = NULL) {}
      virtual void end_contact() {}

      // Contact flag
      // TODO: change in contact to in contact left and right
      bool in_contact;
      bool in_contact_down;

      // Element destructor
      virtual ~Element();
   protected:
      // Flag for various things
      bool flag_;
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

#endif
