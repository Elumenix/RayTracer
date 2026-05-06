#include <gtest/gtest.h>
#include "../src/features/Sphere.h"
#include "../src/features/Intersection.h"
#include "../src/features/IntersectionList.h"
#include "../src/features/Color.h"
#include "../src/features/Plane.h"
#include "../src/features/Transformations.h"
#include "../src/features/Ray.h"
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
    Point nonaxialPoint = Point(sqrtf(3.0f) / 3, sqrtf(3.0f) / 3, sqrtf(3.0f) / 3);
    Vector n = s.NormalAt(nonaxialPoint);

    EXPECT_EQ(n, Vector(sqrtf(3.0f) / 3, sqrtf(3.0f) / 3, sqrtf(3.0f) / 3));
}

TEST(ShapeTest, SphereNormalIsNormalized)
{
    Sphere s;
    Vector n = s.NormalAt(Vector(sqrtf(3.0f) / 3, sqrtf(3.0f) / 3, sqrtf(3.0f) / 3));

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
    Vector n = s.NormalAt(Vector(0, sqrtf(2.0f) / 2, -sqrtf(2.0f) / 2));

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
