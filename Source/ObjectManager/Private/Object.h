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
      Object(int x, int y, int height, int width, Entity* owner);

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
      Platform(int x, int y, int height, int width);

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
      Projectile(int x, int y, bool owner, int damage, 
            float force_x, float force_y,
            const TextureData &normal, const TextureData &hit,
            Entity *entity);

      // doNothing function
      //virtual void doNothing();

      // Load master sprite sheet
      virtual bool LoadMedia();

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

   protected:
      // Owner flag: 1 if belongs to player, 0 if enemy
      bool owner_;

      // Damage amount
      int damage_;

      // Frame counts
      TextureData normal_;
      TextureData hit_;
};

class Eraser : public Projectile {
   public:
      // Constructor
      Eraser(int x, int y, const TextureData &normal, const TextureData &hit,
         Entity *entity);
      
      // Start contact function
      virtual void start_contact(Element *element);

      // Load images
      virtual bool LoadMedia();
};

class EnemyProjectile : public Projectile {
   public:
      // Constructor
      EnemyProjectile(int x, int y, int damage,
         float force_x, float force_y,
         const TextureData &normal, const TextureData &hit,
         Entity *entity);

      // Start contact function
      virtual void start_contact(Element *element);

      // Load images
      virtual bool LoadMedia();

      // Change type
      virtual std::string type() {
         return "EnemyProjectile";
      }
};

#endif
