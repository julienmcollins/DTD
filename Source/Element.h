#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <Box2D/Box2D.h>
#include "Texture.h"

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
      int get_x() const;
      int get_y() const;
      int get_height() const;
      int get_width() const;

      // Adders
      void add_x(int add);
      void sub_x(int sub);
      void add_y(int add);
      void sub_y(int add);

      // Check to see if it's still alive
      virtual bool is_alive();

      // Update function for all elements
      virtual void update();

      // Get application
      Application *get_application();

      // Texture for each element
      Texture texture;

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
