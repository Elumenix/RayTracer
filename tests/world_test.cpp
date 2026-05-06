#include <gtest/gtest.h>
#include "../src/features/World.h"
#include "../src/features/Camera.h"
#include "../src/features/Transformations.h"
#include "../src/features/Sphere.h"
#include "../src/features/Intersection.h"
#include "../src/features/IntersectionList.h"
#include "../src/features/Comps.h"
#include "../src/features/Light.h"
#include "../src/features/Ray.h"
#include "../src/features/Canvas.h"

using namespace Math;
using namespace Rendering;
using namespace Transformations;
using namespace Scene;

TEST(WorldTest, CreateWorld)
{
    World w;

    EXPECT_EQ(w.lights.size(), 0);
    EXPECT_EQ(w.shapes.size(), 0);
}

TEST(WorldTest, DefaultWorld)
{
    World w = World::Default();
    Light l = Light(Point(-10, 10, -10), Color(1, 1, 1));
    Sphere s1;
    s1.material.color = Color(0.8, 1.0, 0.6);
    s1.material.diffuse = 0.7f;
    s1.material.specular = 0.2f;
    Sphere s2;
    s2.transform = Scaling(0.5, 0.5, 0.5);

    EXPECT_EQ(w.lights[0], l);
    EXPECT_EQ(*w.shapes[0], s1);
    EXPECT_EQ(*w.shapes[1], s2);
}

TEST(WorldTest, WorldIntersections)
{
    World w = World::Default();
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    IntersectionList xs = w.IntersectWorld(r);

    EXPECT_EQ(xs.Size(), 4);
    EXPECT_FLOAT_EQ(xs[0]->t, 4.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, 4.5f);
    EXPECT_FLOAT_EQ(xs[2]->t, 5.5f);
    EXPECT_FLOAT_EQ(xs[3]->t, 6.0f);
}

TEST(WorldTest, IntersectionShading)
{
    World w = World::Default();
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Shape &shape = *w.shapes[0];
    Intersection i = Intersection(4, &shape);
    Comps comps = PrepareComputation(i, r);
    Color c = w.ShadeHit(comps);

    EXPECT_EQ(c, Color(0.38066, 0.47583, 0.2855));
}

TEST(WorldTest, IntersectionShadingInside)
{
    World w = World::Default();
    w.lights[0] = Light(Point(0, 0.25, 0), Color(1, 1, 1));
    Ray r = Ray(Point(0, 0, 0), Vector(0, 0, 1));
    Shape &shape = *w.shapes[1];
    Intersection i = Intersection(0.5f, &shape);
    Comps comps = PrepareComputation(i, r);
    Color c = w.ShadeHit(comps);

    EXPECT_EQ(c, Color(0.90498f, 0.90498f, 0.90498f));
}

TEST(WorldTest, RayMiss)
{
    World w = World::Default();
    Ray r = Ray(Point(0, 0, -5), Vector(0, 1, 0));
    Color c = w.ColorAt(r);

    EXPECT_EQ(c, Color(0, 0, 0));
}

TEST(WorldTest, RayHitColor)
{
    World w = World::Default();
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Color c = w.ColorAt(r);

    EXPECT_EQ(c, Color(0.38066f, 0.47583f, 0.2855f));
}

TEST(WorldTest, BehindRayIntersection)
{
    World w = World::Default();
    Shape &outer = *w.shapes[0];
    outer.material.ambient = 1;
    Shape &inner = *w.shapes[1];
    inner.material.ambient = 1;
    Ray r = Ray(Point(0, 0, 0.75), Vector(0, 0, -1));
    Color c = w.ColorAt(r);

    EXPECT_EQ(c, inner.material.color);
}

TEST(CameraTest, ConstructingCamera)
{
    Camera c = Camera(160, 120, M_PI_2);

    EXPECT_EQ(c.hsize, 160);
    EXPECT_EQ(c.vsize, 120);
    EXPECT_FLOAT_EQ(c.fov, M_PI_2);
    EXPECT_EQ(c.transform, IdentityMatrix);
}

TEST(CameraTest, PixelSize)
{
    Camera c = Camera(200, 125, M_PI_2);
    EXPECT_FLOAT_EQ(c.PixelSize(), 0.01f);

    Camera c1 = Camera(125, 200, M_PI_2);
    EXPECT_FLOAT_EQ(c1.PixelSize(), 0.01f);
}

TEST(CameraTest, RayThroughCenterOfCanvas)
{
    Camera c = Camera(201, 101, M_PI_2);
    Ray r = c.RayForPixel(100, 50);
    EXPECT_EQ(r.origin, Point(0, 0, 0));
    EXPECT_EQ(r.direction, Vector(0, 0, -1));
}

TEST(CameraTest, RayThroughCorner)
{
    Camera c = Camera(201, 101, M_PI_2);
    Ray r = c.RayForPixel(0, 0);
    EXPECT_EQ(r.origin, Point(0, 0, 0));
    EXPECT_EQ(r.direction, Vector(0.66519f, 0.33259f, -0.66851f));
}

TEST(CameraTest, RayWithTransformedCamera)
{
    Camera c = Camera(201, 101, M_PI_2);
    c.transform = RotationY(M_PI_4) * Translation(0, -2, 5);
    Ray r = c.RayForPixel(100, 50);
    EXPECT_EQ(r.origin, Point(0, 2, -5));
    EXPECT_EQ(r.direction, Vector(sqrtf(2) / 2, 0, -sqrtf(2) / 2));
}

TEST(CameraTest, CameraRender)
{
    World w = World::Default();
    Camera c = Camera(11, 11, M_PI_2);
    Point from = Point(0, 0, -5);
    Point to = Point(0, 0, 0);
    Vector up = Vector(0, 1, 0);
    c.transform = ViewTransform(from, to, up);
    Canvas image = c.Render(w);

    EXPECT_EQ(image.GetPixelAt(5, 5), Color(0.38066, 0.47583, 0.2855));
}
