#pragma once

// Can't forward declare because used as a member variable. Need to know class size.
#include "Matrix.h"

namespace Rendering
{
    class Ray;
    class Canvas;
}

namespace Scene
{
    // Forward Declare
    class World;

    class Camera
    {
        float _pixelSize;
        float _halfWidth;
        float _halfHeight;

    public:
        const int hsize;
        const int vsize;
        const float fov;
        Math::Matrix<4, 4> transform;

        Camera(int hsize, int vsize, float fov);
        ~Camera() = default;

        inline float PixelSize() { return _pixelSize; }
        inline float HalfWidth() { return _halfWidth; }
        inline float HalfHeight() { return _halfHeight; }

        Rendering::Ray RayForPixel(float px, float py);
        Rendering::Canvas Render(const World &world, int recursionLimit = 4);
    };
}
