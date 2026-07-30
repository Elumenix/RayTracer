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

        // Still need to check this because test cases don't use render, and so won't set this beforehand
        // Note: Render also sets this, so it doesn't cause problems with multithreading in the current setup
        if (!hasInvTransform)
        {
            invTransform = transform.Inverse();
            hasInvTransform = true;
        }

        Math::Point pixel = invTransform * Math::Point(worldX, worldY, -1);
        Math::Point origin = invTransform * Math::Point(0, 0, 0);
        Math::Vector direction = (pixel - origin).Normalized();

        return Ray(origin, direction);
    }

    Canvas Camera::Render(const World &world, int recursionLimit)
    {
        Canvas image(hsize, vsize);

        // We'll set up the cached invTransform now, because the camera's transform is now locked in for the render
        invTransform = transform.Inverse();
        hasInvTransform = true;

        for (int y = 0; y < vsize; y++)
        {
            for (int x = 0; x < hsize; x++)
            {
                Ray ray = RayForPixel(x, y);
                Color color = world.ColorAt(ray, recursionLimit);
                image.WritePixelAt(x, y, color);
            }

            // Communicate progress to javascript
#ifdef __EMSCRIPTEN__
            EM_ASM({ postMessage({type : "progress", progress : $0, total : $1}); }, y, (vsize - 1));
#endif
        }

        return image;
    }

    Canvas Camera::RenderMT(const World &world, int recursionLimit, const int threadCount)
    {
        Canvas image(hsize, vsize);

        // We'll set up the cached invTransform now, because the camera's transform is now locked in for the render
        invTransform = transform.Inverse();
        hasInvTransform = true;

        // Safe fallback of 4 if a strange error occurs in the javaScript;
        unsigned int numThreads = threadCount == 0 ? 4 : threadCount;
        std::atomic<int> nextRow{0};
        std::atomic<int> rowsCompleted{0};

        auto worker = [&]()
        {
            // Thereads will retrieve and increment the atomic rowNumber to figure out what data to work on
            int y;
            while ((y = nextRow.fetch_add(1)) < vsize)
            {
                // This thread will find the color for each pixel in this row
                for (int x = 0; x < hsize; x++)
                {
                    Ray ray = RayForPixel(x, y);
                    Color color = world.ColorAt(ray, recursionLimit);
                    image.WritePixelAt(x, y, color);
                }

                // Mark atomically that another row has been completed
                int completed = rowsCompleted.fetch_add(1) + 1;

                // Communication with javascript
#ifdef __EMSCRIPTEN__
                // Send message back to the main thread to increment the progress bar
                MAIN_THREAD_ASYNC_EM_ASM({ postMessage({type : "progress", progress : $0, total : $1}); }, completed, vsize);
#endif
            }
        };

        // Threads start immediately executing the above block while the main thread continues to add more threads
        std::vector<std::thread> threads;
        for (unsigned int i = 0; i < numThreads; i++)
        {
            threads.emplace_back(worker);
        }

        // We stay at the join block until the image is complete and the threads start escaping the while block
        for (auto &t : threads)
        {
            t.join();
        }

        return image;
    }
}
