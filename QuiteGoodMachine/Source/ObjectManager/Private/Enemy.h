#ifndef ENEMY_H_
#define ENEMY_H_

#if 1

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <string>
#include <unordered_map>

#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Element.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Timers/FPSTimer.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Timers/SDLTimer.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Timers/SecondsTimer.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include "QuiteGoodMachine/Source/MathStructures/Private/Coordinates.h"

#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"

#include "QuiteGoodMachine/Source/GameManager/Private/DoOnce.h"

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/EnemyStates.h"

class Correspondence;
class Animation;

class Enemy : public Entity {
   public:
      // Construct the enemy
      Enemy(std::string name, glm::vec3 initial_position, glm::vec3 size);

      // Update function and get texture
      virtual void Update(bool freeze = false);   

      // Get type
      virtual std::string GetType() {
         return "Enemy";
      }

      // Is enemy is true
      virtual bool is_enemy() {
         return true;
      }

      // Get enemy state
      std::shared_ptr<EnemyState> GetCurrentEnemyState();

      // Within bounds function
      virtual bool within_bounds();

      // Create projectile function
      virtual Projectile* CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y, 
            bool owner, bool damage, float force_x, float force_y);

      // Flag for damage
      bool was_hurt;
      bool hit_ground;
      
      // States and timers
      int shoot_timer_;

      // Destructotr
      virtual ~Enemy();
   
   protected:
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
      Fecreez(std::string name, glm::vec3 initial_position);

      // Load media
      virtual void LoadMedia();

      // Update, move and animate functions
      virtual void Move();

      // Contact listener
      virtual void StartContact(Element *element = NULL);

      // Get type
      virtual std::string GetType() {
         return "Fecreez";
      }

      // Destructor
      virtual ~Fecreez();

   private:
      // Do once shift
      DoOnce do_shift_once_;

};

// ROsea prototype
class Rosea;

// ARM for rosea
class Arm : public Enemy {
   public:
      // Constructor for arm
      Arm(std::string name, glm::vec3 initial_position, glm::vec3 size, Rosea *rosea);

      // Friend class Rosea
      friend class Rosea;

      // Callback function will set enemy's state to HURT
      virtual void StartContact(Element *element = NULL);

      // Define abstract functions
      virtual void Move() {}

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
      Rosea(std::string name, glm::vec3 initial_position, glm::vec3 size, float angle);

      // Load Rosea media
      virtual void LoadMedia();

      // Update, move and animate functions
      virtual void Update(bool freeze = false);
      virtual void Move();

      // Get contact
      virtual void StartContact(Element *element = NULL);

      // Get type
      virtual std::string GetType() {
         return "Rosea";
      }

      // Within bounds function
      virtual bool within_bounds();

      // Destructor for rosea
      virtual ~Rosea();

   private:
      // Element for the arms idle/hurt
      Arm arms_;

      // // Element for the arms attacking
      // Arm arms_;

      // // Model for the arm
      // glm::mat4 arm_model;
      // glm::mat4 attack_model;

      // Dictionary linking box to frames
      std::unordered_map<int, int> arm_heights_;
      std::unordered_map<int, int> arm_widths_;

      // Angle of rosea
      float angle_;
};

class Mosquibler : public Enemy {
   public:
      // Constructor for mosquibler
      Mosquibler(std::string name, glm::vec3 initial_position);

      // Load Rosea media
      virtual void LoadMedia();

      // Update, move and animate functions
      virtual void Move();

      // Get contact
      virtual void StartContact(Element *element = NULL);
      virtual void EndContact(Element *element = NULL);

      // Get type
      virtual std::string GetType() {
         return "Mosquibler";
      }

      // Destructor for rosea
      virtual ~Mosquibler();
};

class Fruig : public Enemy {
   public:
      // Constructor
      Fruig(std::string name, glm::vec3 initial_position);

      // Load fruig media
      virtual void LoadMedia();

      // Update, move and animate functions
      virtual void Move();

      // Get contact
      virtual void StartContact(Element *element = NULL);

      // Get type
      virtual std::string GetType() {
         return "Fruig";
      }
};

class Fleet;

// Fleet Sensor
class FleetSensor : public Sensor {
   public:
      // Constructor
      FleetSensor(float width, float height, Entity *entity, Element::CONTACT contact_type, float center_x, float center_y);

      // Friend
      friend class Fleet;

      // Contact functions
      virtual void StartContact(Element *element);
      virtual void EndContact(Element *element);
};

class Fleet : public Enemy {
   public:
      // Constructor
      Fleet(std::string name, glm::vec3 initial_position);

      // Load fleet media
      virtual void LoadMedia();

      // Move and animate functions
      virtual void Move();

      // Get contact function
      virtual void StartContact(Element *element = NULL);

      // Get type
      virtual std::string GetType() {
         return "Fleet";
      }
   private:
      FleetSensor *fleet_sensor_;
};

class Mosqueenbler : public Enemy {
   public:
      // Constructor
      Mosqueenbler(std::string name, glm::vec3 initial_position);

      // Load fleet media
      virtual void LoadMedia();

      // Move and animate functions
      virtual void Move();

      // Set is enemy false
      virtual bool is_enemy() {
         return false;
      }

      // Get type
      virtual std::string GetType() {
         return "Mosqueenbler";
      }
   private:
      // FPSTimer for his movement
      SDLTimer movement_timer_;

      // Spawn number of mosquiblers
      int spawn_num_of_egg_;
};

class MosquiblerEgg : public Enemy,
                      public std::enable_shared_from_this<MosquiblerEgg> {
   public:
      // Constructor
      MosquiblerEgg(std::string name, glm::vec3 initial_position);

      // Load wormored media
      virtual void LoadMedia();

      // Move and animate functions
      virtual void Move();

      // Start contact function
      virtual void StartContact(Element *element = NULL);

      // Wormored type
      virtual std::string GetType() {
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
      virtual std::string GetType() {
         return "Wormored";
      }
};

class Wormored : public Enemy, 
                 public Correspondent {
   public:      
      // Constructor
      Wormored(std::string name, glm::vec3 initial_position);

      // Load wormored media
      virtual void LoadMedia();

      // Move and animate functions
      virtual void Update(bool freeze = false);
      virtual void Move();

      /**
       * Virtual process correspondent function
       */
      virtual void ProcessCorrespondence(const std::shared_ptr<Correspondence>& correspondence);

      // Wormored type
      virtual std::string GetType() {
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

      // Texture sheets
      Texture *idle_sheet;
      Texture *turn_sheet;
      Texture *attack_sheet;
      Texture *excrete_sheet;
      Texture *sleep_sheet;
      Texture *awake_sheet;
      Texture *tongue_texture;

      // Wakeup timer
      SecondsTimer attack_timer_;

      // Tongue enemy
      Enemy tongue_;
};

#endif

#endif
