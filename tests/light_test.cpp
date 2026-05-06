#include <gtest/gtest.h>
#include "../src/features/Color.h"
#include "../src/features/Light.h"
#include "../src/features/Material.h"
#include "../src/features/Sphere.h"
#include "../src/features/World.h"
#include "../src/features/Comps.h"
#include "../src/features/Ray.h"
#include "../src/features/Transformations.h"
#include "../src/features/Intersection.h"
#include <cmath>

using namespace Math;
using namespace Rendering;
using namespace Scene;
using namespace Transformations;

TEST(LightTest, LightObject)
{
    Color intensity = Color(1, 1, 1);
    Point position = Point(0, 0, 0);
    Light light = Light(position, intensity);

    EXPECT_EQ(light.position, position);
    EXPECT_EQ(light.intensity, intensity);
}

TEST(LightTest, MaterialObject)
{
    Material m;

    EXPECT_EQ(m.color, Color(1, 1, 1));
    EXPECT_FLOAT_EQ(m.ambient, 0.1f);
    EXPECT_FLOAT_EQ(m.diffuse, 0.9f);
    EXPECT_FLOAT_EQ(m.specular, 0.9f);
    EXPECT_FLOAT_EQ(m.shininess, 200.0f);
}

TEST(LightTest, SphereDefaultMaterial)
{
    Sphere s;
    Material m = s.material;

    EXPECT_EQ(m, Material());
}

TEST(LightTest, SphereMaterialAssignment)
{
    Sphere s;
    Material m;
    m.ambient = 1.0f;
    s.material = m;

    EXPECT_EQ(s.material, m);
}

TEST(LightTest, EyeBetweenLightAndSurface)
{
    Sphere s;
    Point position = Point(0, 0, 0);

    Vector eye = Vector(0, 0, -1);
    Vector normal = Vector(0, 0, -1);
    Light light = Light(Point(0, 0, -10), Color(1, 1, 1));
    Color result = light.Lighting(s, position, eye, normal);

    EXPECT_EQ(result, Color(1.9, 1.9, 1.9));
}

TEST(LightTest, LightBetweeenEyeAndSurface)
{
    Sphere s;
    Vector eye = Vector(0, sqrtf(2.0f) / 2, -sqrtf(2.0f) / 2);
    Vector normal = Vector(0, 0, -1);
    Light light = Light(Point(0, 0, -10), Color(1, 1, 1));
    Color result = light.Lighting(s, Point(0, 0, 0), eye, normal);

    EXPECT_EQ(result, Color(1.0, 1.0, 1.0));
}

TEST(LightTest, EyeOppositeSurfaceLightOffset45)
{
    Sphere s;
    Vector eye = Vector(0, 0, -1);
    Vector normal = Vector(0, 0, -1);
    Light light = Light(Point(0, 10, -10), Color(1, 1, 1));
    Vector result = light.Lighting(s, Point(0, 0, 0), eye, normal);

    EXPECT_EQ(result, Color(0.7364, 0.7364, 0.7364));
}

TEST(LightTest, EyeInPathOfReflectionVector)
{
    Sphere s;
    Vector eye = Vector(0, -sqrtf(2.0f) / 2, -sqrtf(2.0f) / 2);
    Vector normal = Vector(0, 0, -1);
    Light light = Light(Point(0, 10, -10), Color(1, 1, 1));
    Color result = light.Lighting(s, Point(0, 0, 0), eye, normal);

    EXPECT_EQ(result, Color(1.6364, 1.6364, 1.6364));
}

TEST(LightTest, LightBehindSurface)
{
    Sphere s;
    Vector eye = Vector(0, 0, -1);
    Vector normal = Vector(0, 0, -1);
    Light light = Light(Point(0, 0, 10), Color(1, 1, 1));
    Color result = light.Lighting(s, Point(0, 0, 0), eye, normal);

    EXPECT_EQ(result, Color(0.1, 0.1, 0.1));
}

TEST(LightTest, SurfaceInShadow)
{
    Sphere s;
    Vector eye = Vector(0, 0, -1);
    Vector normal = Vector(0, 0, -1);
    Light light = Light(Point(0, 0, -10), Color(1, 1, 1));

    Color result = light.Lighting(s, Point(0, 0, 0), eye, normal, true);
    EXPECT_EQ(result, Color(0.1, 0.1, 0.1));
}

TEST(LightTest, NoShadowWorld)
{
    World w = World::Default();
    Point p = Point(0, 10, 0);

    EXPECT_FALSE(w.IsShadowed(p, w.lights[0]));
}

TEST(LightTest, IsShadowWorld)
{
    World w = World::Default();
    Point p = Point(10, -10, 10);

    EXPECT_TRUE(w.IsShadowed(p, w.lights[0]));
}

TEST(LightTest, WorldObjectBehindLight)
{
    World w = World::Default();
    Point p = Point(-20, 20, -20);

    EXPECT_FALSE(w.IsShadowed(p, w.lights[0]));
}

TEST(LightTest, WorldObjectBehindPoint)
{
    World w = World::Default();
    Point p = Point(-2, 2, -2);

    EXPECT_FALSE(w.IsShadowed(p, w.lights[0]));
}

TEST(LightTest, ShadeHitIntersection)
{
    World w;
    w.Add(Light(Point(0, 0, -10), Color(1, 1, 1)));
    Sphere s1;
    w.Add(std::move(s1));
    Sphere s2;
    s2.transform = Transformations::Translation(0, 0, 10);
    w.Add(std::move(s2));
    Intersection i = Intersection(4, w.shapes[1].get());
    Ray r = Ray(Point(0, 0, 5), Vector(0, 0, 1));
    Comps comps = PrepareComputation(i, r);
    Color c = w.ShadeHit(comps);
    EXPECT_EQ(c, Color(0.1, 0.1, 0.1));
}
