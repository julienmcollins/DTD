#ifndef MATRIX_H_
#define MATRIX_H_

#include "OpenGLIncludes.h"
#include "Vector.h"

template <int, int, typename T>
struct Matrix;

template <typename T>
struct Matrix<4, 4, T> {

   /** Typedef vec4 **/
   typedef Vector<4, T> col;

   /**
    * Union of array
    */
   union {
      col Mat[4];
   };

   /** Constructors **/
   Matrix(float init = 1.f); // Identity
   Matrix(glm::mat4 mat);

   /** Operator overloads **/
   Matrix& operator=(Matrix<4, 4, T> const& mat);
   Matrix& operator=(glm::mat4 const& mat);

   /**
    * Get translation
    */
   Vector3f GetTranslation();

   /**
    * Translate
    */
   void Translate(Vector3f vec);
};

typedef Matrix<4, 4, float> Matrix4f;

#include "Matrix.imp"

#endif