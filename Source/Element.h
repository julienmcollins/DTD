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
      Element(int x, int y, double height, double width, Application *application);

      // Setters
      void set_x(int new_x);
      void set_y(int new_y);
      void set_height(double new_height);
      void set_width(double new_width);

      // Getters
      int get_x() const;
      int get_y() const;
      double get_height() const;
      double get_width() const;

      // Adders
      void add_x(int add);
      void sub_x(int sub);
      void add_y(int add);
      void sub_y(int add);

      // Get application
      Application *get_application();

      // Texture for each element
      Texture texture_;

      /***** BOX2D Variables *********/

      b2Body *body;
      b2BodyDef body_def;
      b2PolygonShape box;

      /*******************************/

   private:
      // X and Y locations
      int x_pos_;
      int y_pos_;

      // Height and widths
      double height_;
      double width_;

      // Application pointer
      Application *application_;
};

#endif
