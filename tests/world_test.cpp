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
    EXPECT_EQ(*w.shapes[0], s1);
    EXPECT_EQ(*w.shapes[1], s2);
}

TEST(WorldTest, WorldIntersections) {
    World w = World::Default();
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    IntersectionList xs = w.intersectWorld(r);

    EXPECT_EQ(xs.size(), 4);
    EXPECT_FLOAT_EQ(xs[0]->t, 4.0f);
    EXPECT_FLOAT_EQ(xs[1]->t, 4.5f);
    EXPECT_FLOAT_EQ(xs[2]->t, 5.5f);
    EXPECT_FLOAT_EQ(xs[3]->t, 6.0f);
}

TEST(WorldTest, IntersectionShading) {
    World w = World::Default();
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    Shape& shape = *w.shapes[0];
    Intersection i = Intersection(4, &shape);
    Comps comps = prepare_computation(i, r);
    Color c = w.shade_hit(comps);

    EXPECT_EQ(c, Color(0.38066, 0.47583, 0.2855));
}

TEST(WorldTest, IntersectionShadingInside) {
    World w = World::Default();
    w.lights[0] = Light(Point(0,0.25,0), Color(1,1,1));
    Ray r = Ray(Point(0,0,0), Vector(0,0,1));
    Shape& shape = *w.shapes[1];
    Intersection i = Intersection(0.5f, &shape);
    Comps comps = prepare_computation(i, r);
    Color c = w.shade_hit(comps);

    EXPECT_EQ(c, Color(0.90498f, 0.90498f, 0.90498f));
}

TEST(WorldTest, RayMiss) {
    World w = World::Default();
    Ray r = Ray(Point(0,0,-5),Vector(0,1,0));
    Color c = w.color_at(r);

    EXPECT_EQ(c, Color(0,0,0));
}

TEST(WorldTest, RayHitColor) {
    World w = World::Default();
    Ray r = Ray(Point(0,0,-5), Vector(0,0,1));
    Color c = w.color_at(r);

    EXPECT_EQ(c, Color(0.38066f, 0.47583f, 0.2855f));
}

TEST(WorldTest, BehindRayIntersection) {
    World w = World::Default();
    Shape& outer = *w.shapes[0];
    outer.material.ambient = 1;
    Shape& inner = *w.shapes[1];
    inner.material.ambient = 1;
    Ray r = Ray(Point(0,0,0.75), Vector(0,0,-1));
    Color c = w.color_at(r);

    EXPECT_EQ(c, inner.material.color);
}
