#pragma once
#include "Matrix.h"

namespace Transformations
{
    // This is only used for yaml
    /*struct CameraVectors
    {
        Math::Point from;
        Math::Point to;
        Math::Vector up;
    };*/

    Math::Matrix<4, 4> Translation(float x, float y, float z);
    Math::Matrix<4, 4> Scaling(float x, float y, float z);
    Math::Matrix<4, 4> RotationX(float rad);
    Math::Matrix<4, 4> RotationY(float rad);
    Math::Matrix<4, 4> RotationZ(float rad);
    Math::Matrix<4, 4> Shearing(float xy, float xz, float yx, float yz, float zx, float zy);
    Math::Matrix<4, 4> ViewTransform(Math::Point from, Math::Point to, Math::Vector up);
    /*CameraVectors InvertViewTransform(const Math::Matrix<4, 4> &m);*/
}
