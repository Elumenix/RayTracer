#include <gtest/gtest.h>
#include <memory>
#include "../src/features/Pattern.h"
#include "../src/features/Material.h"
#include "../src/features/Sphere.h"

TEST(PatternTest, TestConstants) {
    Color b = Color(0,0,0);
    Color w = Color(1,1,1);

    EXPECT_EQ(Black, b);
    EXPECT_EQ(White, w);
}

TEST(PatternTest, StripePatternColors) {
    //StripePattern pattern = StripePattern(White, Black);
    Pattern pattern = Pattern(White, Black);

    EXPECT_EQ(pattern.a, White);
    EXPECT_EQ(pattern.b, Black);
}

TEST(PatternTest, StripeConstantY) {
    //StripePattern pattern = StripePattern(White, Black);
    Pattern pattern = Pattern(White, Black);

    EXPECT_EQ(pattern.StripeAt(Point(0,0,0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0,1,0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0,2,0)), White);
}

TEST(PatternTest, StripeConstantZ) {
    //StripePattern pattern = StripePattern(White, Black);
    Pattern pattern = Pattern(White, Black);

    EXPECT_EQ(pattern.StripeAt(Point(0,0,0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0,0,1)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0,0,2)), White);
}

TEST(PatternTest, StripeAlternateX) {
        //StripePattern pattern = StripePattern(White, Black);
        Pattern pattern = Pattern(White, Black);

    EXPECT_EQ(pattern.StripeAt(Point(0,0,0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(0.9,0,0)), White);
    EXPECT_EQ(pattern.StripeAt(Point(1,0,0)), Black);
    EXPECT_EQ(pattern.StripeAt(Point(-0.1,0,0)), Black);
    EXPECT_EQ(pattern.StripeAt(Point(-1,0,0)), Black);
    EXPECT_EQ(pattern.StripeAt(Point(-1.1,0,0)), White);
}

TEST(PatternTest, LightingWithPatternApplied) {
    Sphere s;
    Material m = Material(White, 1, 0, 0, 0);
    m.pattern = std::make_unique<Pattern>(Pattern(White, Black));
    s.material = std::move(m);
    Vector eye = Vector(0,0,-1);
    Vector normal = Vector (0,0,-1);
    Light light = Light(Point(0,0,-10), Color(1,1,1));

    Color c1 = s.lighting(light, Point(0.9, 0, 0), eye, normal, false);
    EXPECT_EQ(c1, Color(1,1,1));
    Color c2 = s.lighting(light, Point(1.1, 0, 0), eye, normal, false);
    EXPECT_EQ(c2, Color(0,0,0));
}
