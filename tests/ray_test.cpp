#include <gtest/gtest.h>
#include "../src/features/Ray.h"
#include "../src/features/Sphere.h"
#include "../src/features/Intersection.h"
#include "../src/features/IntersectionList.h"

using namespace Math;
using namespace Transformations;
using namespace Rendering;

TEST(RayTest, CreateAndQuery)
{
    Tuple origin = Point(1, 2, 3);
    Tuple direction = Vector(4, 5, 6);
    Ray r = Ray(origin, direction);

    EXPECT_EQ(r.origin, origin);
    EXPECT_EQ(r.direction, direction);
}

TEST(RayTest, PositionQuery)
{
    Ray r = Ray(Point(2, 3, 4), Vector(1, 0, 0));

    EXPECT_EQ(r.position(0), Point(2, 3, 4));
    EXPECT_EQ(r.position(1), Point(3, 3, 4));
    EXPECT_EQ(r.position(-1), Point(1, 3, 4));
    EXPECT_EQ(r.position(2.5), Point(4.5, 3, 4));
}

TEST(RayTest, TwoPointIntersection)
{
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_FLOAT_EQ(xs[0]->t, 4.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, 6.0f);
}

TEST(RayTest, TangentIntersection)
{
    Ray r = Ray(Point(0, 1, -5), Vector(0, 0, 1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_FLOAT_EQ(xs[0]->t, 5.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, 5.0f);
}

TEST(RayTest, RayMissed)
{
    Ray r = Ray(Point(0, 2, -5), Vector(0, 0, 1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_EQ(xs.size() == 0, true);
}

TEST(RayTest, RayInsideSphere)
{
    Ray r = Ray(Point(0, 0, 0), Vector(0, 0, 1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_FLOAT_EQ(xs[0]->t, -1.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, 1.0f);
}

TEST(RayTest, RayBehindSphere)
{
    Ray r = Ray(Point(0, 0, 5), Vector(0, 0, 1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_FLOAT_EQ(xs[0]->t, -6.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, -4.0f);
}

TEST(RayTest, IntersectionStruct)
{
    Sphere s;
    Intersection i = Intersection(3.5f, &s);

    EXPECT_FLOAT_EQ(i.t, 3.5f);
    EXPECT_EQ(i.object, &s);
}

TEST(RayTest, AggregatingIntersections)
{
    Sphere s;
    Intersection i1 = Intersection(1, &s);
    Intersection i2 = Intersection(2, &s);
    IntersectionList xs = {i1, i2};

    EXPECT_EQ(xs.size(), 2);
    EXPECT_EQ(xs[0]->t, 1);
    EXPECT_EQ(xs[1]->t, 2);
}

TEST(RayTest, IntersectionListStorageOnRays)
{
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_EQ(xs.size(), 2);
    EXPECT_EQ(xs[0]->object, &s);
    EXPECT_EQ(xs[1]->object, &s);
}

TEST(RayTest, HitAllPositive)
{
    Sphere s;
    Intersection i1 = Intersection(1, &s);
    Intersection i2 = Intersection(2, &s);
    IntersectionList xs = {i2, i1};

    EXPECT_FLOAT_EQ(xs.hit()->t, i1.t);
}

TEST(RayTest, HitSomeNegative)
{
    Sphere s;
    Intersection i1 = Intersection(-1, &s);
    Intersection i2 = Intersection(1, &s);
    IntersectionList xs = {i2, i1};

    EXPECT_FLOAT_EQ(xs.hit()->t, i2.t);
}

TEST(RayTest, HitAllNegative)
{
    Sphere s;
    Intersection i1 = Intersection(-2, &s);
    Intersection i2 = Intersection(-1, &s);
    IntersectionList xs = {i2, i1};

    EXPECT_EQ(xs.hit(), nullptr);
}

TEST(RayTest, HitWithManyIntersections)
{
    Sphere s;
    Intersection i1 = Intersection(5, &s);
    Intersection i2 = Intersection(7, &s);
    Intersection i3 = Intersection(-3, &s);
    Intersection i4 = Intersection(2, &s);
    IntersectionList xs = {i1, i2, i3, i4};

    EXPECT_FLOAT_EQ(xs.hit()->t, i4.t);
}

TEST(RayTest, RayTranslation)
{
    Ray r = Ray(Point(1, 2, 3), Vector(0, 1, 0));
    Matrix m = Translation(3, 4, 5);
    Ray r2 = r.transform(m);

    EXPECT_EQ(r2.origin, Point(4, 6, 8));
    EXPECT_EQ(r2.direction, Vector(0, 1, 0));
}

TEST(RayTest, RayScaling)
{
    Ray r = Ray(Point(1, 2, 3), Vector(0, 1, 0));
    Matrix m = Scaling(2, 3, 4);
    Ray r2 = r.transform(m);

    EXPECT_EQ(r2.origin, Point(2, 6, 12));
    EXPECT_EQ(r2.direction, Vector(0, 3, 0));
}

TEST(RayTest, ScaledSphereRayIntersection)
{
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere s;
    s.transform = Scaling(2, 2, 2);
    IntersectionList xs = s.intersects(r);

    EXPECT_EQ(xs.size(), 2);
    EXPECT_FLOAT_EQ(xs[0]->t, 3);
    EXPECT_FLOAT_EQ(xs[1]->t, 7);
}

TEST(RayTest, TranslatedSphereRayIntersection)
{
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere s;
    s.transform = Translation(5, 0, 0);
    IntersectionList xs = s.intersects(r);

    EXPECT_EQ(xs.size(), 0);
}

TEST(RayTest, CompsTest)
{
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere shape;
    Intersection i = Intersection(4, &shape);

    Comps comps = prepare_computation(i, r);
    EXPECT_FLOAT_EQ(comps.t, i.t);
    EXPECT_EQ(i.object, &shape);
    EXPECT_EQ(comps.point, Point(0, 0, -1));
    EXPECT_EQ(comps.eye, Vector(0, 0, -1));
    EXPECT_EQ(comps.normal, Vector(0, 0, -1));
}

TEST(RayTest, CompOutsideShape)
{
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere shape;
    Intersection i = Intersection(4, &shape);
    Comps comps = prepare_computation(i, r);

    EXPECT_FALSE(comps.isInside);
}

TEST(RayTest, CompInsideShape)
{
    Ray r = Ray(Point(0, 0, 0), Vector(0, 0, 1));
    Sphere shape;
    Intersection i = Intersection(1, &shape);
    Comps comps = prepare_computation(i, r);

    EXPECT_EQ(comps.point, Point(0, 0, 1));
    EXPECT_EQ(comps.eye, Vector(0, 0, -1));
    EXPECT_TRUE(comps.isInside);
    EXPECT_EQ(comps.normal, Vector(0, 0, -1));
}

TEST(RayTest, CompOverPoint)
{
    Ray r = Ray(Point(0, 0, -5), Vector(0, 0, 1));
    Sphere shape;
    shape.transform = Translation(0, 0, 1);
    Intersection i = Intersection(5, &shape);
    Comps comps = prepare_computation(i, r);
    EXPECT_TRUE(comps.over_point.z < -EPSILON / 2);
    EXPECT_TRUE(comps.point.z > comps.over_point.z);
}