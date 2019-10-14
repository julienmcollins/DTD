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

class Platform;
class Projectile;
class Player;

class Entity : public Element {
    public:
        // Constructor
        Entity(int x_pos, int y_pos, double height, double width);

        // Flags
        int has_jumped_;

        // Movement and updating
        virtual void move() = 0;
        virtual void update(bool freeze = false);

        // Texture and SDL stuff
        virtual Texture *get_texture() = 0;

        // Health
        int health;

        // Entity direction
        DIRS entity_direction;
        DIRS prev_entity_dir;

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

/************** PLAYER BODY PARTS ********************/
class PlayerHead : public BodyPart {
   public:
      PlayerHead(Player *player, float x_rel, float y_rel);
      
      // Start contact function
      virtual void start_contact(Element *element = NULL);
      virtual void end_contact(Element *element = NULL);

      virtual std::string type() {
         return "Player";
      }

      std::string sub_type() {
         return "PlayerHead";
      }
};

class PlayerArm : public BodyPart {
   public:
      PlayerArm(Player *player, float x_rel, float y_rel, int width, int height, std::string type);

      // Start contact function
      virtual void start_contact(Element *element = NULL);
      virtual void end_contact(Element *element = NULL);

      virtual std::string type() {
         return "Player";
      }

      std::string sub_type() {
         return type_;
      }

   private:
      std::string type_;
};

class PlayerHand : public BodyPart {
   public:
      PlayerHand(Player *player, float x_rel, float y_rel, std::string type);

      // Start contact function
      virtual void start_contact(Element *element = NULL);
      virtual void end_contact(Element *element = NULL);

      virtual std::string type() {
         return "Player";
      }

      std::string sub_type() {
         return type_;
      }

   private:
      std::string type_;
};

class PlayerLeg : public BodyPart {
   public:
      PlayerLeg(Player *player, float x_rel, float y_rel, int width, int height, std::string type);

      // Start contact function
      virtual void start_contact(Element *element = NULL);
      virtual void end_contact(Element *element = NULL);

      virtual std::string type() {
         return "Player";
      }

      std::string sub_type() {
         return type_;
      }
   
   private:
      std::string type_;
};

// Player class
class Player : public Entity {
   public:
      // Construct the player
      Player();

      // State construct for state machine
      enum STATE {
         STAND,
         RUN,
         JUMP,
         DOUBLE_JUMP,
         STOP,
         RUN_AND_JUMP,
         PUSH,
         JUMP_AND_PUSH,
         BALANCE,
         DEATH
      };

      // Enum for current key
      // TODO: MAKE THESE EXCLUSIVE BITS AND OR THEM TOGETHER
      enum KEYS {
         NONE, // = 0x0
         KEY_LEFT, // = 0x00001
         KEY_RIGHT, // = 0x00010
         KEY_UP, // = 0x00100
         KEY_DOWN, // = 0x01000
         KEY_SPACE // = 0x10000
      };

      KEYS key;
      KEYS last_key_pressed;

      // Contact enums
      enum CONTACTS {
         HEAD = 0,
         LEFT_ARM = 1,
         RIGHT_ARM = 2,
         LEFT_HAND = 3,
         RIGHT_HAND = 4,
         LEFT_LEG = 5,
         RIGHT_LEG = 6
      };

      // Flag for locking direction
      bool lock_dir_left;
      bool lock_dir_right;
      bool lock_dir_up;

      // Shooting flag
      bool shooting;

      // Number for random idle anim
      int rand_idle;
      Texture *curr_idle_texture;

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
      void process_input(const Uint8 *key_state);
      virtual void update(bool freeze = false);
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);
      virtual void move();

      // Contact listener
      virtual void start_contact(Element *element = NULL);
      virtual void end_contact(Element *element = NULL);

      // Damage function
      void take_damage(int damage);

      // Get type
      virtual std::string type() {
         return "Player";
      }

      // Load media function for the player
      virtual bool load_media();

      // File path
      static const std::string media_path;

      // Create projectile
      virtual Projectile* create_projectile(int delta_x_r, int delta_x_l, int delta_y, 
            bool owner, bool damage, float force_x, float force_y,
            const TextureData &normal, const TextureData &hit);

      // Hitmarkers
      std::vector<Hitmarker *> hit_markers;

      // Flags for interactions
      bool contacts_[5];

      // Virtual destructor
      virtual ~Player();
   private:
      // Player state
      STATE player_state_;

      // Previous position of player
      float prev_pos_x_;
      float prev_pos_y_;

      // Player body parts
      std::vector<BodyPart*> player_body_right_;
      std::vector<BodyPart*> player_body_left_;

      // Deactivation flags
      bool right_deactivated_ = false;
      bool left_deactivated_ = true;

      // Immunity timer
      Timer immunity_timer_;
      float immunity_duration_;
};

#endif
