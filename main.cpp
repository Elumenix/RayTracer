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
    world.lights.push_back(Light(Point(2, 10, -5), Color(0.9, 0.9, 0.9)));

    Plane plane;
    plane.transform = RotationX(1.5708f) * Translation(0, 0, -10);
    Color c1(0.15,0.15,0.15);
    Color c2(0.85,0.85,0.85);
    SolidColor s1(c1);
    SolidColor s2(c2);
    plane.material.pattern = MakePattern<Checker>(&s1, &s2);
    plane.material.ambient = 0.8;
    plane.material.diffuse = 0.2;
    plane.material.specular = 0;
    world.Add(std::move(plane));

    Sphere sphere;
    sphere.material.color = White;
    sphere.material.ambient = 0.1;
    sphere.material.diffuse = 0;
    sphere.material.specular = 0;
    sphere.material.shininess = 300;
    sphere.material.reflective = 0.9;
    sphere.material.transparency = 0.9;
    sphere.material.refractiveIndex = 1.5;
    world.Add(std::move(sphere));

    Sphere sphere2;
    sphere2.material.color = White;
    sphere2.material.ambient = 0.1;
    sphere2.material.diffuse = 0;
    sphere2.material.specular = 0;
    sphere2.material.shininess = 300;
    sphere2.material.reflective = 0.9;
    sphere2.material.transparency = 0.9;
    sphere2.material.refractiveIndex = 1.0000034;
    sphere2.transform = Scaling(0.5,0.5,0.5);
    world.Add(std::move(sphere2));

    Camera camera = Camera(600, 600, 0.45);
    camera.transform = ViewTransform(Point(0,0,-5),Point(0,0,0),Vector(0,1,0));
    Canvas canvas = camera.Render(world);
    canvas.CanvasToPNG();
}
