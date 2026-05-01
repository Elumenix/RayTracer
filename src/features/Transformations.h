#pragma once
#include "Matrix.h"

namespace Transformations
{
    Math::Matrix<4, 4> translation(float x, float y, float z);
    Math::Matrix<4, 4> scaling(float x, float y, float z);
    Math::Matrix<4, 4> rotationX(float rad);
    Math::Matrix<4, 4> rotationY(float rad);
    Math::Matrix<4, 4> rotationZ(float rad);
    Math::Matrix<4, 4> shearing(float xy, float xz, float yx, float yz, float zx, float zy);
    Math::Matrix<4, 4> viewTransform(Math::Point from, Math::Point to, Math::Vector up);
}
