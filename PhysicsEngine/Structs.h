#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <cmath>

class Vector2D {
    public:
        // Constructor
        Vector2D();
        Vector2D(float x, float y);

        // Vector points
        float x;
        float y;

        // Operator overloading
        Vector2D operator+(Vector2D a, const Vector2D& b) {
            a.x += b.x;
            a.y += b.y;
            return a;
        }

        Vector2D operator-(Vector2D a, const Vector2D& b) {
            a.x -= b.x;
            a.y -= b.y;
            return a;
        }

        Vector2D operator*(Vector2D a, const float& scaleFactor) {
            a.x *= scaleFactor;
            a.y *= scaleFactor;
            return a;
        }

        Vector2D operator/(Vector2D a, const float& divider) {
            float factor = 1 / divider;
            a.x *= factor;
            a.y *= factor;
            return a;
        }

        // Aux functions
        float length() {
            return sqrt(x * x + y * y);
        }

        float sqrLenght() {
            return x * x + y * y;
        }

        void normalize() {
            float val = 1.0f / length();
            x *= val;
            y *= val;
        }

        // Dot and cross
        float dot(Vector2D a, const Vector2D& b) {
            return a.x * b.x + a.y * b.y;
        }

        float cross(Vector2D a, const Vector2D& b) {
            return a.x * b.y - a.y * b.x;
        }

        Vector2D cross(Vector2D a, const float& scaleFactor) {
            a.y *= scaleFactor;
            a.x *= -scaleFactor;
            return a;
        }

        Vector2D cross(const float& scaleFactor, Vector2D a) {
            a.y *= -scaleFactor;
            a.x *= scaleFactor;
            return a;
        }
}

class Matrix2D {
    public:
        // Matrix2D constructor
        Matrix2D(float m00, float m01, float m10, float m11);

        // Matrix cells
        float m00, m01;
        float m10, m11;

        // Create from angle in radians
        void set(float radians) {
            float c = cos(radians);
            float s = sin(radians);

            m00 = c; m01 = -s;
            m10 = s; m11 = c;
        }

        Matrix2D transpose() {
            return Matrix2D(m00, m10,
                            m01, m11);
        }

        Vector2D operator*(const Vector2D& rhs) {
            return Vec2(m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y);
        }

    Matrix2D operator*(const Matrix2D& rhs ) {
        return Mat2(
            m00 * rhs.m00 + m01 * rhs.m10, m00 * rhs.m01 + m01 * rhs.m11,
            m10 * rhs.m00 + m11 * rhs.m10, m10 * rhs.m01 + m11 * rhs.m11);
    }
}

typedef struct {
    Vector2D min;
    Vector2D max;
} AABB;
