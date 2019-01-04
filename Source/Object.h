#ifndef OBJECT_H_
#define OBJECT_H_

#include <Box2D/Box2D.h>
#include "Texture.h"
#include "Element.h"

class Entity;

// A parent class because I might add other objects in the future
class Object : public Element {
    public:
        // Constructor
        Object(int x, int y, double height, double width, Application *application);

        // Might need to add a function that gets texture
        // Pure virtual function to make this abstract base class
        //virtual void doNothing() = 0;
};

// Platform class will be a subclass of Object (through inheritance)
class Platform : public Object {
    public:
        // Constructor
        Platform(int x, int y, Application *application);

        // Other functions
        //virtual void doNothing();
};

// Projectile class is also a subclass of object
class Projectile : public Object {
   public:
      // Constructor
      Projectile(int x, int y, bool owner, bool damage, Application *application);

      // doNothing function
      //virtual void doNothing();

      // Get owner
      bool get_owner() const;

      // Get damage amount
      int get_damage() const;

   private:
      // Owner flag: 1 if belongs to player, 0 if enemy
      bool owner_;

      // Damage amount
      int damage_;
};

#endif
