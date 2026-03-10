#include <gtest/gtest.h>
#include "../src/features/Sphere.h"
#include "../src/features/Intersection.h"
#include "../src/features/IntersectionList.h"
#include "../src/features/Color.h"
#include "../src/features/Light.h"
#include "../src/features/Material.h"

TEST(ShapeTest, DefaultShapeTransformation) {
    Sphere s;

    EXPECT_EQ(s.transform, IdentityMatrix);
}

TEST(ShapeTest, ChangeShapeTransformation) {
    Sphere s;
    Matrix t = transformations::translation(2,3,4);
    s.transform = t;

    EXPECT_EQ(s.transform, t);
}    

TEST(ShapeTest, SphereNormalX) {
    Sphere s;
    Vector n = s.normal_at(Point(1,0,0));

    EXPECT_EQ(n, Vector(1,0,0));
}

TEST(ShapeTest, SphereNormalY) {
    Sphere s;
    Vector n = s.normal_at(Point(0,1,0));

    EXPECT_EQ(n, Vector(0,1,0));
}

TEST(ShapeTest, SphereNormalZ) {
    Sphere s;
    Vector n = s.normal_at(Point(0,0,1));

    EXPECT_EQ(n, Vector(0,0,1));
}

TEST(ShapeTest, SphereNormalNonaxial) {
    Sphere s;
    Point nonaxialPoint = Point(sqrtf(3.0f)/3, sqrtf(3.0f)/3, sqrtf(3.0f)/3);
    Vector n = s.normal_at(nonaxialPoint);

    EXPECT_EQ(n, Vector(sqrtf(3.0f)/3,sqrtf(3.0f)/3,sqrtf(3.0f)/3));
}

TEST(ShapeTest, SphereNormalIsNormalized) {
    Sphere s;
    Vector n = s.normal_at(Vector(sqrtf(3.0f)/3,sqrtf(3.0f)/3,sqrtf(3.0f)/3));

    EXPECT_EQ(n, n.Normalized());
}

TEST(ShapeTest, SphereNormalTranslated) {
    Sphere s;
    s.transform = transformations::translation(0,1,0);
    Vector n = s.normal_at(Point(0,1.70711,-0.70711));

    EXPECT_EQ(n, Vector(0,0.70711,-0.70711));
}

TEST(ShapeTest, SphereTransformed) {
    Sphere s;
    Matrix m = transformations::scaling(1,0.5,1) * transformations::rotationZ(M_PI / 5);
    s.transform = m;
    Vector n = s.normal_at(Vector(0, sqrtf(2.0f)/2,-sqrtf(2.0f)/2)); 

    EXPECT_EQ(n, Vector(0, 0.97014, -0.24254));
}
