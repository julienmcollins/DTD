#ifndef OBJECT_H_
#define OBJECT_H_

#include "Texture.h"

class Entity;

// A parent class because I might add other objects in the future
class Object {
    public:
        // Constructor
        Object(double x, double y, double length, double width);

        // Getters
        double get_x() const;
        double get_y() const;
        double get_height() const;
        double get_width() const;

        // Setters
        void set_x(double new_x);
        void set_y(double new_y);
        void set_height(double new_height);
        void set_width(double new_width);

        // Object texture
        Texture texture_;

        // Might need to add a function that gets texture
        // Pure virtual function to make this abstract base class
        virtual void doNothing() = 0;

        // Virtual collision function
        //virtual void collide(Entity* entity) = 0;

    private:
        // Metrics of the object
        double x_pos_;
        double y_pos_;
        double height_;
        double width_;
};

// Platform class will be a subclass of Object (through inheritance)
class Platform : public Object {
    public:
        // Constructor
        Platform(double x, double y);

        // Default constructor
        Platform();

        // Other functions
        virtual void doNothing();
        //virtual void collide(Entity* entity);
    // Might need to add a function that changes texture
};

#endif
