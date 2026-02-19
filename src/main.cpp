#include <iostream>
#include "features/Tuple.h"
#include "Environment.h"
#include "Projectile.h"
#include "features/Canvas.h"

int main() {

    // Image
    int image_width = 100;
    int image_height = 100;
    Canvas canvas = Canvas(image_width, image_height);
    Color red = Color(1,0,0);

    // Testing out the projectile
    Projectile p = Projectile(Point(0,1,0), Vector(1,1.8,0));    
    Environment e = Environment(Vector(0, -0.05, 0), Vector(-0.01, 0, 0));

    while (p.position.y >= 0 && p.position.y <= image_height && p.position.x <= image_width && p.position.x >= 0) {
        //std::clog << "(" << p.position.x << ", " << p.position.y << ", " << p.position.z << ")" << std::endl;
        canvas.WritePixelAt(p.position.x, image_height - p.position.y - 1, red);

        p.position = p.position + p.velocity;
        p.velocity = p.velocity + e.gravity + e.wind;
    }

    std::string ppm = canvas.CanvasToPPM();
    std::cout << ppm;
}