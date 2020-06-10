#if 1
#ifndef VECTOR_H_
#define VECTOR_H_

#include "OpenGLIncludes.h"

template <int, typename T>
struct Vector;

template<typename T>
struct Vector<3, T> {
   /** Union of coordinates */
   union {
      struct {T x, y, z; };
      struct {T r, g, b; };
      struct {T w, h, d; };
   };

   /** Constructors */
   Vector(T a, T b, T c, T d);
   Vector(T a, T b, T c);
   Vector(glm::vec4 vec);
   Vector(glm::vec3 vec);

   /** Operators */
   Vector& operator=(Vector<3, T> const& v);
   Vector& operator=(glm::vec3 const& v);
   Vector& operator+(Vector<3, T> const& v);
   Vector& operator+(glm::vec3 const& v);
   Vector& operator+=(Vector<3, T> const& v);
   Vector& operator+=(glm::vec3 const& v);
   Vector& operator-(Vector<3, T> const& v);
   Vector& operator-(glm::vec3 const& v);
   Vector& operator-=(Vector<3, T> const& v);
   Vector& operator-=(glm::vec3 const& v);

   /** General functions */

   // Convert to GLM
   glm::vec3 ToGLM();

   // Set functions take either individual components as inputs or glm
   void Set(T x, T y, T z);
   void Set(glm::vec3 pos);
};

template<typename T>
struct Vector<4, T> {
   /** Union of coordinates */
   union {
      struct {T x, y, z, u; };
      struct {T r, g, b, a; };
      struct {T w, h, d, m; };
   };

   /** Constructors */
   Vector(T a, T b, T c, T d);
   Vector(glm::vec4 vec);

   /** Operators */
   Vector& operator=(Vector<4, T> const& v);
   Vector& operator=(Vector<3, T> const& v);
   Vector& operator=(glm::vec4 const& v);
   Vector& operator=(glm::vec3 const& v);
   Vector& operator+(Vector<4, T> const& v);
   Vector& operator+(Vector<3, T> const& v);
   Vector& operator+(glm::vec4 const& v);
   Vector& operator+(glm::vec3 const& v);
   Vector& operator+=(Vector<4, T> const& v);
   Vector& operator+=(Vector<3, T> const& v);
   Vector& operator+=(glm::vec4 const& v);
   Vector& operator+=(glm::vec3 const& v);
   Vector& operator-(Vector<4, T> const& v);
   Vector& operator-(Vector<3, T> const& v);
   Vector& operator-(glm::vec4 const& v);
   Vector& operator-(glm::vec3 const& v);
   Vector& operator-=(Vector<4, T> const& v);
   Vector& operator-=(Vector<3, T> const& v);
   Vector& operator-=(glm::vec4 const& v);
   Vector& operator-=(glm::vec3 const& v);

   /** General functions */

   // Convert to GLM
   glm::vec4 ToGLM();

   // Set functions take either individual components as inputs or glm
   void Set(T x, T y, T z, T u);
   void Set(glm::vec4 pos);
};

typedef Vector<3, float> Vector3f;

#include "Vector.imp"

#endif
#endif