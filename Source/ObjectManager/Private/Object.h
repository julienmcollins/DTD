#ifndef OBJECT_H_
#define OBJECT_H_

#include <Box2D/Box2D.h>
#include <string>
#include <iostream>

#include "Source/ObjectManager/Private/Element.h"

#include "Source/RenderingEngine/Private/Texture.h"

class Entity;

// A parent class because I might add other objects in the future
class Object : public Element {
   public:
      // Constructor
      Object(int x, int y, int width, int height, Entity* owner);

      // Might need to add a function that gets texture
      
      // Need to have a handle on entity that owns this object
      Entity *owning_entity;

      // States for the shot
      enum STATE {
         ALIVE,
         DEAD
      };

      // Get curr state
      STATE get_state() {
         return object_state_;
      }

      // Get type
      virtual std::string type() {
         return "Object";
      }

      // TEMPORARY FIX
      bool shift;
   protected:
      STATE object_state_;
};

// Platform class will be a subclass of Object (through inheritance)
class Platform : public Object {
   public:
      // Constructor
      Platform(int x, int y, int width, int height);

      // Setup function for the stuff
      void setup();

      // Other functions
      //virtual void doNothing();

      // Collision listener
      virtual void StartContact(Element *element) {}

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
      Projectile(std::string name, int x, int y, float width, float height, bool owner, int damage, 
            float force_x, float force_y, Entity *entity);

      // doNothing function
      //virtual void doNothing();

      // Get animation by state
      virtual Animation *GetAnimationFromState();

      // Load master sprite sheet
      virtual bool LoadMedia();

      // Set contact listener
      virtual void StartContact(Element *element);

      // End contact listener
      virtual void EndContact() {
         alive = false;
      }

      // Update functions will just animate them
      virtual void update();

      // Get owner
      bool get_owner() const;

      // Get damage amount
      int get_damage() const;

      // Get name of projectile
      std::string name;

      // Shot direction
      int shot_dir;

      // Get type
      virtual std::string type() {
         return "Projectile";
      }

      // Destructor
      virtual ~Projectile();

   protected:
      // Owner flag: 1 if belongs to player, 0 if enemy
      bool owner_;

      // Damage amount
      int damage_;
};

#endif
