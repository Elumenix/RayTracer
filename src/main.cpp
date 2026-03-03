#include <iostream>
#include "features/Tuple.h"
#include "Environment.h"
#include "Projectile.h"
#include "features/Matrix.h"
#include "features/Transformations.h"
#include "features/Canvas.h"

int main() {

    // Image
    int image_width = 100;
    int image_height = 100;
    Canvas canvas = Canvas(image_width, image_height);
    Color white = Color(1,1,1);

    float clockRadius = image_width * .45f;
    for (int i = 0; i < 12; i++) {
        Tuple p = Point(0,0,0);
        Matrix rot = transformations::rotationY(i * (M_PI / 6));
        p = rot * Point(0,0,1) * clockRadius; 
        canvas.WritePixelAt((int)roundf(p.x + image_width / 2), (int)roundf(p.z + image_height / 2), white);
    }

    std::string ppm = canvas.CanvasToPPM();
    std::cout << ppm;
}