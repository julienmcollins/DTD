#ifndef ENEMY_H_
#define ENEMY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <string>

#include "Entity.h"
#include "Texture.h"
#include "Element.h"

class Enemy : public Entity {
   public:
      // Construct the enemy
      Enemy(int x, int y, int height, int width, Application *application);
      
      // Different enemy states
      enum STATE {
         IDLE,
         SHOOT,
         DEATH
      };

      // Load media function will load everything enemy related
      virtual bool load_media() {}

      // Enemy's update function
      virtual void update(bool freeze = false) {}

      // Dummy move function
      virtual void move() {}

      // Animate function
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0) {}

      // Get texture for enemies
      virtual Texture *get_texture() {}

      // Contact listener
      virtual void start_contact() {
         health -= 10;
         if (health <= 0) {
            alive = false;
         }
      }
      
      // Get type
      virtual std::string type() {
         return "Enemy";
      }

      // Destructotr
      virtual ~Enemy() = 0;
   
   protected:
      STATE enemy_state_;
      int shoot_timer_;
};

class Fecreez : public Enemy {
   public:
      // Constructor for fecreez
      Fecreez(int x, int y, int height, int width, Application *application);

      // Load media
      virtual bool load_media();

      // Update, move and animate functions
      virtual void update(bool freeze = false);
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0);

      // Get texture
      virtual Texture *get_texture();

      // Destructor
      virtual ~Fecreez();
};

#endif
