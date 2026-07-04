#include "Camera.h"
#include "Ray.h"
#include "Canvas.h"
#include "Color.h"
#include "World.h"
#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

using namespace Rendering;

namespace Scene
{
    Camera::Camera() : hsize(128), vsize(128), fov(M_PI / 3)
    {
    }

    Camera::Camera(int hsize, int vsize, float fov) : hsize(hsize), vsize(vsize), fov(fov)
    {
        transform = Math::IdentityMatrix;

        // Half the width of the canvas. adjacent side / forward vector is 1 unit, so halfview is the opposite/canvas
        float halfView = std::tan(fov / 2);
        float aspectRatio = (float)hsize / vsize;

        if (aspectRatio >= 1)
        {
            _halfWidth = halfView;
            _halfHeight = halfView / aspectRatio;
        }
        else
        {
            _halfWidth = halfView * aspectRatio;
            _halfHeight = halfView;
        }

        // Assumption is pixels are square
        _pixelSize = (_halfWidth * 2) / hsize;
    }

    Camera &Camera::operator=(const Camera &other)
    {
        if (this != &other)
        {
            const_cast<int &>(hsize) = other.hsize;
            const_cast<int &>(vsize) = other.vsize;
            const_cast<float &>(fov) = other.fov;
            transform = other.transform;
            _halfWidth = other._halfWidth;
            _halfHeight = other._halfHeight;
            _pixelSize = other._pixelSize;
        }
        return *this;
    }

    Ray Camera::RayForPixel(float px, float py)
    {
        float xOffset = (px + 0.5) * _pixelSize;
        float yOffset = (py + 0.5) * _pixelSize;

        float worldX = _halfWidth - xOffset;
        float worldY = _halfHeight - yOffset;

        Math::Point pixel = transform.Inverse() * Math::Point(worldX, worldY, -1);
        Math::Point origin = transform.Inverse() * Math::Point(0, 0, 0);
        Math::Vector direction = (pixel - origin).Normalized();

        return Ray(origin, direction);
    }

    // Some optional pointers so webassembly can track progress and cancel the render if needed
    Canvas Camera::Render(const World &world, int recursionLimit, int *progress)
    {
        Canvas image(hsize, vsize);

        for (int y = 0; y < vsize; y++)
        {
#ifdef __EMSCRIPTEN__
            if (progress != nullptr)
            {
                *progress = y;
                EM_ASM({ postMessage({type : "progress", progress : $0, total : $1}); }, y, (vsize - 1));
            }
#endif

            for (int x = 0; x < hsize; x++)
            {
                Ray ray = RayForPixel(x, y);
                Color color = world.ColorAt(ray, recursionLimit);
                image.WritePixelAt(x, y, color);
            }
        }

        return image;
    }
}
