#include <gtest/gtest.h>
#include "../src/features/Tuple.h" 

TEST(TupleTest, MakePoint1) {
    Tuple a = Tuple(4.3, -4.2, 3.1, 1.0);
    EXPECT_FLOAT_EQ(a.x, 4.3);
    EXPECT_FLOAT_EQ(a.y, -4.2);
    EXPECT_FLOAT_EQ(a.z, 3.1);
    EXPECT_FLOAT_EQ(a.w, 1.0);
}

TEST(TupleTest, MakeVector1) {
    Tuple a = Tuple(4.3, -4.2, 3.1, 0.0);
    EXPECT_FLOAT_EQ(a.x, 4.3);
    EXPECT_FLOAT_EQ(a.y, -4.2);
    EXPECT_FLOAT_EQ(a.z, 3.1);
    EXPECT_FLOAT_EQ(a.w, 0.0);
}

TEST(TupleTest, MakePoint2) {
    Tuple p = Point(4, -4, 3);

    EXPECT_EQ(p, Tuple(4, -4, 3, 1));
}

TEST(TupleTest, MakeVector2) {
    Tuple v = Vector(4, -4, 3);

    EXPECT_EQ(v, Tuple(4, -4, 3, 0));
}