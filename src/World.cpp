#include "World.h"
#include "Sphere.h"
#include "Transformations.h"
#include "IntersectionList.h"
#include "Ray.h"

using namespace Math;
using namespace Rendering;
using namespace Transformations;

namespace Scene
{
    World World::Default()
    {
        World w = World();

        Light light = Light(Point(-10, 10, -10), Color(1, 1, 1));
        w.Add(light);

        Sphere s1;
        s1.material.color = Color(0.8, 1.0, 0.6);
        s1.material.diffuse = 0.7;
        s1.material.specular = 0.2;
        w.Add(std::move(s1));

        Sphere s2;
        s2.transform = Scaling(0.5, 0.5, 0.5);
        w.Add(std::move(s2));

        return w;
    }

    const IntersectionList World::IntersectWorld(const Ray &r) const
    {
        IntersectionList xs;
        for (auto &shape : shapes)
        {
            xs.Merge(shape->Intersects(r));
        }

        return xs;
    }

    Color World::ShadeHit(const Comps &comp) const
    {
        Color c = Color(0, 0, 0);

        for (const Light &light : lights)
        {
            bool isShadowed = IsShadowed(comp.over_point, light);
            c += light.Lighting(*comp.object, comp.over_point, comp.eye, comp.normal, isShadowed);
        }

        return c;
    }

    Color World::ColorAt(const Ray &r) const
    {
        IntersectionList xs = IntersectWorld(r);
        const Intersection *i = xs.Hit();

        if (i == nullptr)
        {
            return Color(0, 0, 0);
        }

        Comps comp = PrepareComputation(*i, r);
        Color c = ShadeHit(comp);
        return c;
    }

    bool World::IsShadowed(const Point &p, const Light &light) const
    {
        Vector v = light.position - p;
        float distance = v.Magnitude();
        Vector direction = v.Normalized();

        Ray r = Ray(p, direction);
        IntersectionList intersections = IntersectWorld(r);
        const Intersection *h = intersections.Hit();

        return h != nullptr && h->t < distance;
    }
}
