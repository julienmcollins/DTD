#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include "Texture.h"
#include "Timer.h"

class Application;
class Platform;

class Entity {
    public:
        // Constructor
        Entity(int x_pos, int y_pos, int height, int width, Application* application);

        // Setters
        void set_x(int new_x);
        void set_y(int new_y);
        void set_next_x(int new_x);
        void set_next_y(int new_y);
        void set_height(int new_height);
        void set_width(int new_width);
        void set_center_x(double new_center_x);
        void set_center_y(double new_center_y);

        // Getters
        int get_x() const;
        int get_y() const;
        int get_next_x() const;
        int get_next_y() const;
        int get_height() const;
        int get_width() const;
        double get_center_x();
        double get_center_y();

        // Adders
        void add_x(int add);
        void sub_x(int sub);
        void add_y(int add);
        void sub_y(int add);
        void add_next_x(int add);
        void sub_next_x(int sub);
        void add_next_y(int add);
        void sub_next_y(int sub);

        // Rendering functions (no need for texture getter, in load media simply use directly)
        Texture texture_;
        void render();

        // Flags
        bool has_jumped_;

        // Movement and updating
        virtual void move() = 0;
        void update();

        // Application
        Application* get_application();

        /***** Box2D Related Variables *****/
        // Make body public so that it can be accessed
        b2Body* body_;

        // Get world factor
        const float get_world_factor();
        /***********************************/

        // Destructor
        virtual ~Entity();

    private:
        // Entity positions
        int x_pos_;
        int y_pos_;

        // Prediction analysis for collision detection will need future x and y coords
        // Essentially, if current x/y is not colliding, but next x/y is, then collide preemptively
        int next_x_;
        int next_y_;

        // Height and width
        int height_;
        int width_;

        // Character center
        double center_x_;
        double center_y_;

        // Application pointer for reference
        Application* application_main_;

        // Box2D definitions
        b2BodyDef bodyDef_;
        b2PolygonShape dynamicBox_;
        b2FixtureDef fixtureDef_;
};

// Player class
class Player : public Entity {
    public:
        // Construct the player
        Player(Application* application);

        // Move the player using keyboard
        virtual void move();

        // Virtual destructor
        virtual ~Player();

    private:
        // Stats of the player
        double mass_;
        
        // Add initial velocity
        double init_vel_x_;
        double init_vel_y_;
        double init_vel_y_next_;
        double final_vel_x_;
        double final_vel_y_;
        double final_vel_x_next_;
        double final_vel_y_next_;
        
        // Add timer
        Timer jump_timer_;
        int num_jumps_;

        // States for gravity 
        double gravity_;
        double fall_;
        
        // State construct for state machine
        enum STATE {
            JUMP,
            FALL,
            CROUCH,
            SHOOT,
            STAND
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
