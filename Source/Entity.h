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
        void update();

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
         SHOOT,
         RUN_AND_JUMP
      };

      // Different textures
      Texture idle_texture_;
      Texture running_texture_;
      Texture kick_texture_;
      Texture running_jump_texture_;
      Texture arm_texture_;

      // Function to get the proper texture based on the state
      Texture *get_texture();

      // Get current clip
      SDL_Rect *get_curr_clip();

      // Get player state
      STATE get_player_state();

      // Animate based on state
      void animate();

      // Move the player using keyboard
      virtual void move();

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

#endif
