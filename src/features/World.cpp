#include "World.h"

using namespace Math;
using namespace Rendering;

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
    s2.transform = Transformations::Scaling(0.5,0.5,0.5);
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
        bool isShadowed = is_shadowed(comp.over_point, light);
        c += light.lighting(*comp.object, comp.over_point, comp.eye, comp.normal, isShadowed);
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

bool World::is_shadowed(Point p, Light light) const {
    Vector v = light.position - p;
    float distance = v.Magnitude();
    Vector direction = v.Normalized();

    Ray r = Ray(p, direction);
    IntersectionList intersections = intersectWorld(r);
    const Intersection* h = intersections.hit();

    return h != nullptr && h->t < distance;
}
