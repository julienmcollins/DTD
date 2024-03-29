#ifndef TANGIBLEELEMENT_H_
#define TANGIBLEELEMENT_H_

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"

#include <Box2D/Box2D.h>
#include <string>

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

/** Abstract base class (?) */
class TangibleElement : virtual public PositionalElement {
   private:
      // Shape specification
      // TODO: Make this better...
      enum SHAPE_TYPE {
         SQUARE,
         CIRCLE
      };

   public:
      /**
       * Constructor
       * Sets up physical body (?)
       */
      TangibleElement(std::string name, glm::vec3 initial_position, glm::vec3 size);

      /**
       * Update function
       */
      virtual void Update(bool freeze = false);

      /**
       * Sets the collision type
       * 
       * @param collision_types - list of collision types
       * @param fixture - additional fixtures
       */
      void SetCollision(uint16 collision_types, b2Fixture *fixture = nullptr);

      /**
       * Sets hitbox
       * 
       * @param x - x position
       * @param y - y position
       * @param type - shape type of element
       * @param group - group of collision
       */
      void SetHitbox(float x, float y, SHAPE_TYPE type = SQUARE, int group = -1);

      /**
       * Create hitbox
       * 
       * @param x - x position of hitbox
       * @param y - y position of hitbox
       */
      void CreateHitbox(float x, float y);

      /**
       * Move the element
       */
      virtual void Move();

      /**
       * Start's contact, when contact happens
       * 
       * @param element - shared ptr of element made contact with
       */
      virtual void StartContact(std::shared_ptr<TangibleElement> element = {}) {}

      /**
       * End contact, when contact ends
       * 
       * @param element - shared ptr of element end's contact with
       */
      virtual void EndContact(std::shared_ptr<TangibleElement> element = {}) {}

      /**
       * Check current contact
       */
      uint8 CheckCurrentContact();

      /**
       * Sets the current contact
       */
      void SetCurrentContact(uint8 contact);

      /**
       * Get type function returns tangible element
       */
      virtual std::string GetType();

      /**
       * Virtual destructor
       */
      virtual ~TangibleElement();

   private:
      // Physical location
      glm::vec3 body_position_;

      // BOX2D VARIABLES
      b2Body *body_;
      b2BodyDef body_def_;
      b2PolygonShape shape_;
      b2FixtureDef fixture_def_;
      b2Fixture *main_fixture_;

      // Matrix representation of position
      glm::mat4 hitbox_model_;
      float hitbox_angle_;

      // Actual shape of object
      Shape element_shape;

      // Contact flags
      uint8 contacts = 0; // Goes down, up, left and right (right to left)
};

#endif