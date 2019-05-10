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
         RETREAT,
         HURT,
         TURN,
         DEATH
      };
      
      // Get type
      virtual std::string type() {
         return "Enemy";
      }

      // Is enemy is true
      virtual bool is_enemy() {
         return true;
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
      virtual void start_contact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Fecreez";
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
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);

      // Get texture for rosea
      virtual Texture *get_texture();

      // Get contact
      virtual void start_contact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Rosea";
      }

      // Destructor for rosea
      virtual ~Rosea();
   private:
      // Element for the arms idle/hurt
      Element arms_still;

      // Element for the arms attacking
      Element arms_attack;

      // Counter for hurt
      int hurt_counter_;

      // Arm state for when player is near
      int arm_state_;

      // Check to see if player is within bounds
      bool within_bounds();
      bool in_bounds_;

      // Checks to see if player is on left or right
      bool left_bound();
      bool right_bound();

      // Check to see if player is leaving the bounds
      bool is_leaving_bounds();

      // Dictionary linking box to frames
      std::unordered_map<int, int> arm_heights_;
};

class Mosquibler : public Enemy {
   public:
      // Constructor for mosquibler
      Mosquibler(int x, int y, Application *application);

      // Load Rosea media
      virtual bool load_media();

      // Update, move and animate functions
      virtual void update(bool freeze = false);
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);

      // Get texture for rosea
      virtual Texture *get_texture();

      // Get contact
      virtual void start_contact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Mosquibler";
      }

      // Destructor for rosea
      virtual ~Mosquibler();
   private:
      // Flag for death
      int start_death_;
      int end_death_;
};

#endif
