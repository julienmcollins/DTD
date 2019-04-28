#ifndef ENEMY_H_
#define ENEMY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <string>
#include <unordered_map>

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
         ATTACK,
         HURT,
         DEATH
      };
      
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
      Fecreez(int x, int y, Application *application);

      // Load media
      virtual bool load_media();

      // Update, move and animate functions
      virtual void update(bool freeze = false);
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0);

      // Get texture
      virtual Texture *get_texture();

      // Contact listener
      virtual void start_contact() {
         health -= 10;
         if (health <= 0) {
            alive = false;
         }
      }

      // Destructor
      virtual ~Fecreez();
   private:
      // Dumb flag for now, find a better way later
      bool shift_;
};

class Rosea : public Enemy {
   public:
      // COnstructor for rosea
      Rosea(int x, int y, Application *application);

      // Load Rosea media
      virtual bool load_media();

      // Update, move and animate functions
      virtual void update(bool freeze = false);
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0);

      // Get texture for rosea
      virtual Texture *get_texture();

      // Get contact
      virtual void start_contact();

      // Destructor for rosea
      virtual ~Rosea();
   private:
      // Element for the arms idle/hurt
      Element arms_still;

      // Element for the arms attacking
      Element arms_attack;

      // Counter for hurt
      int hurt_counter_;

      // Dictionary linking box to frames
      std::unordered_map<int, int> arm_heights_;
};

#endif
