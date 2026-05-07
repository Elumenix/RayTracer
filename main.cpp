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
    world.lights.push_back(Light(Point(-10, 10, -10), Color(1, 1, 1)));

    Plane floor;
    floor.transform = Scaling(10, 0.01, 10);
    floor.material.color = Color(1, 0.9, 0.9);
    floor.material.pattern = MakePattern<Checker>();
    floor.material.pattern->transform = Scaling(.05, .05, .05);
    floor.material.specular = 0;
    world.Add(std::move(floor));

    /* Plane leftWall = floor;
     leftWall.transform = translation(0, 0, 5) *
                          rotationY(-M_PI_4) * rotationX(M_PI_2) *
                          scaling(10, 0.01, 10);
     world.Add(std::move(leftWall));

     Plane rightWall = floor;
     rightWall.transform = translation(0, 0, 5) *
                           rotationY(M_PI_4) * rotationX(M_PI_2) *
                           scaling(10, 0.01, 10);
     world.Add(std::move(rightWall));*/

    Sphere middle;
    middle.transform = Translation(-0.5, 1, 0.5);
    middle.material.color = Color(0.1, 1, 0.5);
    SolidColor c1(Color(0.1, 1, 0.5));
    RadialGradient smallStripe(&c1, &SolidBlack);
    middle.material.pattern = MakePattern<RadialGradient>(&smallStripe, &SolidWhite);
    middle.material.pattern->transform = RotationX(M_PI_2) * Scaling(.3, .3, .3);
    middle.material.diffuse = 0.7f;
    middle.material.specular = 0.3f;
    world.Add(std::move(middle));

    Sphere right;
    right.transform = Translation(1.5, 0.5, -0.5) * Scaling(0.5, 0.5, 0.5);
    right.material.color = Color(0.5, 1, 0.1);
    SolidColor c2(Color(0.5, 1, 0.1));
    right.material.pattern = MakePattern<Gradient>(&c2, &SolidWhite);
    right.material.pattern->transform = RotationY(M_PI_4) * Scaling(2, 2, 2) * Translation(.5, 0, 0);
    right.material.diffuse = 0.7f;
    right.material.specular = 0.3f;
    world.Add(std::move(right));

    Sphere left;
    left.transform = Translation(-1.5, 0.33, -0.75) * Scaling(0.33, 0.33, 0.33);
    left.material.color = Color(1, 0.8, 0.1);
    SolidColor c3(Color(1, 0.8, 0.1));
    StripePattern sp(&c3, &SolidBlack);
    left.material.pattern = MakePattern<Perturb>(&sp, 1337);
    left.material.pattern->transform = RotationX(-M_PI_2) * Scaling(.2, .2, .2);
    left.material.diffuse = 0.7f;
    left.material.specular = 0.3f;
    world.Add(std::move(left));

    Camera camera = Camera(640, 480, M_PI / 3);
    // Camera camera = Camera(320, 240, M_PI / 3);
    camera.transform = ViewTransform(Point(0, 1.5, -5), Point(0, 1, 0), Vector(0, 1, 0));
    Canvas canvas = camera.Render(world);
    canvas.CanvasToPNG();
}