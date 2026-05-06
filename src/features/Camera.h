#pragma once

#include "Matrix.h"
#include "Canvas.h"
#include "World.h"
#include "Ray.h"

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

    Camera(int hsize, int vsize, float fov) : hsize(hsize), vsize(vsize), fov(fov)
    {
        transform = Math::IdentityMatrix;

        // Half the width of the canvas. adjacent side / forward vector is 1 unit, so halfview is the opposite/canvas
        float halfView = tanf(fov / 2);
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
    ~Camera() = default;

    inline float pixelSize() { return _pixelSize; }
    inline float halfWidth() { return _halfWidth; }
    inline float halfHeight() { return _halfHeight; }

    Rendering::Ray RayForPixel(float px, float py)
    {
        float xOffset = (px + 0.5) * _pixelSize;
        float yOffset = (py + 0.5) * _pixelSize;

        float worldX = _halfWidth - xOffset;
        float worldY = _halfHeight - yOffset;

        Math::Point pixel = transform.Inverse() * Math::Point(worldX, worldY, -1);
        Math::Point origin = transform.Inverse() * Math::Point(0, 0, 0);
        Math::Vector direction = (pixel - origin).Normalized();

        return Rendering::Ray(origin, direction);
    }

    Rendering::Canvas Render(const World &world)
    {
        Rendering::Canvas image(hsize, vsize);

        for (int y = 0; y < vsize; y++)
        {
            for (int x = 0; x < hsize; x++)
            {
                Rendering::Ray ray = RayForPixel(x, y);
                Rendering::Color color = world.color_at(ray);
                image.WritePixelAt(x, y, color);
            }
        }

        return image;
    }
};
