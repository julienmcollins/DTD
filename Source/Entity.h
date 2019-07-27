#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <string>
#include <vector>
#include "Texture.h"
#include "Timer.h"
#include "Element.h"

#define PC_OFF_X 62.0f
#define PC_OFF_Y 0.0f
#define SIM_GRAV -3.0f

class Application;
class Platform;
class Projectile;
class Player;

class Entity : public Element {
    public:
        // Constructor
        Entity(int x_pos, int y_pos, double height, double width, Application* application);

        // Flags
        bool has_jumped_;

        // Movement and updating
        virtual void move() = 0;
        virtual void update(bool freeze = false);

        // Texture and SDL stuff
        virtual Texture *get_texture() = 0;

        /***** Box2D Related Variables *****/

        // Make body public so that it can be accessed
        b2FixtureDef fixture_def;

        /***********************************/

        // Health
        int health;

        // Entity direction
        DIRS entity_direction;

        // Get the direction
        int get_dir() const;

        // Get type
        virtual std::string type() {
           return "Entity";
        }

        // Create projectile (might need to add an entity pointer just in case)
        virtual Projectile* create_projectile(int delta_x_r, int delta_x_l, int delta_y, 
               bool owner, bool damage, float force_x, float force_y,
               const TextureData &normal, const TextureData &hit);


         // Dumb flag for now, find a better way later
         bool shift_;

        // Destructor
        virtual ~Entity();
};

// Sensor class
class Sensor : public Element {
   public:
      // Construct the sensor object
      Sensor(int height, int width, Player *entity, float center_x);

      // Start contact function
      virtual void start_contact(Element *element = NULL);
      virtual void end_contact();

      // Change type
      virtual std::string type() {
         return "Sensor";
      }
   private:
      Player *entity_;
      b2CircleShape circle_;
};

// Hit marker
class Hitmarker : public Element {
   public:
      // Construct the hit marker object
      Hitmarker(int x, int y);

      // State of hit
      enum STATE {
         ALIVE,
         DEAD
      };

      // Load media function
      virtual bool load_media();

      // Update function
      virtual void update(bool freeze = false);
      virtual void animate();
      virtual Texture *get_texture();

      // State
      STATE state;
};

// Player class
class Player : public Entity {
   public:
      // Construct the player
      Player(Application* application);

      // State construct for state machine
      enum STATE {
         STAND,
         RUN,
         JUMP,
         STOP,
         RUN_AND_JUMP,
         PUSH,
         JUMP_AND_PUSH,
         CROUCH,
         DEATH
      };

      // Shooting flag
      bool shooting;

      // Function to get the proper texture based on the state
      virtual Texture *get_texture();

      // Arm delta displacement
      int arm_delta_x;
      int arm_delta_y;
      int arm_delta_shoot_x;
      int arm_delta_shoot_y;

      // Adjust delta function
      void adjust_deltas();

      // Get and set player state
      STATE get_player_state();
      void set_player_state(STATE new_state) {
         player_state_ = new_state;
      }

      // Change player state
      void change_player_state();

      // Update function now done in player
      virtual void update(bool freeze = false);
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);
      virtual void move();

      // Contact listener
      virtual void start_contact(Element *element = NULL);
      virtual void end_contact();

      // Damage function
      void take_damage(int damage);

      // Get type
      virtual std::string type() {
         return "Player";
      }

      // Load media function for the player
      virtual bool load_media();

      // Create projectile
      virtual Projectile* create_projectile(int delta_x_r, int delta_x_l, int delta_y, 
            bool owner, bool damage, float force_x, float force_y,
            const TextureData &normal, const TextureData &hit);

      // Hitmarkers
      std::vector<Hitmarker *> hit_markers;

      // Virtual destructor
      virtual ~Player();
   private:
      // Player state
      STATE player_state_;

      // Previous position of player
      float prev_pos_x_;
      float prev_pos_y_;

      // Sensor
      Sensor *left_sensor_;
      Sensor *right_sensor_;

      // Immunity timer
      Timer immunity_timer_;
      float immunity_duration_;
};

#endif
