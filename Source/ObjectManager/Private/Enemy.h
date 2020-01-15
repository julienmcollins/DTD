#ifndef ENEMY_H_
#define ENEMY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <string>
#include <unordered_map>

#include "Source/ObjectManager/Private/Entity.h"
#include "Source/ObjectManager/Private/Element.h"
#include "Source/ObjectManager/Private/Timer.h"
#include "Source/ObjectManager/Private/SDLTimer.h"

#include "Source/RenderingEngine/Private/Texture.h"

#include "Source/MathStructures/Private/Coordinates.h"

class Enemy : public Entity {
   public:
      // Construct the enemy
      Enemy(int x, int y, int width, int height);
      
      // Different enemy states
      enum STATE {
         IDLE,
         ATTACK,
         EXCRETE,
         RETREAT,
         HURT,
         TURN,
         FALL,
         DEATH
      };

      // Update function and get texture
      virtual void Update(bool freeze = false);   
      virtual Animation *GetAnimationFromState();
   
      // Static string
      static const std::string media_path;

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

      // Set enemy_state_
      void set_state(STATE state) {
         enemy_state_ = state;
      }

      // Within bounds function
      virtual bool within_bounds();

      // Create projectile function
      virtual Projectile* CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y, 
            bool owner, bool damage, float force_x, float force_y);

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
      Fecreez(int x, int y);

      // Load media
      virtual bool LoadMedia();

      // Update, move and animate functions
      virtual void Move();
      virtual void Animate(Texture *tex = NULL, int reset = 0, int max = 0);

      // Contact listener
      virtual void StartContact(Element *element = NULL);

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
      Arm(std::string name, int x, int y, int width, int height, Rosea *rosea);

      // Callback function will set enemy's state to HURT
      virtual void StartContact(Element *element = NULL);

      // Define abstract functions
      virtual void Move() {}
      virtual Animation *GetAnimationFromState();

      // Static position
      std::unordered_map<std::string, Space2D> static_positions;
   private:
      // Rosea parent class
      Rosea *rosea_;

      // Name of arm
      std::string name_;
};

class Rosea : public Enemy {
   public:
      // COnstructor for rosea
      Rosea(int x, int y, float angle);

      // Load Rosea media
      virtual bool LoadMedia();

      // Update, move and animate functions
      virtual void Update(bool freeze = false);
      virtual void Move();
      virtual void Animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);

      // Get texture for rosea
      virtual Animation *GetAnimationFromState();

      // Get contact
      virtual void StartContact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Rosea";
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

      // Texture for arms
      Texture *arm_sheet;

      // Model for the arm
      glm::mat4 arm_model;
      glm::mat4 attack_model;

      // Counter for hurt
      int hurt_counter_;

      // Arm state for when player is near
      int arm_state_;

      // Check to see if player is within bounds
      bool in_bounds_;

      // Dictionary linking box to frames
      std::unordered_map<int, int> arm_heights_;
      std::unordered_map<int, int> arm_widths_;

      // Angle of rosea
      float angle_;
};

class Mosquibler : public Enemy {
   public:
      // Constructor for mosquibler
      Mosquibler(int x, int y);

      // Load Rosea media
      virtual bool LoadMedia();

      // Update, move and animate functions
      virtual void Move();
      virtual void Animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);

      // Get texture for rosea
      virtual Animation *GetAnimationFromState();

      // Get contact
      virtual void StartContact(Element *element = NULL);
      virtual void EndContact(Element *element = NULL);

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
      Fruig(int x, int y);

      // Load fruig media
      virtual bool LoadMedia();

      // Update, move and animate functions
      virtual void Move();
      virtual void Animate(Texture *tex, int reset, int max, int start);

      // Get contact
      virtual void StartContact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Fruig";
      }
};

// Fleet Sensor
class FleetSensor : public Sensor {
   public:
      // Constructor
      FleetSensor(float width, float height, Entity *entity, CONTACT contact_type, float center_x, float center_y);

      // Contact functions
      virtual void StartContact(Element *element);
      virtual void EndContact(Element *element);
};

class Fleet : public Enemy {
   public:
      // Constructor
      Fleet(int x, int y);

      // Load fleet media
      virtual bool LoadMedia();

      // Move and animate functions
      virtual void Move();
      virtual void Animate(Texture *tex, int reset, int max, int start);

      // Get contact function
      virtual void StartContact(Element *element = NULL);

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
      Mosqueenbler(int x, int y);

      // Load fleet media
      virtual bool LoadMedia();

      // Move and animate functions
      virtual void Move();
      virtual void Animate(Texture *tex, int reset, int max, int start);

      // Get contact function
      //virtual void StartContact(Element *element = NULL);

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
      SDLTimer movement_timer_;

      // Spawn number of mosquiblers
      int spawn_num_of_egg_;
};

class MosquiblerEgg : public Enemy {
   public:
      // Constructor
      MosquiblerEgg(int x, int y);

      // Load wormored media
      virtual bool LoadMedia();

      // Move and animate functions
      virtual void Move();
      virtual void Animate(Texture *tex, int reset, int max, int start);

      // Start contact function
      virtual void StartContact(Element *element = NULL);

      // Wormored type
      virtual std::string type() {
         return "MosquiblerEgg";
      }
};

// Fleet Sensor
class WormoredSensor : public Sensor {
   public:
      // Constructor
      WormoredSensor(float height, float width, Entity *entity, CONTACT contact_type, float center_x, float center_y);

      // Contact functions
      virtual void StartContact(Element *element);
      virtual void EndContact(Element *element) {};

      // State type
      virtual std::string type() {
         return "Wormored";
      }
};

class Wormored : public Enemy {
   public:
      // Constructor
      Wormored(int x, int y);

      // Load wormored media
      virtual bool LoadMedia();

      // Move and animate functions
      virtual void Move();
      virtual void Animate(Texture *tex, int reset, int max, int start);
      virtual Texture *get_texture();

      // Wormored type
      virtual std::string type() {
         return "Wormored";
      }

      // Virtual destructor
      virtual ~Wormored();

   private:
      // List of sensors
      BodyPart *left_facing_sensors_[6];
      BodyPart *right_facing_sensors_[6];

      // Movement of body parts
      std::unordered_map<int, int> body_1_heights_;
      std::unordered_map<int, int> body_2_heights_;
      std::unordered_map<int, int> body_3_heights_;
      std::unordered_map<int, int> body_4_heights_;
      std::unordered_map<int, int> body_5_heights_;
      std::unordered_map<int, int> body_6_heights_;

      // curr and prev frame
      int curr_frame = 0;
      int prev_frame = 0;

};

#endif
