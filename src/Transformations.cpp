#include "Transformations.h"
#include <cmath>

namespace Transformations
{
    Math::Matrix<4, 4> Translation(float x, float y, float z)
    {
        Math::Matrix translation = Math::IdentityMatrix;
        translation[0][3] = x;
        translation[1][3] = y;
        translation[2][3] = z;
        return translation;
    }

    Math::Matrix<4, 4> Scaling(float x, float y, float z)
    {
        Math::Matrix scale = Math::IdentityMatrix;
        scale[0][0] = x;
        scale[1][1] = y;
        scale[2][2] = z;

        return scale;
    }

    Math::Matrix<4, 4> RotationX(float rad)
    {
        float s = std::sin(rad);
        float c = std::cos(rad);
        Math::Matrix rotation = Math::Matrix<4, 4>{
            1, 0, 0, 0,
            0, c, -s, 0,
            0, s, c, 0,
            0, 0, 0, 1};

        return rotation;
    }

    Math::Matrix<4, 4> RotationY(float rad)
    {
        float s = std::sin(rad);
        float c = std::cos(rad);
        Math::Matrix rotation = Math::Matrix<4, 4>{
            c, 0, s, 0,
            0, 1, 0, 0,
            -s, 0, c, 0,
            0, 0, 0, 1};

        return rotation;
    }

    Math::Matrix<4, 4> RotationZ(float rad)
    {
        float s = std::sin(rad);
        float c = std::cos(rad);
        Math::Matrix rotation = Math::Matrix<4, 4>{
            c, -s, 0, 0,
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};

        return rotation;
    }

    Math::Matrix<4, 4> Shearing(float xy, float xz, float yx, float yz, float zx, float zy)
    {
        Math::Matrix shear = Math::Matrix<4, 4>{
            1, xy, xz, 0,
            yx, 1, yz, 0,
            zx, zy, 1, 0,
            0, 0, 0, 1};

        return shear;
    }

    Math::Matrix<4, 4> ViewTransform(Math::Point from, Math::Point to, Math::Vector up)
    {
        Math::Vector forward = (to - from).Normalized();
        Math::Vector left = CrossProduct(forward, up.Normalized()).Normalized();
        Math::Vector trueUp = CrossProduct(left, forward);

        // This should tell the user if they set up the camera wrong. "Matrix can't be inverted" isn't as helpful
        assert(left.Magnitude() > EPSILON && "Up vector is parallel to forward — ambiguous orientation");

        Math::Matrix orientation = Math::Matrix<4, 4>({left.x, left.y, left.z, 0,
                                                       trueUp.x, trueUp.y, trueUp.z, 0,
                                                       -forward.x, -forward.y, -forward.z, 0,
                                                       0, 0, 0, 1});
        return orientation * Translation(-from.x, -from.y, -from.z);
    }

    /*CameraVectors InvertViewTransform(const Math::Matrix<4, 4> &m)
    {
        Math::Vector left = {m[0][0], m[0][1], m[0][2]};
        Math::Vector trueUp = {m[1][0], m[1][1], m[1][2]};
        Math::Vector negForward = {m[2][0], m[2][1], m[2][2]};
        Math::Vector forward = -negForward; // unit, exact

        Math::Vector t = {m[0][3], m[1][3], m[2][3]}; // translation column = -R*from

        // from = -R^T * t
        Math::Point from;
        from.x = -(left.x * t.x + trueUp.x * t.y + negForward.x * t.z);
        from.y = -(left.y * t.x + trueUp.y * t.y + negForward.y * t.z);
        from.z = -(left.z * t.x + trueUp.z * t.y + negForward.z * t.z);

        Math::Point to = from + forward;
        return {from, to, trueUp};
    }*/
}