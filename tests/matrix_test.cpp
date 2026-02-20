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
    Matrix A = Matrix<4,4>({1,2,3,4,2,4,4,2,8,6,4,1,0,0,0,1});
    Tuple b = Point(1,2,3);

    Tuple expected = Tuple(18,24,33,1);

    // b * a wouldn't make sense
    EXPECT_EQ(A * b, expected);
}

TEST(MatrixTest, IdentityMatrix) {
    Matrix A = Matrix<4,4>({0,1,2,4,1,2,4,8,2,4,8,16,4,8,16,32});
    EXPECT_EQ((A * Matrix<4,4>::Identity), A);

    Tuple b = Tuple(1,2,3,4);
    EXPECT_EQ((Matrix<4,4>::Identity * b), b);
}

TEST(MatrixTest, TransposeTest) {
    Matrix A = Matrix<4,4>({0,9,3,0,9,8,0,8,1,8,5,3,0,0,5,8});
    Matrix T = Matrix<4,4>({0,9,1,0,9,8,8,0,3,0,5,5,0,8,3,8});

    EXPECT_EQ(A.Transpose(), T);
}

TEST(MatrixTest, TransposeIdentity) {
    Matrix A = Matrix<4,4>::Identity;

    EXPECT_EQ(A.Transpose(), A);
}

TEST(MatrixTest, Determinant2x2) {
    Matrix A = Matrix<2,2>({1,5,-3,2});

    EXPECT_FLOAT_EQ(A.Determinant(), 17.0f);
}

TEST(MatrixTest, SubMatrix32) {
    Matrix A = Matrix<3,3>({1,5,0,-3,2,7,0,6,-3});
    Matrix E = Matrix<2,2>({-3,2,0,6});

    EXPECT_EQ(A.SubMatrix(0, 2), E);
}

TEST(MatrixTest, SubMatrix43) {
    Matrix A = Matrix<4,4>({-6,1,1,6,-8,5,8,6,-1,0,8,2,-7,1,-1,1});
    Matrix E = Matrix<3,3>({-6,1,6,-8,8,6,-7,-1,1});

    EXPECT_EQ(A.SubMatrix(2, 1), E);
}

TEST(MatrixTest, TestMinor) {
    Matrix A = Matrix<3,3>({3,5,0,2,-1,-7,6,-1,5});
    Matrix B = A.SubMatrix(1,0);

    EXPECT_FLOAT_EQ(B.Determinant(), 25);
    EXPECT_FLOAT_EQ(A.Minor(1,0), 25);
}

TEST(MatrixTest, Cofactors) {
    Matrix A = Matrix<3,3>({3,5,0,2,-1,-7,6,-1,5});

    EXPECT_FLOAT_EQ(A.Minor(0,0), -12);
    EXPECT_FLOAT_EQ(A.Cofactor(0,0), -12);
    EXPECT_FLOAT_EQ(A.Minor(1,0), 25);
    EXPECT_FLOAT_EQ(A.Cofactor(1,0), -25);
}

TEST(MatrixTest, Determinant3x3) {
    Matrix A = Matrix<3,3>({1,2,6,-5,8,-4,2,6,4});

    EXPECT_FLOAT_EQ(A.Cofactor(0,0), 56);
    EXPECT_FLOAT_EQ(A.Cofactor(0,1), 12);
    EXPECT_FLOAT_EQ(A.Cofactor(0,2), -46);
    EXPECT_FLOAT_EQ(A.Determinant(), -196);
}

TEST(MatrixTest, Determinant4x4) {
    Matrix A = Matrix<4,4>({-2,-8,3,5,-3,1,7,3,1,2,-9,6,-6,7,7,-9});

    EXPECT_FLOAT_EQ(A.Cofactor(0,0), 690);
    EXPECT_FLOAT_EQ(A.Cofactor(0,1), 447);
    EXPECT_FLOAT_EQ(A.Cofactor(0,2), 210);
    EXPECT_FLOAT_EQ(A.Cofactor(0,3), 51);
    EXPECT_FLOAT_EQ(A.Determinant(), -4071);
}