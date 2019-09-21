#ifndef ENEMY_H_
#define ENEMY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <string>
#include <unordered_map>

#include "Entity.h"
#include "Texture.h"
#include "Element.h"
#include "Timer.h"

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
         FALL,
         DEATH
      };

      // Update function and get texture
      virtual void update(bool freeze = false);      
      virtual Texture *get_texture();

      // Get type
      virtual std::string type() {
         return "Enemy";
      }

      // Is enemy is true
      virtual bool is_enemy() {
         return true;
      }

      // Get the enemy state
      STATE get_enemy_state() const;

      // Within bounds function
      virtual bool within_bounds();

      // Create projectile function
      virtual Projectile* create_projectile(int delta_x_r, int delta_x_l, int delta_y, 
            bool owner, bool damage, float force_x, float force_y,
            const TextureData &nornal, const TextureData &hit);

      // Destructotr
      virtual ~Enemy() = 0;
   
   protected:
      // States and timers
      STATE enemy_state_;
      int shoot_timer_;

      // Flag for death
      int start_death_;
      int end_death_;

      // Anchor x and y
      int anchor_x;
      int anchor_y;
      
      // Projectile they own
      Projectile *proj_;
};

class Fecreez : public Enemy {
   public:
      // Constructor for fecreez
      Fecreez(int x, int y, Application *application);

      // Load media
      virtual bool load_media();

      // Update, move and animate functions
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0);

      // Contact listener
      virtual void start_contact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Fecreez";
      }

      // Destructor
      virtual ~Fecreez();
};

// ROsea prototype
class Rosea;

// ARM for rosea
class Arm : public Enemy {
   public:
      // Constructor for arm
      Arm(int x, int y, int height, int width, Application *application, Rosea *rosea);

      // Callback function will set enemy's state to HURT
      virtual void start_contact(Element *element = NULL);

      // Define abstract functions
      virtual void move() {}
      virtual Texture *get_texture() {
         return NULL;
      }
   private:
      // Rosea parent class
      Rosea *rosea_;
};

class Rosea : public Enemy {
   public:
      // COnstructor for rosea
      Rosea(int x, int y, float angle, Application *application);

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

      // Set enemy_state_
      void set_state(STATE state) {
         enemy_state_ = state;
      }

      // get enemy state
      STATE get_state() const {
         return enemy_state_;
      }

      // Within bounds function
      virtual bool within_bounds();

      // Destructor for rosea
      virtual ~Rosea();
   private:
      // Element for the arms idle/hurt
      Arm arms_still;

      // Element for the arms attacking
      Arm arms_attack;

      // Counter for hurt
      int hurt_counter_;

      // Arm state for when player is near
      int arm_state_;

      // Check to see if player is within bounds
      bool in_bounds_;

      // Checks to see if player is on left or right
      bool left_bound();
      bool right_bound();

      // Check to see if player is leaving the bounds
      bool is_leaving_bounds();

      // Dictionary linking box to frames
      std::unordered_map<int, int> arm_heights_;
      std::unordered_map<int, int> arm_widths_;

      // Angle of rosea
      float angle_;
};

class Mosquibler : public Enemy {
   public:
      // Constructor for mosquibler
      Mosquibler(int x, int y, Application *application);

      // Load Rosea media
      virtual bool load_media();

      // Update, move and animate functions
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);

      // Get texture for rosea
      virtual Texture *get_texture();

      // Get contact
      virtual void start_contact(Element *element = NULL);
      virtual void end_contact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Mosquibler";
      }

      // Destructor for rosea
      virtual ~Mosquibler();
};

class Fruig : public Enemy {
   public:
      // Constructor
      Fruig(int x, int y, Application *application);

      // Load fruig media
      virtual bool load_media();

      // Update, move and animate functions
      virtual void move();
      virtual void animate(Texture *tex, int reset, int max, int start);

      // Get contact
      virtual void start_contact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Fruig";
      }
};

// Fleet Sensor
class FleetSensor : public Sensor {
   public:
      // Constructor
      FleetSensor(float height, float width, Entity *entity, CONTACT contact_type, float center_x, float center_y);

      // Contact functions
      virtual void start_contact(Element *element);
      virtual void end_contact(Element *element);
};

class Fleet : public Enemy {
   public:
      // Constructor
      Fleet(int x, int y, Application *application);

      // Load fleet media
      virtual bool load_media();

      // Move and animate functions
      virtual void move();
      virtual void animate(Texture *tex, int reset, int max, int start);

      // Get contact function
      virtual void start_contact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Fleet";
      }
   private:
      FleetSensor *fleet_sensor_;
};

class Mosqueenbler : public Enemy {
   public:
      // Constructor
      Mosqueenbler(int x, int y, Application *application);

      // Load fleet media
      virtual bool load_media();

      // Move and animate functions
      virtual void move();
      virtual void animate(Texture *tex, int reset, int max, int start);

      // Get contact function
      //virtual void start_contact(Element *element = NULL);

      // Set is enemy false
      virtual bool is_enemy() {
         return false;
      }

      // Get type
      virtual std::string type() {
         return "Mosqueenbler";
      }
   private:
      // Timer for his movement
      Timer movement_timer_;

      // Spawn number of mosquiblers
      int spawn_num_of_egg_;
};

class MosquiblerEgg : public Enemy {
   public:
      // Constructor
      MosquiblerEgg(int x, int y, Application *application);

      // Load wormored media
      virtual bool load_media();

      // Move and animate functions
      virtual void move();
      virtual void animate(Texture *tex, int reset, int max, int start);

      // Start contact function
      virtual void start_contact(Element *element = NULL);

      // Wormored type
      virtual std::string type() {
         return "MosquiblerEgg";
      }
};

class Wormored : public Enemy {
   public:
      // Constructor
      Wormored(int x, int y, Application *application);

      // Load wormored media
      virtual bool load_media();

      // Move and animate functions
      virtual void move();
      virtual void animate(Texture *tex, int reset, int max, int start);

      // Wormored type
      virtual std::string type() {
         return "Wormored";
      }
};

#endif
