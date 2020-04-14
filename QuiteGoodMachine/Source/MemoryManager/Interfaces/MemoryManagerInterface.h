#ifndef MEMORYMANAGERINTERFACE_H_
#define MEMORYMANAGERINTERFACE_H_

#include <memory>

class MemoryManagerInterface {
   public:
      /**
       * Object creation will follow this template with virtual components
       * 
       * @return Shared pointer to created object
       */
      template<class T>
      std::shared_ptr<T> CreateObject() {
         PreInstantiation();
         // Create object here --> need to find a way to pass params
         // here, not sure how
         Initialize();
         PostInstantiation();
      }

      /**
       * This function deals with anything that needs to be done before creating the object
       */
      virtual void PreInstantiation() = 0;

      /**
       * This function initializes the class as overriden by each object
       */
      virtual void Initialize() = 0;

      /**
       * This function deals with anything that needs to be done after creating the object
       */
      virtual void PostInstantiation() = 0;
};

#endif