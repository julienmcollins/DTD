#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include "Texture.h"
#include "Timer.h"
#include "Element.h"

class Application;
class Platform;

class Entity : public Element {
    public:
        // Constructor
        Entity(int x_pos, int y_pos, double height, double width, Application* application);

        // Rendering functions (no need for texture getter, in load media simply use directly)
        void render(Texture *texture, SDL_Rect *clip);

        // Flags
        bool has_jumped_;

        // Movement and updating
        virtual void move() = 0;
        virtual void animate() = 0;
        virtual void update();

        // Texture and SDL stuff
        virtual Texture *get_texture() = 0;
        SDL_Rect *get_curr_clip();

        /***** Box2D Related Variables *****/

        // Make body public so that it can be accessed
        b2FixtureDef fixture_def;

        /***********************************/

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
         CROUCH,
         RUN_AND_JUMP
      };

      // Shooting flag
      bool shooting;

      // Different textures
      Texture idle_texture;
      Texture running_texture;
      Texture kick_texture;
      Texture running_jump_texture;
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

      // Get player state
      STATE get_player_state();

      // Update function now done in player
      virtual void update();

      // Animate based on state
      virtual void animate();

      // Move the player using keyboard
      virtual void move();

      // Create eraser
      void create_eraser();

      // Virtual destructor
      virtual ~Player();

   private:
      // Directions
      enum DIRS {
          NEUTRAL,
          LEFT,
          RIGHT
      };

      // Player state
      STATE player_state_;
      DIRS player_direction_;
};

class Enemy : public Entity {
   public:
      // Construct the enemy
      Enemy(Application *application);
      
      // Different enemy states
      enum STATE {
         IDLE,
         SHOOT
      };

      // Different textures
      Texture idle_texture;
      Texture shoot_texture;

      // Enemy's update function
      virtual void update();

      // Dummy move function
      virtual void move() {}

      // Animate function
      virtual void animate();

      // Get texture for enemies
      virtual Texture *get_texture();
   
   protected:
      STATE enemy_state_;
};

#endif
