#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <string>
#include "Texture.h"
#include "Timer.h"
#include "Element.h"

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
              int height, int width, bool owner, bool damage, Texture texture);

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
         CROUCH
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

      // Animate based on state
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);

      // Move the player using keyboard
      virtual void move();

      // Contact listener
      virtual void start_contact(Element *element = NULL);

      // Get type
      virtual std::string type() {
         return "Player";
      }

      // Load media function for the player
      virtual bool load_media();

      // Create projectile
      virtual Projectile *create_projectile(int delta_x_r, int delta_x_l, int delta_y, 
              int height, int width, bool owner, bool damage, Texture texture);

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
};

#endif
