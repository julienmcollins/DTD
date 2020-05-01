#ifndef ELEMENTINTERFACE_H_
#define ELEMENTINTERFACE_H_

#include "QuiteGoodMachine/Source/ObjectManager/Interfaces/Component.h"

#include <string>

class ElementInterface : public Component {
   public:
      /**
       * Constructor sets name of object
       */
      ElementInterface(std::string name) : Component(name) {}
      
      /**
       * Update
       * Updates the element, all elements need an update
       */
      virtual void Update(bool freeze = false) = 0;

      /**
       * IsActive
       * Checks if the element is active
       */
      bool IsActive() {
         return active_;
      }

      /**
       * Activate
       * Activates the element
       */
      void Activate() {
         active_ = true;
      }

      /**
       * Deactivate the element
       */
      void Deactivate() {
         active_ = false;
      }

      // TODO: Consider a mark for destroy function --> delegates to memory manager

      /**
       * Get type of element
       * 
       * @return string - Type of element
       */
      virtual std::string GetType() = 0;

      /**
       * Virtual destructor
       */
      virtual ~ElementInterface() {};

   private:
      // Simple flag for enabled/disabled elements
      bool active_ = true;
};

#endif