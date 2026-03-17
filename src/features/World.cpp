#include "World.h"

World World::Default()
{
    World w = World();

    Light light = Light(Point(-10,10,-10), Color(1,1,1));
    w.add(light);
    
    Sphere s1;
    s1.material.color = Color(0.8,1.0,0.6);
    s1.material.diffuse = 0.7;
    s1.material.specular = 0.2;
    w.add(std::move(s1));

    Sphere s2;
    s2.transform = transformations::scaling(0.5,0.5,0.5);
    w.add(std::move(s2));
    
    return w;
}

const IntersectionList World::intersectWorld(const Ray r) const
{
    IntersectionList xs;
    for(auto& shape : shapes) {
        xs.merge(shape->intersects(r));
    }

    return xs;
}

Color World::shade_hit(const Comps &comp) const
{
    Color c = Color(0,0,0);

    for (const Light& light : lights) {
        c += comp.object->lighting(light, comp.point, comp.eye, comp.normal);
    }

    return c;
}

Color World::color_at(const Ray r) const
{
    IntersectionList xs = intersectWorld(r);
    const Intersection* i = xs.hit();
    
    if (i == nullptr) {
        return Color(0,0,0);
    }

    Comps comp = prepare_computation(*i, r);
    Color c = shade_hit(comp);
    return c;
}
