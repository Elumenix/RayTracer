#include <iostream>
#include "features/Tuple.h"
#include "Environment.h"
#include "Projectile.h"
#include "features/Matrix.h"
#include "features/Transformations.h"
#include "features/Canvas.h"
#include "features/Sphere.h"

int main() {

    // Image
    int imageDimensions = 128;
    Canvas canvas = Canvas(imageDimensions, imageDimensions);
    Point camera = Point(0,0,-5);
    float WallZ = 10;
    float wallSize = 7;
    float pixelSize = wallSize / imageDimensions;
    float half = wallSize / 2;
    Sphere s; 
    s.material.color = Color(1,0.2,1);
    Light light = Light(Point(-10,10,-10), Color(1,1,1));

    for (auto y = 0; y < imageDimensions; y++) {
        // Converting from pixel space to wall space
        float worldY = half - pixelSize * y;

        for (auto x = 0; x < imageDimensions; x++) {
            float worldX = -half + pixelSize * x;
            
            Point pos = Point(worldX, worldY, WallZ);
            Vector rayDir = (pos - camera).Normalized();
            Ray r = Ray(camera, rayDir);
            IntersectionList xs = s.intersects(r);

            // If there's a hit in world space, we can write at the point in pixel space
            if (xs.hit() != nullptr) {
                Point hitPosition = r.position(xs.hit()->t);
                Vector normal = xs.hit()->object->normal_at(hitPosition);
                Vector eye = -r.direction;

                Color phongColor = xs.hit()->object->lighting(light, hitPosition, eye, normal);
                canvas.WritePixelAt(x, y, phongColor);
            }
        }
    }

    std::string ppm = canvas.CanvasToPPM();
    std::cout << ppm;
}