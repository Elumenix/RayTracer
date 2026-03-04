#include <gtest/gtest.h>
#include "../src/features/Ray.h"
#include "../src/features/Sphere.h"
#include "../src/features/Intersection.h"
#include "../src/features/IntersectionList.h"

TEST(RayTest, CreateAndQuery) {
    Tuple origin = Point(1,2,3);
    Tuple direction = Vector(4,5,6);
    Ray r = Ray(origin, direction);

    EXPECT_EQ(r.origin, origin);
    EXPECT_EQ(r.direction, direction);
}

TEST(RayTest, PositionQuery) {
    Ray r = Ray(Point(2,3,4), Vector(1,0,0));
    
    EXPECT_EQ(r.position(0), Point(2,3,4));
    EXPECT_EQ(r.position(1), Point(3,3,4));
    EXPECT_EQ(r.position(-1), Point(1,3,4));
    EXPECT_EQ(r.position(2.5), Point(4.5,3,4));
}

TEST(RayTest, TwoPointIntersection) {
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_FLOAT_EQ(xs[0]->t, 4.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, 6.0f);
}

TEST(RayTest, TangentIntersection) {
    Ray r = Ray(Point(0,1,-5), Vector(0,0,1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_FLOAT_EQ(xs[0]->t, 5.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, 5.0f);
}

TEST(RayTest, RayMissed) {
    Ray r = Ray(Point(0,2,-5), Vector(0,0,1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_EQ(xs.size() == 0, true);
}

TEST(RayTest, RayInsideSphere) {
    Ray r = Ray(Point(0,0,0), Vector(0,0,1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_FLOAT_EQ(xs[0]->t, -1.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, 1.0f);
}

TEST(RayTest, RayBehindSphere) {
    Ray r = Ray(Point(0,0,5), Vector(0,0,1));
    Sphere s;
    IntersectionList xs = s.intersects(r);

    EXPECT_FLOAT_EQ(xs[0]->t, -6.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, -4.0f);
}

/*TEST(RayTest, IntersectionStruct) {
    Sphere s;
    Intersection i = Intersection(3.5, s);
}*/