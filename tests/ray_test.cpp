#include <gtest/gtest.h>
#include "../src/features/Ray.h"

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
    IntersectionResult xs = r.intersects(s);

    EXPECT_EQ(xs.hit, true);
    EXPECT_FLOAT_EQ(xs.t1, 4.0f);
    EXPECT_FLOAT_EQ(xs.t2, 6.0f);
}

TEST(RayTest, TangentIntersection) {
    Ray r = Ray(Point(0,1,-5), Vector(0,0,1));
    Sphere s;
    IntersectionResult xs = r.intersects(s);

    EXPECT_EQ(xs.hit, true);
    EXPECT_FLOAT_EQ(xs.t1, 5.0f);
    EXPECT_FLOAT_EQ(xs.t2, 5.0f);
}

TEST(RayTest, RayMissed) {
    Ray r = Ray(Point(0,2,-5), Vector(0,0,1));
    Sphere s;
    IntersectionResult xs = r.intersects(s);

    EXPECT_EQ(xs.hit, false);
}

TEST(RayTest, RayInsideSphere) {
    Ray r = Ray(Point(0,0,0), Vector(0,0,1));
    Sphere s;
    IntersectionResult xs = r.intersects(s);

    EXPECT_EQ(xs.hit, true);
    EXPECT_FLOAT_EQ(xs.t1, -1.0f);
    EXPECT_FLOAT_EQ(xs.t2, 1.0f);
}

TEST(RayTest, RayBehindSphere) {
    Ray r = Ray(Point(0,0,5), Vector(0,0,1));
    Sphere s;
    IntersectionResult xs = r.intersects(s);

    EXPECT_EQ(xs.hit, true);
    EXPECT_FLOAT_EQ(xs.t1, -6.0f);
    EXPECT_FLOAT_EQ(xs.t2, -4.0f);
}