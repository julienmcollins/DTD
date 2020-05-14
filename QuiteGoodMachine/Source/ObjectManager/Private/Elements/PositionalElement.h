#ifndef POSITIONALELEMENT_H_
#define POSITIONALELEMENT_H_

#include "QuiteGoodMachine/Source/ObjectManager/Interfaces/ElementInterface.h"

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"

#include "OpenGLIncludes.h"

#include <string>

class PositionalElement : public ElementInterface {
   public:
      /**
       * Default constructor
       */
      // PositionalElement();

      /**
       * Constructor
       */
      PositionalElement(std::string name, glm::vec3 initial_position, glm::vec3 size, float angle = 0.f);

      /**
       * Update
       */
      virtual void Update(bool freeze = false);

      /**
       * Get's the position
       */
      virtual glm::vec3 GetPosition() const;

      /**
       * Sets the position
       * 
       * @param position - new position
       */
      virtual void SetPosition(glm::vec3 position);

      /**
       * Gets the size
       */
      virtual glm::vec3 GetSize() const;

      /**
       * Sets the size
       * 
       * @param size - new size
       */
      virtual void SetSize(glm::vec3 size);


      /**
       * Get the state context
       */
      std::shared_ptr<StateContext> GetStateContext();

      /**
       * Get the angle
       */
      float GetAngle();

      /**
       * Get type of element
       * 
       * @return string - Type of element
       */
      virtual std::string GetType();

      /**
       * Destructor
       */
      virtual ~PositionalElement();

   protected:
      // Position vector
      glm::vec3 position_;

      // Size vector
      glm::vec3 size_;

      // Angle
      float angle_;

      // Reference to current state
      std::shared_ptr<StateContext> state_context_;
};

#endif