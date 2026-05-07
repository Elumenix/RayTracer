#include <gtest/gtest.h>
#include "Matrix.h"
#include "Transformations.h"
#include "Tuple.h"
#include <math.h>

using namespace Math;
using namespace Transformations;

TEST(MatrixTest, CreateMatrix4x4)
{
    Matrix m = Matrix<4, 4>();

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

TEST(MatrixTest, CreateMatrix2x2)
{
    Matrix m = Matrix<2, 2>({-3, 5, 1, -2});

    EXPECT_FLOAT_EQ(m[0][0], -3);
    EXPECT_FLOAT_EQ(m[0][1], 5);
    EXPECT_FLOAT_EQ(m[1][0], 1);
    EXPECT_FLOAT_EQ(m[1][1], -2);
}

TEST(MatrixTest, CreatMatrix3x3)
{
    Matrix m = Matrix<3, 3>({-3, 5, 0, 1, -2, -7, 0, 1, 1});

    EXPECT_FLOAT_EQ(m[0][0], -3);
    EXPECT_FLOAT_EQ(m[1][1], -2);
    EXPECT_FLOAT_EQ(m[2][2], 1);
}

TEST(MatrixTest, MatrixEquality)
{
    Matrix a = Matrix<4, 4>({1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2});
    Matrix b = Matrix<4, 4>({1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2});

    EXPECT_EQ(a, b);
}

TEST(MatrixTest, MatrixInequality)
{
    Matrix a = Matrix<4, 4>({1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2});
    Matrix b = Matrix<4, 4>({2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1});

    EXPECT_NE(a, b);
}

TEST(MatrixTest, MatrixMultSameDimensions)
{
    Matrix a = Matrix<4, 4>({1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2});
    Matrix b = Matrix<4, 4>({-2, 1, 2, 3, 3, 2, 1, -1, 4, 3, 6, 5, 1, 2, 7, 8});

    Matrix expected = Matrix<4, 4>({20, 22, 50, 48, 44, 54, 114, 108, 40, 58, 110, 102, 16, 26, 46, 42});
    EXPECT_EQ(a * b, expected);
}

TEST(MatrixTest, ScalarMultiplication)
{
    Matrix m = Matrix<2, 2>({4, 6, 2, 1});

    Matrix a = Matrix<2, 2>({12, 18, 6, 3});
    Matrix b = Matrix<2, 2>({8, 12, 4, 2});

    // Checking both directions
    EXPECT_EQ(3 * m, a);
    EXPECT_EQ(m * 2, b);
}

TEST(MatrixTest, TupleMultiplication)
{
    Matrix A = Matrix<4, 4>({1, 2, 3, 4, 2, 4, 4, 2, 8, 6, 4, 1, 0, 0, 0, 1});
    Tuple b = Point(1, 2, 3);

    Tuple expected = Tuple(18, 24, 33, 1);

    // b * a wouldn't make sense
    EXPECT_EQ(A * b, expected);
}

TEST(MatrixTest, IdentityMatrix)
{
    Matrix A = Matrix<4, 4>({0, 1, 2, 4, 1, 2, 4, 8, 2, 4, 8, 16, 4, 8, 16, 32});
    EXPECT_EQ((A * IdentityMatrix), A);

    Tuple b = Tuple(1, 2, 3, 4);
    EXPECT_EQ((IdentityMatrix * b), b);
}

TEST(MatrixTest, TransposeTest)
{
    Matrix A = Matrix<4, 4>({0, 9, 3, 0, 9, 8, 0, 8, 1, 8, 5, 3, 0, 0, 5, 8});
    Matrix T = Matrix<4, 4>({0, 9, 1, 0, 9, 8, 8, 0, 3, 0, 5, 5, 0, 8, 3, 8});

    EXPECT_EQ(A.Transpose(), T);
}

TEST(MatrixTest, TransposeIdentity)
{
    Matrix A = IdentityMatrix;

    EXPECT_EQ(A.Transpose(), A);
}

TEST(MatrixTest, Determinant2x2)
{
    Matrix A = Matrix<2, 2>({1, 5, -3, 2});

    EXPECT_FLOAT_EQ(A.Determinant(), 17.0f);
}

TEST(MatrixTest, SubMatrix32)
{
    Matrix A = Matrix<3, 3>({1, 5, 0, -3, 2, 7, 0, 6, -3});
    Matrix E = Matrix<2, 2>({-3, 2, 0, 6});

    EXPECT_EQ(A.SubMatrix(0, 2), E);
}

TEST(MatrixTest, SubMatrix43)
{
    Matrix A = Matrix<4, 4>({-6, 1, 1, 6, -8, 5, 8, 6, -1, 0, 8, 2, -7, 1, -1, 1});
    Matrix E = Matrix<3, 3>({-6, 1, 6, -8, 8, 6, -7, -1, 1});

    EXPECT_EQ(A.SubMatrix(2, 1), E);
}

TEST(MatrixTest, TestMinor)
{
    Matrix A = Matrix<3, 3>({3, 5, 0, 2, -1, -7, 6, -1, 5});
    Matrix B = A.SubMatrix(1, 0);

    EXPECT_FLOAT_EQ(B.Determinant(), 25);
    EXPECT_FLOAT_EQ(A.Minor(1, 0), 25);
}

TEST(MatrixTest, Cofactors)
{
    Matrix A = Matrix<3, 3>({3, 5, 0, 2, -1, -7, 6, -1, 5});

    EXPECT_FLOAT_EQ(A.Minor(0, 0), -12);
    EXPECT_FLOAT_EQ(A.Cofactor(0, 0), -12);
    EXPECT_FLOAT_EQ(A.Minor(1, 0), 25);
    EXPECT_FLOAT_EQ(A.Cofactor(1, 0), -25);
}

TEST(MatrixTest, Determinant3x3)
{
    Matrix A = Matrix<3, 3>({1, 2, 6, -5, 8, -4, 2, 6, 4});

    EXPECT_FLOAT_EQ(A.Cofactor(0, 0), 56);
    EXPECT_FLOAT_EQ(A.Cofactor(0, 1), 12);
    EXPECT_FLOAT_EQ(A.Cofactor(0, 2), -46);
    EXPECT_FLOAT_EQ(A.Determinant(), -196);
}

TEST(MatrixTest, Determinant4x4)
{
    Matrix A = Matrix<4, 4>({-2, -8, 3, 5, -3, 1, 7, 3, 1, 2, -9, 6, -6, 7, 7, -9});

    EXPECT_FLOAT_EQ(A.Cofactor(0, 0), 690);
    EXPECT_FLOAT_EQ(A.Cofactor(0, 1), 447);
    EXPECT_FLOAT_EQ(A.Cofactor(0, 2), 210);
    EXPECT_FLOAT_EQ(A.Cofactor(0, 3), 51);
    EXPECT_FLOAT_EQ(A.Determinant(), -4071);
}

TEST(MatrixTest, CheckInvertible)
{
    Matrix A = Matrix<4, 4>({6, 4, 4, 4, 5, 5, 7, 6, 4, -9, 3, -7, 9, 1, 7, -6});

    EXPECT_FLOAT_EQ(A.Determinant(), -2120);
    EXPECT_EQ(A.IsInvertible(), true);
}

TEST(MatrixTest, CheckNotInvertible)
{
    Matrix A = Matrix<4, 4>({-4, 2, -2, -3, 9, 6, 2, 6, 0, -5, 1, -5, 0, 0, 0, 0});

    EXPECT_FLOAT_EQ(A.Determinant(), 0);
    EXPECT_EQ(A.IsInvertible(), false);
}

TEST(MatrixTest, InverseMatrix1)
{
    Matrix A = Matrix<4, 4>({-5, 2, 6, -8, 1, -5, 1, 8, 7, 7, -6, -7, 1, -3, 7, 4});
    Matrix B = A.Inverse();

    EXPECT_FLOAT_EQ(A.Determinant(), 532);
    EXPECT_FLOAT_EQ(A.Cofactor(2, 3), -160);
    EXPECT_FLOAT_EQ(B[3][2], -160.0 / 532.0);
    EXPECT_FLOAT_EQ(A.Cofactor(3, 2), 105);
    EXPECT_FLOAT_EQ(B[2][3], 105.0 / 532.0);

    Matrix C = Matrix<4, 4>({0.21805, 0.45113, 0.24060, -0.04511,
                             -0.80827, -1.45677, -0.44361, 0.52068,
                             -0.07895, -0.22368, -0.05263, 0.19737,
                             -0.52256, -0.81391, -0.30075, 0.30693});
    EXPECT_EQ(B, C);
}

TEST(MatrixTest, InverseMatrix2)
{
    Matrix A = Matrix<4, 4>({8, -5, 9, 2, 7, 5, 6, 1, -6, 0, 9, 6, -3, 0, -9, -4});
    Matrix B = Matrix<4, 4>({-0.15385, -0.15385, -0.28205, -0.53846,
                             -0.07692, 0.12308, 0.02564, 0.03077,
                             0.35897, 0.35897, 0.43590, 0.92308,
                             -0.69231, -0.69231, -0.76923, -1.92308});
    EXPECT_EQ(A.Inverse(), B);
}

TEST(MatrixTest, InverseMatrix3)
{
    Matrix A = Matrix<4, 4>({9, 3, 0, 9, -5, -2, -6, -3, -4, 9, 6, 4, -7, 6, 6, 2});
    Matrix B = Matrix<4, 4>({-0.04074, -0.07778, 0.14444, -0.22222,
                             -0.07778, 0.03333, 0.36667, -0.33333,
                             -0.02901, -0.14630, -0.10926, 0.12963,
                             0.17778, 0.06667, -0.26667, 0.33333});
    EXPECT_EQ(A.Inverse(), B);
}

TEST(MatrixTest, InverseEquality)
{
    Matrix A = Matrix<4, 4>({3, -9, 7, 3, 3, -8, 2, -9, -4, 4, 4, 1, -6, 5, -1, 1});
    Matrix B = Matrix<4, 4>({8, 2, 2, 2, 3, -1, 7, 0, 7, 0, 5, 5, 6, -2, 0, 5});
    Matrix C = A * B;

    EXPECT_EQ(C * B.Inverse(), A);
}

TEST(MatrixTransformations, Translation)
{
    Matrix transform = Translation(5, -3, 2);
    Tuple p = Point(-3, 4, 5);

    EXPECT_EQ(transform * p, Point(2, 1, 7));
}

TEST(MatrixTransformations, InvTranslation)
{
    Matrix transform = Translation(5, -3, 2);
    Matrix inv = transform.Inverse();
    Tuple p = Point(-3, 4, 5);

    EXPECT_EQ(inv * p, Point(-8, 7, 3));
}

TEST(MatrixTransformations, VectorTranslation)
{
    Matrix transform = Translation(5, -3, 2);
    Tuple v = Vector(-3, 4, 5);

    EXPECT_EQ(transform * v, v);
}

TEST(MatrixTransformations, PointScaling)
{
    Matrix transform = Scaling(2, 3, 4);
    Tuple p = Point(-4, 6, 8);

    EXPECT_EQ(transform * p, Point(-8, 18, 32));
}

TEST(MatrixTransformations, VectorScaling)
{
    Matrix transform = Scaling(2, 3, 4);
    Tuple v = Vector(-4, 6, 8);

    EXPECT_EQ(transform * v, Vector(-8, 18, 32));
}

TEST(MatrixTransformations, InverseScaling)
{
    Matrix transform = Scaling(2, 3, 4);
    Matrix inv = transform.Inverse();
    Tuple v = Vector(-4, 6, 8);

    EXPECT_EQ(inv * v, Vector(-2, 2, 2));
}

TEST(MatrixTransformations, Reflection)
{
    Matrix transform = Scaling(-1, 1, 1);
    Tuple p = Point(2, 3, 4);

    EXPECT_EQ(transform * p, Point(-2, 3, 4));
}

TEST(MatrixTransformations, XRotation)
{
    Tuple p = Point(0, 1, 0);
    Matrix halfQuarter = RotationX(M_PI / 4);
    Matrix fullQuarter = RotationX(M_PI / 2);

    EXPECT_EQ(halfQuarter * p, Point(0, sqrtf(2.0f) / 2, sqrtf(2.0f) / 2));
    EXPECT_EQ(fullQuarter * p, Point(0, 0, 1));
}

TEST(MatrixTransformations, XInvRotation)
{
    Tuple p = Point(0, 1, 0);
    Matrix halfQuarter = RotationX(M_PI / 4);
    Matrix inv = halfQuarter.Inverse();

    EXPECT_EQ(inv * p, Point(0, sqrtf(2.0f) / 2, -sqrtf(2.0f) / 2));
}

TEST(MatrixTransformations, YRotation)
{
    Tuple p = Point(0, 0, 1);
    Matrix halfQuarter = RotationY(M_PI / 4);
    Matrix fullQuarter = RotationY(M_PI / 2);

    EXPECT_EQ(halfQuarter * p, Point(sqrtf(2.0f) / 2, 0, sqrtf(2.0f) / 2));
    EXPECT_EQ(fullQuarter * p, Point(1, 0, 0));
}

TEST(MatrixTransformations, ZRotation)
{
    Tuple p = Point(0, 1, 0);
    Matrix halfQuarter = RotationZ(M_PI / 4);
    Matrix fullQuarter = RotationZ(M_PI / 2);

    EXPECT_EQ(halfQuarter * p, Point(-sqrtf(2.0f) / 2, sqrtf(2.0f) / 2, 0));
    EXPECT_EQ(fullQuarter * p, Point(-1, 0, 0));
}

TEST(MatrixTransformations, ShearXY)
{
    Matrix transform = Shearing(1, 0, 0, 0, 0, 0);
    Tuple p = Point(2, 3, 4);

    EXPECT_EQ(transform * p, Point(5, 3, 4));
}

TEST(MatrixTransformations, ShearXZ)
{
    Matrix transform = Shearing(0, 1, 0, 0, 0, 0);
    Tuple p = Point(2, 3, 4);

    EXPECT_EQ(transform * p, Point(6, 3, 4));
}

TEST(MatrixTransformations, ShearYX)
{
    Matrix transform = Shearing(0, 0, 1, 0, 0, 0);
    Tuple p = Point(2, 3, 4);

    EXPECT_EQ(transform * p, Point(2, 5, 4));
}

TEST(MatrixTransformations, ShearYZ)
{
    Matrix transform = Shearing(0, 0, 0, 1, 0, 0);
    Tuple p = Point(2, 3, 4);

    EXPECT_EQ(transform * p, Point(2, 7, 4));
}

TEST(MatrixTransformations, ShearZX)
{
    Matrix transform = Shearing(0, 0, 0, 0, 1, 0);
    Tuple p = Point(2, 3, 4);

    EXPECT_EQ(transform * p, Point(2, 3, 6));
}

TEST(MatrixTransformations, ShearZY)
{
    Matrix transform = Shearing(0, 0, 0, 0, 0, 1);
    Tuple p = Point(2, 3, 4);

    EXPECT_EQ(transform * p, Point(2, 3, 7));
}

TEST(MatrixTransformations, TransformationSequence)
{
    Tuple p = Point(1, 0, 1);
    Matrix A = RotationX(M_PI / 2);
    Matrix B = Scaling(5, 5, 5);
    Matrix C = Translation(10, 5, 7);

    Tuple p2 = A * p;
    EXPECT_EQ(p2, Point(1, -1, 0));

    Tuple p3 = B * p2;
    EXPECT_EQ(p3, Point(5, -5, 0));

    Tuple p4 = C * p3;
    EXPECT_EQ(p4, Point(15, 0, 7));
}

TEST(MatrixTransformations, ChainTransformation)
{
    Tuple p = Point(1, 0, 1);
    Matrix A = RotationX(M_PI / 2);
    Matrix B = Scaling(5, 5, 5);
    Matrix C = Translation(10, 5, 7);
    Matrix T = C * B * A;

    EXPECT_EQ(T * p, Point(15, 0, 7));
}

TEST(MatrixTransformations, DefaultViewTransform)
{
    Point from = Point(0, 0, 0);
    Point to = Point(0, 0, -1);
    Vector up = Vector(0, 1, 0);
    Matrix t = ViewTransform(from, to, up);

    EXPECT_EQ(t, IdentityMatrix);
}

TEST(MatrixTransformations, ViewTransformPositiveZ)
{
    Point from = Point(0, 0, 0);
    Point to = Point(0, 0, 1);
    Vector up = Vector(0, 1, 0);
    Matrix t = ViewTransform(from, to, up);

    EXPECT_EQ(t, Scaling(-1, 1, -1));
}

TEST(MatrixTransformations, ViewMovesWorld)
{
    Point from = Point(0, 0, 8);
    Point to = Point(0, 0, 0);
    Vector up = Vector(0, 1, 0);
    Matrix t = ViewTransform(from, to, up);

    EXPECT_EQ(t, Translation(0, 0, -8));
}

TEST(MatrixTransformations, ArbitraryView)
{
    Point from = Point(1, 3, 2);
    Point to = Point(4, -2, 8);
    Vector up = Vector(1, 1, 0);
    Matrix t = ViewTransform(from, to, up);

    Matrix expected = Matrix<4, 4>({-0.50709, 0.50709, 0.67612, -2.36643,
                                    0.76772, 0.60609, 0.12122, -2.82843,
                                    -0.35857, 0.59761, -0.71714, 0.00000,
                                    0.00000, 0.00000, 0.00000, 1.00000});

    EXPECT_EQ(t, expected);
}
