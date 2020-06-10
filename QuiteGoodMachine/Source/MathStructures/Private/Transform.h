#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "Matrix.h"

class Transform {
   public:
      /**
       * Constructor
       */
      // Transform(Transform Parent...)

      // Getters and setters

      /**
       * Get World Position
       */
      // Vector3f GetWorldPosition();

      /**
       * Get Relative Position
       */
      // Vector3f GetRelativePosition();

      /**
       * Set World Position
       */
      // void SetWorldPosition(Vector3f);

      /**
       * Set Relative Position
       */
      // void SetRelativePosition(Vector3f);

      //...

   private:
      // Transform of object
      Matrix4f transform_;

      // Transform of parent
      Matrix4F parent_transform_;
};

#endif