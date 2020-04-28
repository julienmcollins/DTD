#ifndef POSITIONALELEMENT_H_
#define POSITIONALELEMENT_H_

#include "QuiteGoodMachine/Source/ObjectManager/Interfaces/ElementInterface.h"

#include "OpenGLIncludes.h"

#include <string>

class PositionalElement : public ElementInterface {
   public:
      /**
       * Default constructor
       */
      PositionalElement();
      
      /**
       * Constructor
       */
      PositionalElement(std::string name, glm::vec2 initial_position, glm::vec2 size);

      /**
       * Update
       */
      virtual void Update(bool freeze = false);

      /**
       * Get's the position
       */
      glm::vec2 GetPosition() const;

      /**
       * Sets the position
       * 
       * @param position - new position
       */
      void SetPosition(glm::vec2 position);

      /**
       * Gets the size
       */
      glm::vec2 GetSize() const;

      /**
       * Sets the size
       * 
       * @param size - new size
       */
      void SetSize(glm::vec2 size);

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

   private:
      // Position vector
      glm::vec2 position_;

      // Size vector
      glm::vec2 size_;
};

#endif