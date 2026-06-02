#include <gtest/gtest.h>
#include "Sphere.h"
#include "Intersection.h"
#include "IntersectionList.h"
#include "Color.h"
#include "Plane.h"
#include "Transformations.h"
#include "Ray.h"
#include "Cube.h"
#include <cmath>

using namespace Math;
using namespace Transformations;
using namespace Rendering;
using namespace Scene;

TEST(ShapeTest, DefaultShapeTransformation)
{
    Sphere s;

    EXPECT_EQ(s.transform, IdentityMatrix);
}

TEST(ShapeTest, ChangeShapeTransformation)
{
    Sphere s;
    Matrix t = Translation(2, 3, 4);
    s.transform = t;

    EXPECT_EQ(s.transform, t);
}

TEST(ShapeTest, SphereNormalX)
{
    Sphere s;
    Vector n = s.NormalAt(Point(1, 0, 0));

    EXPECT_EQ(n, Vector(1, 0, 0));
}

TEST(ShapeTest, SphereNormalY)
{
    Sphere s;
    Vector n = s.NormalAt(Point(0, 1, 0));

    EXPECT_EQ(n, Vector(0, 1, 0));
}

TEST(ShapeTest, SphereNormalZ)
{
    Sphere s;
    Vector n = s.NormalAt(Point(0, 0, 1));

    EXPECT_EQ(n, Vector(0, 0, 1));
}

TEST(ShapeTest, SphereNormalNonaxial)
{
    Sphere s;
    Point nonaxialPoint = Point(std::sqrt(3.0f) / 3, std::sqrt(3.0f) / 3, std::sqrt(3.0f) / 3);
    Vector n = s.NormalAt(nonaxialPoint);

    EXPECT_EQ(n, Vector(std::sqrt(3.0f) / 3, std::sqrt(3.0f) / 3, std::sqrt(3.0f) / 3));
}

TEST(ShapeTest, SphereNormalIsNormalized)
{
    Sphere s;
    Vector n = s.NormalAt(Vector(std::sqrt(3.0f) / 3, std::sqrt(3.0f) / 3, std::sqrt(3.0f) / 3));

    EXPECT_EQ(n, n.Normalized());
}

TEST(ShapeTest, SphereNormalTranslated)
{
    Sphere s;
    s.transform = Translation(0, 1, 0);
    Vector n = s.NormalAt(Point(0, 1.70711, -0.70711));

    EXPECT_EQ(n, Vector(0, 0.70711, -0.70711));
}

TEST(ShapeTest, SphereTransformed)
{
    Sphere s;
    Matrix m = Scaling(1, 0.5, 1) * RotationZ(M_PI / 5);
    s.transform = m;
    Vector n = s.NormalAt(Vector(0, std::sqrt(2.0f) / 2, -std::sqrt(2.0f) / 2));

    EXPECT_EQ(n, Vector(0, 0.97014, -0.24254));
}

TEST(ShapeTest, NormalOfPlaneIsConstant)
{
    Plane p;
    Vector n1 = p.NormalAt(Point(0, 0, 0));
    Vector n2 = p.NormalAt(Point(10, 0, -10));
    Vector n3 = p.NormalAt(Point(-5, 0, 150));

    EXPECT_EQ(n1, Vector(0, 1, 0));
    EXPECT_EQ(n2, Vector(0, 1, 0));
    EXPECT_EQ(n3, Vector(0, 1, 0));
}

TEST(ShapeTest, IntersectRayParallelToPlane)
{
    Plane p;
    Ray r = Ray(Point(0, 10, 0), Vector(0, 0, 1));
    IntersectionList xs = p.Intersects(r);
    EXPECT_EQ(xs.Size(), 0);
}

TEST(ShapeTest, PlaneCoplanarIntersection)
{
    Plane p;
    Ray r = Ray(Point(0, 0, 0), Vector(0, 0, 1));
    IntersectionList xs = p.Intersects(r);
    EXPECT_EQ(xs.Size(), 0);
}

TEST(ShapeTest, PlaneIntersectionFromAbove)
{
    Plane p;
    Ray r = Ray(Point(0, 1, 0), Vector(0, -1, 0));
    IntersectionList xs = p.Intersects(r);
    EXPECT_EQ(xs.Size(), 1);
    EXPECT_EQ(xs[0]->t, 1);
    EXPECT_EQ(xs[0]->object, &p);
}

TEST(ShapeTest, PlaneIntersectionFromBelow)
{
    Plane p;
    Ray r = Ray(Point(0, -1, 0), Vector(0, 1, 0));
    IntersectionList xs = p.Intersects(r);
    EXPECT_EQ(xs.Size(), 1);
    EXPECT_EQ(xs[0]->t, 1);
    EXPECT_EQ(xs[0]->object, &p);
}

TEST(ShapeTest, GlassySphereHelper)
{
    Sphere s = GlassSphere();

    EXPECT_EQ(s.transform, IdentityMatrix);
    EXPECT_FLOAT_EQ(s.material.transparency, 1.0f);
    EXPECT_FLOAT_EQ(s.material.refractiveIndex, 1.5f);
}

TEST(ShapeTest, CubeIntersectionTest)
{
    Cube c;

    // Positive x
    Ray r(Point(5, 0.5, 0), Vector(-1, 0, 0));
    IntersectionList xs = c.CustomIntersects(r);
    EXPECT_EQ(xs.Size(), 2);
    EXPECT_FLOAT_EQ(xs[0]->t, 4);
    EXPECT_FLOAT_EQ(xs[1]->t, 6);

    // Negative x
    r.origin = Point(-5, 0.5, 0);
    r.direction = Vector(1, 0, 0);
    xs = c.CustomIntersects(r);
    EXPECT_EQ(xs.Size(), 2);
    EXPECT_FLOAT_EQ(xs[0]->t, 4);
    EXPECT_FLOAT_EQ(xs[1]->t, 6);

    // Positive y
    r.origin = Point(0.5, 5, 0);
    r.direction = Vector(0, -1, 0);
    xs = c.CustomIntersects(r);
    EXPECT_EQ(xs.Size(), 2);
    EXPECT_FLOAT_EQ(xs[0]->t, 4);
    EXPECT_FLOAT_EQ(xs[1]->t, 6);

    // Negative y
    r.origin = Point(0.5, -5, 0);
    r.direction = Vector(0, 1, 0);
    xs = c.CustomIntersects(r);
    EXPECT_EQ(xs.Size(), 2);
    EXPECT_FLOAT_EQ(xs[0]->t, 4);
    EXPECT_FLOAT_EQ(xs[1]->t, 6);

    // Positive z
    r.origin = Point(0.5, 0, 5);
    r.direction = Vector(0, 0, -1);
    xs = c.CustomIntersects(r);
    EXPECT_EQ(xs.Size(), 2);
    EXPECT_FLOAT_EQ(xs[0]->t, 4);
    EXPECT_FLOAT_EQ(xs[1]->t, 6);

    // Negative z
    r.origin = Point(0.5, 0, -5);
    r.direction = Vector(0, 0, 1);
    xs = c.CustomIntersects(r);
    EXPECT_EQ(xs.Size(), 2);
    EXPECT_FLOAT_EQ(xs[0]->t, 4);
    EXPECT_FLOAT_EQ(xs[1]->t, 6);

    // Inside
    r.origin = Point(0, 0.5, 0);
    r.direction = Vector(0, 0, 1);
    xs = c.CustomIntersects(r);
    EXPECT_EQ(xs.Size(), 2);
    EXPECT_FLOAT_EQ(xs[0]->t, -1);
    EXPECT_FLOAT_EQ(xs[1]->t, 1);
}

TEST(ShapeTest, RayMissesCube)
{
    Cube c;

    Ray r(Point(-2, 0, 0), Vector(0.2673, 0.5345, 0.8018));
    IntersectionList xs = c.CustomIntersects(r);
    EXPECT_EQ(xs.Size(), 0);

    r.origin = Point(0, -2, 0);
    r.direction = Vector(0.8018, 0.2673, 0.5345);
    EXPECT_EQ(xs.Size(), 0);

    r.origin = Point(0, 0, -2);
    r.direction = Vector(0.5345, 0.8018, 0.2673);
    EXPECT_EQ(xs.Size(), 0);

    r.origin = Point(2, 0, 2);
    r.direction = Vector(0, 0, -1);
    EXPECT_EQ(xs.Size(), 0);

    r.origin = Point(0, 2, 2);
    r.direction = Vector(0, -1, 0);
    EXPECT_EQ(xs.Size(), 0);

    r.origin = Point(2, 2, 0);
    r.direction = Vector(-1, 0, 0);
    EXPECT_EQ(xs.Size(), 0);
}
