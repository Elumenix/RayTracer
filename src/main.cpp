#include <iostream>
#include "features/Transformations.h"
#include "features/Canvas.h"
#include "features/Sphere.h"
#include "features/Camera.h"
#include "features/World.h"

int main() {
    World world;
    world.lights.push_back(Light(Point(-10,10,-10), Color(1,1,1)));

    Sphere floor;
    floor.transform = transformations::scaling(10,0.01,10);
    floor.material.color = Color(1,0.9,0.9);
    floor.material.specular = 0;
    world.add(std::move(floor));

    Sphere leftWall = floor;
    leftWall.transform = transformations::translation(0,0,5) * 
        transformations::rotationY(-M_PI_4) * transformations::rotationX(M_PI_2) * 
        transformations::scaling(10,0.01,10);
    world.add(std::move(leftWall));

    Sphere rightWall = floor;
    rightWall.transform = transformations::translation(0,0,5) * 
        transformations::rotationY(M_PI_4) * transformations::rotationX(M_PI_2) * 
        transformations::scaling(10,0.01,10);
    world.add(std::move(rightWall));

    Sphere middle;
    middle.transform = transformations::translation(-0.5,1,0.5);
    middle.material.color = Color(0.1, 1, 0.5);
    middle.material.diffuse = 0.7f;
    middle.material.specular = 0.3f;
    world.add(std::move(middle));

    Sphere right;
    right.transform = transformations::translation(1.5,0.5,-0.5) * transformations::scaling(0.5,0.5,0.5);
    right.material.color = Color(0.5,1,0.1);
    right.material.diffuse = 0.7f;
    right.material.specular = 0.3f;
    world.add(std::move(right));

    Sphere left;
    left.transform = transformations::translation(-1.5,0.33,-0.75) * transformations::scaling(0.33,0.33,0.33);
    left.material.color = Color(1,0.8,0.1);
    left.material.diffuse = 0.7f;
    left.material.specular = 0.3f;
    world.add(std::move(left));

    Camera camera = Camera(300, 150, M_PI / 3);
    camera.transform = transformations::viewTransform(Point(0,1.5,-5), Point(0,1,0), Vector(0,1,0));
    Canvas canvas = camera.Render(world);

    std::string ppm = canvas.CanvasToPPM();
    std::cout << ppm;
}