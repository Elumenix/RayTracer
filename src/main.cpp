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
    int imageDimensions = 100;
    Canvas canvas = Canvas(imageDimensions, imageDimensions);
    Color Red = Color(1,0,0);
    Point rayOrigin = Point(0,0,-5);
    float WallZ = 10;
    float wallSize = 7;
    float pixelSize = wallSize / imageDimensions;
    float half = wallSize / 2;
    Sphere s; 

    for (auto y = 0; y < imageDimensions; y++) {
        // Converting from pixel space to wall space
        float worldY = half - pixelSize * y;

        for (auto x = 0; x < imageDimensions; x++) {
            float worldX = -half + pixelSize * x;
            
            Point pos = Point(worldX, worldY, WallZ);
            Ray r = Ray(rayOrigin, (pos - rayOrigin).Normalized());
            IntersectionList xs = s.intersects(r);

            // If there's a hit in world space, we can write at the point in pixel space
            if (xs.hit() != nullptr) {
                canvas.WritePixelAt(x, y, Red);
            }
        }
    }

    std::string ppm = canvas.CanvasToPPM();
    std::cout << ppm;
}