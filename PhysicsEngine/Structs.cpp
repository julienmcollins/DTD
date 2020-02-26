#include "Structs.h"

// Constructor
Vector2D::Vector2D() {
    this.x = 0;
    this.y = 0;
}

Vector2D::Vector2D(float x, float y) {
    this.x = x;
    this.y = y;
}

Matrix2D::Matrix2D(float m00, float m01, float m10, float m11) {
    this.m00 = m00;
    this.m01 = m01;
    this.m10 = m10;
    this.m11 = m11;
}
