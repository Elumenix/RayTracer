#include <gtest/gtest.h>
#include "../src/features/Matrix.h"
#include "../src/features/Tuple.h"

TEST(MatrixTest, CreateMatrix4x4) {
    Matrix m = Matrix<4,4>();

    m[0][0] = 1;
    m[0][1] = 2;
    m[0][2] = 3;
    m[0][3] = 4;
    m[1][0] = 5.5;
    m[1][1] = 6.5;
    m[1][2] = 7.5;
    m[1][3] = 8.5;
    m[2][0] = 9;
    m[2][1] = 10;
    m[2][2] = 11;
    m[2][3] = 12;
    m[3][0] = 13.5;
    m[3][1] = 14.5;
    m[3][2] = 15.5;
    m[3][3] = 16.5;

    EXPECT_FLOAT_EQ(m[0][0], 1);
    EXPECT_FLOAT_EQ(m[0][3], 4);
    EXPECT_FLOAT_EQ(m[1][0], 5.5);
    EXPECT_FLOAT_EQ(m[1][2], 7.5);
    EXPECT_FLOAT_EQ(m[2][2], 11);
    EXPECT_FLOAT_EQ(m[3][0], 13.5);
    EXPECT_FLOAT_EQ(m[3][2], 15.5);
}

TEST(MatrixTest, CreateMatrix2x2) {
    Matrix m = Matrix<2,2>({-3,5,1,-2});

    EXPECT_FLOAT_EQ(m[0][0], -3);
    EXPECT_FLOAT_EQ(m[0][1], 5);
    EXPECT_FLOAT_EQ(m[1][0], 1);
    EXPECT_FLOAT_EQ(m[1][1], -2);
}

TEST(MatrixTest, CreatMatrix3x3) {
    Matrix m = Matrix<3,3>({-3,5,0,1,-2,-7,0,1,1});

    EXPECT_FLOAT_EQ(m[0][0], -3);
    EXPECT_FLOAT_EQ(m[1][1], -2);
    EXPECT_FLOAT_EQ(m[2][2], 1);
}

TEST(MatrixTest, MatrixEquality) {
    Matrix a = Matrix<4,4>({1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2});
    Matrix b = Matrix<4,4>({1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2});

    EXPECT_EQ(a,b);
}

TEST(MatrixTest, MatrixInequality) {
    Matrix a = Matrix<4,4>({1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2});
    Matrix b = Matrix<4,4>({2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1});

    EXPECT_NE(a,b);
}

TEST(MatrixTest, MatrixMultSameDimensions) {
    Matrix a = Matrix<4,4>({1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2});
    Matrix b = Matrix<4,4>({-2,1,2,3,3,2,1,-1,4,3,6,5,1,2,7,8});

    Matrix expected = Matrix<4,4>({20,22,50,48,44,54,114,108,40,58,110,102,16,26,46,42});
    EXPECT_EQ(a*b, expected);
}

TEST(MatrixTest, ScalarMultiplication) {
    Matrix m = Matrix<2,2>({4,6,2,1});

    Matrix a = Matrix<2,2>({12,18,6,3});
    Matrix b = Matrix<2,2>({8,12,4,2});

    // Checking both directions
    EXPECT_EQ(3*m, a);
    EXPECT_EQ(m*2, b);
}

TEST(MatrixTest, TupleMultiplication) {
    Matrix a = Matrix<4,4>({1,2,3,4,2,4,4,2,8,6,4,1,0,0,0,1});
    Tuple b = Point(1,2,3);

    Matrix expected = Matrix<1,4>({18,24,33,1});

    // b * a wouldn't make sense
    EXPECT_EQ(a * b, expected);
}

TEST(MatrixTest, IdentityMatrix) {
    Matrix a = Matrix<4,4>({0,1,2,4,1,2,4,8,2,4,8,16,4,8,16,32});
    EXPECT_EQ((a * Matrix<4,4>::Identity), a);

    //Tuple b = Tuple(1,2,3,4);
    //EXPECT_EQ((Matrix<4,4>::Identity * b), Matrix<1,4>(1,2,3,4));
}