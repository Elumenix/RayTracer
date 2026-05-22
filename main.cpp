#include <iostream>
#include "Transformations.h"
#include "Canvas.h"
#include "Sphere.h"
#include "Camera.h"
#include "World.h"
#include "Plane.h"
#include "Light.h"

using namespace Math;
using namespace Rendering;
using namespace Transformations;
using namespace Scene;

int main()
{
    World world;
    world.lights.push_back(Light(Point(20, 10, 0), Color(0.7, 0.7, 0.7)));

    Plane plane;
    plane.transform = Translation(0,-10.1,0);
    plane.material.pattern = MakePattern<Checker>();
    plane.material.pattern.get()->transform = Translation(0,0.1,0);
    world.Add(std::move(plane));

    Sphere sphere;
    sphere.material.diffuse = 0.1;
    sphere.material.shininess = 300;
    sphere.material.reflective = 1;
    sphere.material.transparency = 1;
    sphere.material.refractiveIndex = 1.52;
    world.Add(std::move(sphere));

    Sphere sphere2;
    sphere2.material.diffuse = 0.1;
    sphere2.material.shininess = 300;
    sphere2.material.reflective = 1;
    sphere2.material.transparency = 1;
    sphere2.material.refractiveIndex = 1.0000034;
    sphere2.transform = Scaling(0.5,0.5,0.5);
    world.Add(std::move(sphere2));

    Camera camera = Camera(1024, 1024, M_PI / 3);
    camera.transform = ViewTransform(Point(0,2.5,0),Point(0,0,0),Vector(1,0,0));
    Canvas canvas = camera.Render(world, 5);
    canvas.CanvasToPNG();
}
