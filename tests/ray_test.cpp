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