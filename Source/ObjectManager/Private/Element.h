#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <Box2D/Box2D.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Source/ObjectManager/Private/Timer.h"

#include "Source/RenderingEngine/Private/Texture.h"

// Dependencies
class Entity;

// Shape struct
typedef struct Shape {
   union ShapeType {
      ShapeType(int w, int h, float a) : square{w, h, a} {}
      ShapeType(float a) : circle{a} {}
      struct Square {
         int width = 0;
         int height = 0;
         float angle = 0.0f;
      } square;
      struct Circle {
         float radius = 0.0f;
      } circle;
   } shape_type;

   // Flag for movement enabled shape
   bool dynamic = false;

   // Specify the center of the shape
   b2Vec2 center {0.0f, 0.0f};

   // Density of the shape
   float density = 1.0f;

   Shape() : shape_type(0, 0, 0.0f) {}
} Shape;

// This will be the base class for all elements in the game, including players and objects
class Element {
   public:
      // Constructor
      Element(int x, int y, int width, int height);

      // Setters
      void set_x(int new_x);
      void set_y(int new_y);
      void set_tex_x(int new_x);
      void set_tex_y(int new_y);
      void set_height(int new_height);
      void set_width(int new_width);

      // Getters
      float get_x();
      float get_y();
      float get_tex_x();
      float get_tex_y();
      float get_anim_x();
      float get_anim_y();
      float get_height() const;
      float get_width() const;

      // Adders
      void add_x(int add);
      void sub_x(int sub);
      void sub_tex_x(int sub);
      void add_y(int add);
      void sub_y(int add);

      // Enum for directions
      enum DIRS {
         LEFT,
         RIGHT,
         NEUTRAL
      };

      // ENUM for contact type
      enum CONTACT {
         CONTACT_LEFT,
         CONTACT_RIGHT,
         CONTACT_UP,
         CONTACT_DOWN
      };

      /***** BOX2D Variables *********/

      b2Body *body;
      b2BodyDef body_def;
      b2PolygonShape box;
      b2CircleShape circle;
      b2FixtureDef fixture_def;
      b2Fixture *main_fixture;

      enum COLLISIONS {
         CAT_PLAYER = 0x0001,
         CAT_PLATFORM = 0x0002,
         CAT_PROJECTILE = 0x0004,
         CAT_ENEMY = 0x0008,
         CAT_BOSS = 0x0010,
         CAT_SENSOR = 0x0020
      };

      enum ANTI_COLLISIONS {
         ACAT_PLAYER = 0xFFFE,
         ACAT_PLATFORM = 0xFFFD,
         ACAT_PROJECTILE = 0xFFFB,
         ACAT_ENEMY = 0xFFF7
      };

      // Set collisions
      void set_collision(uint16 collision_types, b2Fixture *fixture = nullptr);

      /*******************************/

      /****** CUSTOM SHAPE STUFF *****/

      Shape element_shape;

      enum SHAPE_TYPE {
         SQUARE,
         CIRCLE
      };

      /*******************************/

      /******* ANIMATION *************/
      // Texture reference
      Texture *sprite_sheet;

      // Each element has it's own animations (previously textures)
      std::unordered_map<std::string, Animation *> animations;

      /* Retrieve animation */
      virtual Animation *GetAnimationFromState() {};
      Animation *GetAnimationByName(std::string name);

      /* Flip all animations of a texture */
      void FlipAllAnimations();

      // Flag fo all animations flipped
      bool texture_flipped;

      /* Check if all animations have been flipped */
      bool TextureFlipped();

      /* Returns whether the animation is completed */
      bool AnimationCompleted(std::string name);
      /*******************************/

      /*********** MATRIX STUFF *******************/
      glm::mat4 element_model;
      /********************************************/

      // virtual load media
      virtual bool LoadMedia();

      // Set and add hitboxes
      void SetHitbox(int x, int y, SHAPE_TYPE type = SQUARE, int group = -1);
      void create_hitbox(float x, float y);

      // Check to see if it's still alive
      virtual bool is_alive();
      bool alive;

      // Render function
      void texture_render(Texture *texture);
      void Render(Texture *texture, int x = 0, int y = 0);

      // Update function for all elements
      virtual void move();
      virtual void animate(Texture *tex = NULL, int reset = 0, int max = 0, int start = 0);
      virtual void update(bool freeze = false);

      // Draw function if immediate drawing is desired
      void draw(Texture *tex = NULL, int reset = 0);

      // Texture related stuff
      virtual Texture *get_texture();
      GLFloatRect *get_curr_clip();

      // Texture for each element
      Texture texture;

      // Hash map of texture names to textures
      std::unordered_map<std::string, Texture *> textures;

      // Get type of object
      virtual std::string type() {
         return "Element";
      }

      // Check if enemy
      virtual bool is_enemy() {
         return false;
      }

      // Check if marked for destroy
      bool is_marked_for_destroy() const {
         return mark_for_immediate_destroy_;
      }

      // Apply force
      void apply_force(const b2Vec2 force) {
         body->ApplyForceToCenter(force, true);
      }

      // Start and end contact
      virtual void StartContact(Element *element = NULL) {}
      virtual void EndContact(Element *element = NULL) {}

      // Contact flag
      bool in_contact;
      bool in_contact_left;
      bool in_contact_right;
      bool in_contact_down;

      // Element destructor
      virtual ~Element();
   protected:
      // Flag for various things
      bool flag_;

      // Flag for destroying object immediately
      bool mark_for_immediate_destroy_;

   private:
      // X and Y locations
      float x_pos_;
      float y_pos_;

      // Height and widths
      float height_;
      float width_;

   public:
      // This section will deal with everything OpenGL related
      
};


// Sensor class
class Sensor : public Element {
   public:
      // Construct the sensor object
      Sensor(float height, float width, Entity *entity, CONTACT contact_type, float center_x = 0.0f, float center_y = 0.0f, float density = 0.0f, bool set_as_body = false);

      // Initialize function
      virtual void initialize(float width, float height, float center_x, float center_y, uint16 category = CAT_SENSOR, bool is_sensor = false);

      // Start contact function
      virtual void StartContact(Element *element = NULL) {};
      virtual void EndContact(Element *element = NULL) {};

      // Activation and deactivation of sensor
      virtual void activate_sensor();
      virtual void deactivate_sensor();

      // Change type
      virtual std::string type() {
         return "Sensor";
      }

      // Only one contact at a time
      CONTACT sensor_contact;

      // Maintain a filter
      b2Filter filter;

   protected:
      Entity *owner_;
      b2CircleShape circle_;
      b2Fixture *fixture_;
      float density_;
};

// Body part class for enemies
class BodyPart : public Sensor {
   public:
      // Constructor
      BodyPart(Entity *owning_entity, float x_rel_to_owner, float y_rel_to_owner, float width, float height, bool is_fixture = true, uint16 category = CAT_SENSOR);

      // Initialize function different for body part
      virtual void initialize(float width, float height, float center_x, float center_y, uint16 category = CAT_SENSOR);

      // Update function only takes the offsetted values
      virtual void update(bool freeze = false) {};
      void update(int x_offset = 0, int y_offset = 0);

      // Activate and deactivate
      virtual void activate_sensor() {
         body->SetActive(true);
      }
      virtual void deactivate_sensor() {
         body->SetActive(false);
      }

      // Start contact function
      virtual void StartContact(Element *element = NULL) {};
      virtual void EndContact(Element *element = NULL) {};

      // Return type
      virtual std::string type() {
         return type_;
      }

      int x_rel;
      int y_rel;

   private:
      std::string type_;

      bool is_fixture_;
};

#endif
