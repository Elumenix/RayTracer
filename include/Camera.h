#pragma once

#include "Matrix.h" // Can't forward declare because used as a member variable. Need to know class size.
#include <thread>

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
        Math::Matrix<4, 4> invTransform; // cached to be much faster
        bool hasInvTransform = false;

        Camera();
        Camera(int hsize, int vsize, float fov);
        ~Camera() = default;
        Camera &operator=(const Camera &other); // Required for yaml to work

        inline float PixelSize() { return _pixelSize; }
        inline float HalfWidth() { return _halfWidth; }
        inline float HalfHeight() { return _halfHeight; }

        Rendering::Ray RayForPixel(float px, float py);
        Rendering::Canvas Render(const World &world, int recursionLimit = 5);
        Rendering::Canvas RenderMT(const World &world, int recursionLimit = 5, const int threadCount = 0); // multi-threaded version
    };
}
