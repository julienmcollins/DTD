#ifndef CLASSTYPE_H_
#define CLASSTYPE_H_

template <class Type>
class ClassType {
   public:
      /** Default constructor defaults to nullptr **/
      ClassType() :
         object(nullptr) {}

      // /** Assignment operator constructs object **/
      // ClassType& operator=(const ClassType<Type>& other) {
      //    object = *other;
      //    return *this;
      // }

      // /** Dereference into underlying object **/
      // Type* operator*() const {
      //    return object;
      // }

      // /** Get underlying object **/
      // Type* operator->() const {
      //    return **this;
      // }
      /**
       * Simply get underlying object
       */
      Type *GetObject() const {
         return object;
      }

   private:
      // Type object
      Type *object;
};

#endif