#pragma once
#include "Matrix.h"
#include <math.h>


namespace transformations {
    Matrix<4,4> translation(float x, float y, float z);
    Matrix<4,4> scaling(float x, float y, float z);
    Matrix<4,4> rotationX(float rad);
    Matrix<4,4> rotationY(float rad);
    Matrix<4,4> rotationZ(float rad);
    Matrix<4,4> shearing(float xy, float xz, float yx, float yz, float zx, float zy);
    Matrix<4,4> viewTransform(Point from, Point to, Vector up);
}
