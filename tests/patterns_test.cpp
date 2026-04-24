#include <gtest/gtest.h>
#include <memory>
#include "../src/features/Pattern.h"
#include "../src/features/Material.h"
#include "../src/features/Sphere.h"

TEST(PatternTest, TestConstants)
{
    Color b = Color(0, 0, 0);
    Color w = Color(1, 1, 1);

    EXPECT_EQ(Black, b);
    EXPECT_EQ(White, w);
}

TEST(PatternTest, StripePatternColors)
{
    StripePattern pattern(White, Black);

    EXPECT_EQ(pattern.a, White);
    EXPECT_EQ(pattern.b, Black);
}

TEST(PatternTest, StripeConstantY)
{
    StripePattern pattern(White, Black);

    EXPECT_EQ(pattern.StripeAt(Point(0, 0, 0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0, 1, 0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0, 2, 0)), White);
}

TEST(PatternTest, StripeConstantZ)
{
    StripePattern pattern(White, Black);

    EXPECT_EQ(pattern.StripeAt(Point(0, 0, 0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0, 0, 1)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0, 0, 2)), White);
}

TEST(PatternTest, StripeAlternateX)
{
    StripePattern pattern(White, Black);

    EXPECT_EQ(pattern.StripeAt(Point(0, 0, 0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0.9, 0, 0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(1, 0, 0)), Black);
    EXPECT_EQ(pattern.StripeAt(Point(-0.1, 0, 0)), Black);
    EXPECT_EQ(pattern.StripeAt(Point(-1, 0, 0)), Black);
    EXPECT_EQ(pattern.StripeAt(Point(-1.1, 0, 0)), White);
}

TEST(PatternTest, LightingWithPatternApplied)
{
    Sphere s;
    Material m = Material(White, 1, 0, 0, 0);
    m.pattern = MakePattern<StripePattern>();
    s.material = std::move(m);
    Vector eye = Vector(0, 0, -1);
    Vector normal = Vector(0, 0, -1);
    Light light = Light(Point(0, 0, -10), Color(1, 1, 1));

    Color c1 = light.lighting(s, Point(0.9, 0, 0), eye, normal, false);
    EXPECT_EQ(c1, Color(1, 1, 1));
    Color c2 = light.lighting(s, Point(1.1, 0, 0), eye, normal, false);
    EXPECT_EQ(c2, Color(0, 0, 0));
}

TEST(PatternTest, StripesObjectTransform)
{
    Sphere object;
    object.transform = transformations::scaling(2, 2, 2);
    StripePattern p(White, Black);
    Color c = p.SampleAt(object, Point(1.5, 0, 0));

    EXPECT_EQ(c, White);
}

TEST(PatternTest, StripesPatternTransform)
{
    Sphere object;
    StripePattern pattern(White, Black);
    pattern.transform = transformations::scaling(2, 2, 2);
    Color c = pattern.SampleAt(object, Point(1.5, 0, 0));

    EXPECT_EQ(c, White);
}

TEST(PatternTest, StripesPatternBothTransform)
{
    Sphere object;
    object.transform = transformations::scaling(2, 2, 2);
    StripePattern pattern(White, Black);
    pattern.transform = transformations::translation(0.5, 0, 0);
    Color c = pattern.SampleAt(object, Point(2.5, 0, 0));

    EXPECT_EQ(c, White);
}

TEST(PatternTest, DefaultPatternTransformation)
{
    TestPattern pattern;
    EXPECT_EQ(pattern.transform, IdentityMatrix);
}

TEST(PatternTest, AssigningPatternTransformation)
{
    TestPattern pattern;
    pattern.transform = transformations::translation(1, 2, 3);

    EXPECT_EQ(pattern.transform, transformations::translation(1, 2, 3));
}

TEST(PatternTest, PatternObjectTransform)
{
    Sphere s;
    s.transform = transformations::scaling(2, 2, 2);
    TestPattern pattern;
    Color c = pattern.SampleAt(s, Point(2, 3, 4));

    EXPECT_EQ(c, Color(1, 1.5, 2));
}

TEST(PatternTest, PatternTransform)
{
    Sphere shape;
    TestPattern pattern;
    pattern.transform = transformations::scaling(2, 2, 2);
    Color c = pattern.SampleAt(shape, Point(2, 3, 4));

    EXPECT_EQ(c, Color(1, 1.5, 2));
}

TEST(PatternTest, ObjectAndPatternTransform)
{
    Sphere shape;
    shape.transform = transformations::scaling(2, 2, 2);
    TestPattern pattern;
    pattern.transform = transformations::translation(0.5, 1, 1.5);
    Color c = pattern.SampleAt(shape, Point(2.5, 3, 3.5));

    EXPECT_EQ(c, Color(0.75, 0.5, 0.25));
}

TEST(PatternTest, GradientPatternTest)
{
    Sphere shape;
    GradientPattern pattern;

    EXPECT_EQ(pattern.SampleAt(shape, Point(0, 0, 0)), White);
    EXPECT_EQ(pattern.SampleAt(shape, Point(0.25, 0, 0)), Color(0.75, 0.75, 0.75));
    EXPECT_EQ(pattern.SampleAt(shape, Point(0.5, 0, 0)), Color(0.5, 0.5, 0.5));
    EXPECT_EQ(pattern.SampleAt(shape, Point(0.75, 0, 0)), Color(0.25, 0.25, 0.25));
}

TEST(PatternTest, RingTest)
{
    Sphere shape;
    RingPattern pattern;

    EXPECT_EQ(pattern.SampleAt(shape, Point(0, 0, 0)), White);
    EXPECT_EQ(pattern.SampleAt(shape, Point(1, 0, 0)), Black);
    EXPECT_EQ(pattern.SampleAt(shape, Point(0, 0, 1)), Black);
    EXPECT_EQ(pattern.SampleAt(shape, Point(0.708, 0, 0.708)), Black);
}

TEST(PatternTest, CheckersTestX)
{
    Sphere s;
    CheckerPattern pattern;

    EXPECT_EQ(pattern.SampleAt(s, Point(0, 0, 0)), White);
    EXPECT_EQ(pattern.SampleAt(s, Point(0.99, 0, 0)), White);
    EXPECT_EQ(pattern.SampleAt(s, Point(1.01, 0, 0)), Black);
}

TEST(PatternTest, CheckersTestY)
{
    Sphere s;
    CheckerPattern pattern;

    EXPECT_EQ(pattern.SampleAt(s, Point(0, 0, 0)), White);
    EXPECT_EQ(pattern.SampleAt(s, Point(0, 0.99, 0)), White);
    EXPECT_EQ(pattern.SampleAt(s, Point(0, 1.01, 0)), Black);
}

TEST(PatternTest, CheckersTestZ)
{
    Sphere s;
    CheckerPattern pattern;

    EXPECT_EQ(pattern.SampleAt(s, Point(0, 0, 0)), White);
    EXPECT_EQ(pattern.SampleAt(s, Point(0, 0, 0.99)), White);
    EXPECT_EQ(pattern.SampleAt(s, Point(0, 0, 1.01)), Black);
}
