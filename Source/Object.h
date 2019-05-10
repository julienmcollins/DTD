#ifndef OBJECT_H_
#define OBJECT_H_

#include <Box2D/Box2D.h>
#include <string>
#include <iostream>
#include "Texture.h"
#include "Element.h"

class Entity;
class Application;

// A parent class because I might add other objects in the future
class Object : public Element {
   public:
      // Constructor
      Object(int x, int y, int height, int width, Entity* owner, Application *application);

      // Might need to add a function that gets texture
      
      // Need to have a handle on entity that owns this object
      Entity *owning_entity;

      // Get type
      virtual std::string type() {
         return "Object";
      }
};

// Platform class will be a subclass of Object (through inheritance)
class Platform : public Object {
   public:
      // Constructor
      Platform(int x, int y, int height, int width, Application *application);

      // Setup function for the stuff
      void setup();

      // Other functions
      //virtual void doNothing();

      // Collision listener
      virtual void start_contact(Element *element) {}

      // Get type
      virtual std::string type() {
         return "Platform";
      }

      // Virtual destructor
      virtual ~Platform();
};

// Projectile class is also a subclass of object
class Projectile : public Object {
   public:
      // Constructor
      Projectile(int x, int y, int height, int width,
            bool owner, bool damage, Entity *entity, Application *application);

      // doNothing function
      //virtual void doNothing();

      // Set contact listener
      virtual void start_contact(Element *element) {
         alive = false;
      }

      // End contact listener
      virtual void end_contact() {
         alive = false;
      }

      // Update functions will just animate them
      virtual void update();

      // Get owner
      bool get_owner() const;

      // Get damage amount
      int get_damage() const;

      // Shot direction
      int shot_dir;

      // Get type
      virtual std::string type() {
         return "Projectile";
      }

      // Destructor
      virtual ~Projectile();

   private:
      // Owner flag: 1 if belongs to player, 0 if enemy
      bool owner_;

      // Damage amount
      int damage_;
};

#endif
