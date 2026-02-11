#include <gtest/gtest.h>
#include <cmath>
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

TEST(TupleTest, AddTuple) {
    Tuple a1 = Tuple(3, -2, 5, 1);
    Tuple a2 = Tuple(-2, 3, 1, 0);

    EXPECT_EQ(a1 + a2, Tuple(1,1,6,1));
}

TEST(TupleTest, SubtractPoints) {
    Tuple p1 = Point(3,2,1);
    Tuple p2 = Point(5,6,7);

    EXPECT_EQ(p1 - p2, Vector(-2,-4,-6));
}

TEST(TupleTest, SubtractVectorFromPoint) {
    Tuple p = Point(3,2,1);
    Tuple v = Vector(5,6,7);

    EXPECT_EQ(p - v, Point(-2,-4,-6));
}

TEST(TupleTest, SubtractingTwoVectors) {
    Tuple v1 = Vector(3,2,1);
    Tuple v2 = Vector(5,6,7);

    EXPECT_EQ(v1 - v2, Vector(-2,-4,-6));
}

TEST(TupleTest, SubtractFromZeroVector) {
    Tuple zero = Vector(0,0,0);
    Tuple v = Vector(1,-2,3);

    EXPECT_EQ(zero - v, Vector(-1,2,-3));
}

TEST(TupleTest, TupleNegation) {
    Tuple a = Tuple(1,-2,3,-4);

    EXPECT_EQ(-a, Tuple(-1, 2, -3, 4));
}

TEST(TupleTest, MultiplyTupleByScalar) {
    Tuple a = Tuple(1,-2, 3,-4);

    EXPECT_EQ(a * 3.5, Tuple(3.5, -7, 10.5, -14));
}

TEST(TupleTest, MultiplyTupleByFraction) {
    Tuple a = Tuple(1,-2, 3,-4);

    EXPECT_EQ(a * 0.5, Tuple(0.5, -1, 1.5, -2));
}

TEST(TupleTest, DivideTupleByScalar) {
    Tuple a = Tuple(1,-2, 3,-4);

    EXPECT_EQ(a / 2, Tuple(0.5, -1, 1.5, -2));
}

TEST(TupleTest, VectorMagnitude) {
    Tuple v = Vector(1,0,0);
    EXPECT_FLOAT_EQ(v.Magnitude(), 1);

    Tuple v2 = Vector(0,0,1);
    EXPECT_FLOAT_EQ(v2.Magnitude(), 1);

    Tuple v3 = Vector(0,0,1);
    EXPECT_FLOAT_EQ(v3.Magnitude(), 1);

    Tuple v4 = Vector(1,2,3);
    EXPECT_FLOAT_EQ(v4.Magnitude(), sqrt(14));

    Tuple v5 = Vector(-1, -2, -3);
    EXPECT_FLOAT_EQ(v5.Magnitude(), sqrt(14));
}

TEST(TupleTest, TupleNormalize) {
    Tuple v1 = Vector(4,0,0);
    EXPECT_EQ(v1.Normalized(), Vector(1,0,0));

    Tuple v2 = Vector(1,2,3);
    EXPECT_EQ(v2.Normalized(), Vector(0.26726, 0.53452, 0.80178));

    Tuple norm = v2.Normalized();
    EXPECT_FLOAT_EQ(norm.Magnitude(), 1);
}

TEST(TupleTest, TupleDotProduct) {
    Tuple a = Vector(1,2,3);
    Tuple b = Vector(2,3,4);

    EXPECT_FLOAT_EQ(DotProduct(a,b), 20);
}

TEST(TupleTest, TupleCrossProduct) {
    Tuple a = Vector(1,2,3);
    Tuple b = Vector(2,3,4);

    EXPECT_EQ(CrossProduct(a,b), Vector(-1,2,-1));
    EXPECT_EQ(CrossProduct(b,a), Vector(1,-2,1));
}

