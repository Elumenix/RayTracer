#include <gtest/gtest.h>
#include "../src/features/World.h"

TEST(WorldTest, CreateWorld) {
    World w;

    EXPECT_EQ(w.lights.size(), 0);
    EXPECT_EQ(w.shapes.size(), 0);
}

TEST(WorldTest, DefaultWorld) {
    World w = World::Default();
    Light l = Light(Point(-10,10,-10), Color(1,1,1));
    Sphere s1;
    s1.material.color = Color(0.8,1.0,0.6);
    s1.material.diffuse = 0.7f;
    s1.material.specular = 0.2f;
    Sphere s2;
    s2.transform = transformations::scaling(0.5,0.5,0.5);

    EXPECT_EQ(w.lights[0], l);
    // Need overload here for equality on base shape class
    EXPECT_EQ(*w.shapes[0], s1);
    EXPECT_EQ(*w.shapes[1], s2);
}
