#include "Matrix.h"
#include "Canvas.h"

class Camera
{
    float _pixelSize;
    float _halfWidth;
    float _halfHeight;

public:
    const int hsize;
    const int vsize;
    const float fov;
    Matrix<4,4> transform;

    Camera(int hsize, int vsize, float fov) : hsize(hsize), vsize(vsize), fov(fov) {
        transform = IdentityMatrix;

        // Half the width of the canvas. adjacent side / forward vector is 1 unit, so halfview is the opposite/canvas 
        float halfView = tanf(fov/2);
        float aspectRatio = (float)hsize/vsize;

        if (aspectRatio >= 1) {
            _halfWidth = halfView;
            _halfHeight = halfView / aspectRatio;
        }
        else {
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

    
    Ray RayForPixel(float px, float py) {
        float xOffset = (px + 0.5) * _pixelSize;
        float yOffset = (py + 0.5) * _pixelSize;

        float worldX = _halfWidth - xOffset;
        float worldY = _halfHeight - yOffset;

        Point pixel = transform.inverse() * Point(worldX, worldY, -1);
        Point origin = transform.inverse() * Point(0,0,0);
        Vector direction = (pixel - origin).Normalized();

        return Ray(origin, direction);
    }

    Canvas Render(const World& world) {
        Canvas image(hsize, vsize);

        for (int y = 0; y < vsize; y++) {
            for (int x = 0; x < hsize; x++) {
                Ray ray = RayForPixel(x, y);
                Color color = world.color_at(ray);
                image.WritePixelAt(x, y, color);
            }
        }

        return image;
    }
};
