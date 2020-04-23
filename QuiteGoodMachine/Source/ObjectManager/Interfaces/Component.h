#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <string>

/**
 * Very top abstract object type. All elements are objects with names.
 */
class Component {
   public:
      /**
       * Constructor
       * 
       * @param name Name of the object
       */
      Component(std::string name) {
         name_ = name;
      }


      /**
       * Get's the object's name
       * 
       * @return string The name of the object
       */
      std::string GetName() const {
         return name_;
      }

   private:
      std::string name_;
};

#endif