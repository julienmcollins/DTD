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

class Entity : public Element {
    public:
        // Constructor
        Entity(int x_pos, int y_pos, double height, double width, Application* application);

        // Flags
        bool has_jumped_;

        // Movement and updating
        virtual void move() = 0;
        virtual void animate(Texture *tex = NULL, int reset = 0) = 0;
        virtual void update();

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
        Projectile* create_projectile(int delta_x_r, int delta_x_l, int delta_y, 
              int height, int width, bool owner, bool damage, Texture texture);

        // Destructor
        virtual ~Entity();
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
         CROUCH
      };

      // Shooting flag
      bool shooting;

      // Different textures
      Texture idle_texture;
      Texture running_texture;
      Texture kick_texture;
      Texture running_jump_texture;
      Texture idle_jump_texture;
      Texture arm_texture;
      Texture arm_shoot_texture;
      Texture arm_running_texture;
      Texture eraser_texture;

      // Function to get the proper texture based on the state
      virtual Texture *get_texture();

      // Arm delta displacement
      int arm_delta_x;
      int arm_delta_y;
      int arm_delta_shoot_x;
      int arm_delta_shoot_y;

      // Adjust delta function
      void adjust_deltas();

      // Get player state
      STATE get_player_state();

      // Change player state
      void change_player_state();

      // Update function now done in player
      virtual void update();

      // Animate based on state
      virtual void animate(Texture *tex = NULL, int reset = 0);

      // Move the player using keyboard
      virtual void move();

      // Contact listener
      virtual void start_contact() {
         health -= 10;
         if (health <= 0) {
            alive = false;
         }
      }

      // Get type
      virtual std::string type() {
         return "Player";
      }

      // Virtual destructor
      virtual ~Player();

   private:
      // Player state
      STATE player_state_;
};

class Enemy : public Entity {
   public:
      // Construct the enemy
      Enemy(Application *application);
      
      // Different enemy states
      enum STATE {
         IDLE,
         SHOOT,
         DEATH
      };

      // Different textures
      Texture idle_texture;
      Texture shoot_texture;
      Texture poojectile_texture;
      Texture death_texture;

      // Enemy's update function
      virtual void update();

      // Dummy move function
      virtual void move();

      // Animate function
      virtual void animate(Texture *tex = NULL, int reset = 0);

      // Get texture for enemies
      virtual Texture *get_texture();

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
      virtual ~Enemy();
   
   protected:
      STATE enemy_state_;
      int shoot_timer_;
};

#endif
