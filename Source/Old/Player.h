#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL2/SDL.h>
#include "Box2D/Box2D.h"
#include "Texture.h"
#include "Timer.h"

class Application;

class Entity {
    public:
        // Constructor
        Entity(int x_pos, int y_pos, int height, int width, Application* application);

        // Collide function between entities
        virtual void collide(Entity* entity) = 0;
        
        // Getters and setters
        void set_x(int new_x);
        void set_y(int new_y);
        void set_height(int new_height);
        void set_width(int new_width);
        int get_x();
        int get_y();
        int get_height();
        int set_width();

        // Rendering functions (no need for texture getter, in load media simply use directly)
        Texture texture_;
        void render() = 0;

        // Get the application
        Application* get_application();

        // Movement and updating
        void update();
        virtual void move() = 0;

    private:
        // Entity positions
        int x_pos_;
        int y_pos_;

        // Height and width
        int height_;
        int width_;

        // Application pointer for reference
        Application* application_main_;
};

class Player : public Entity {
    public:
        // Construct the player
        Player(Application* application);

        // Flip player
        
        // Gravity management
        void fall();

        // Jumping physics
        void jump();

        // Move the player using keyboard
        void move();

        // Get the state of the player
        void get_state();

    private:
        // Stats of the player
        double mass_;
        
        // Add initial velocity
        double init_vel_x_;
        double init_vel_y_;
        double final_vel_x_;
        double final_vel_y_;
        
        // Add timer
        Timer jump_timer_;

        // States for gravity 
        bool has_jumped_;
        bool has_collided_;
        double gravity_;
        double fall_;
        
        // State construct for state machine
        enum STATE {
            JUMP,
            FALL,
            CROUCH,
            SHOOT
        };

        // Directions
        enum DIRS {
            NEUTRAL,
            LEFT,
            RIGHT
        };

        // Current time
        double curr_time_;

        // Player state
        STATE player_state_;
        DIRS player_directions_;
};

#endif
