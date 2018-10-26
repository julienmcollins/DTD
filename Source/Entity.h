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
        void set_height(int new_height);
        void set_width(int new_width);
        void set_center_x(double new_center_x);
        void set_center_y(double new_center_y);

        // Getters
        int get_x() const;
        int get_y() const;
        float get_height() const;
        float get_width() const;
        double get_center_x();
        double get_center_y();

        // Adders
        void add_x(int add);
        void sub_x(int sub);
        void add_y(int add);
        void sub_y(int add);

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
        //const float get_world_factor();
        /***********************************/

        // Destructor
        virtual ~Entity();

    private:
        // Entity positions
        int x_pos_;
        int y_pos_;

        // Height and width
        float height_;
        float width_;

        // Character center
        double center_x_;
        double center_y_;

        // Application pointer for reference
        Application* application_main_;

    protected:

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

        // Player state
        STATE player_state_;
        DIRS player_directions_;
};

#endif
