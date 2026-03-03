#include "Transformations.h"
namespace transformations {
    Matrix<4, 4> translation(float x, float y, float z)
    {
        Matrix translation = IdentityMatrix;
        translation[0][3] = x;
        translation[1][3] = y;
        translation[2][3] = z;
        return translation;
    }

    Matrix<4, 4> scaling(float x, float y, float z)
    {
        Matrix scale = IdentityMatrix;
        scale[0][0] = x;
        scale[1][1] = y;
        scale[2][2] = z;

        return scale;
    }

    Matrix<4, 4> rotationX(float rad)
    {
        float s = sinf(rad);
        float c = cosf(rad);
        Matrix rotation = Matrix<4,4>{
            1, 0, 0, 0,
            0, c, -s, 0,
            0, s, c, 0,
            0, 0, 0, 1
        };

        return rotation;
    }

    Matrix<4, 4> rotationY(float rad)
    {
        float s = sinf(rad);
        float c = cosf(rad);
        Matrix rotation = Matrix<4,4>{
            c, 0, s, 0,
            0, 1, 0, 0,
            -s, 0, c, 0,
            0, 0, 0, 1
        };

        return rotation;
    }

    Matrix<4, 4> rotationZ(float rad)
    {
        float s = sinf(rad);
        float c = cosf(rad);
        Matrix rotation = Matrix<4,4>{
            c, -s, 0, 0,
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        return rotation;
    }

    Matrix<4, 4> shearing(float xy, float xz, float yx, float yz, float zx, float zy)
    {
        Matrix shear = Matrix<4,4> {
            1, xy, xz, 0,
            yx, 1, yz, 0,
            zx, zy, 1, 0,
            0, 0, 0, 1
        };

        return shear;
    }
}