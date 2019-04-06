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

      // Check to see if it's still alive
      virtual bool is_alive();
      bool alive;

      // Render function
      void render(Texture *texture, SDL_Rect *clip);

      // Update function for all elements
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0);
      virtual void update();

      // Draw function if immediate drawing is desired
      void draw(Texture *tex = NULL, int reset = 0);

      // Texture related stuff
      virtual Texture *get_texture();
      SDL_Rect *get_curr_clip();

      // Get application
      Application *get_application();

      // Texture for each element
      Texture texture;

      // Hash map of texture names to textures
      std::unordered_map<std::string, Texture> textures;

      // Fps timer for animations
      Timer fps_timer;
      float last_frame;
      float fps;

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

      // Get type of object
      virtual std::string type() {
         return "Element";
      }

      // Start and end contact
      virtual void start_contact() {}
      virtual void end_contact() {}

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
