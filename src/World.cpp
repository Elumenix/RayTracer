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

    Color World::ShadeHit(const Comps &comp, int remaining) const
    {
        Color c = Color(0, 0, 0);

        for (const Light &light : lights)
        {
            bool isShadowed = IsShadowed(comp.overPoint, light);
            Color surface = light.Lighting(*comp.object, comp.overPoint, comp.eye, comp.normal, isShadowed);
            Color reflected = ReflectedColor(comp, remaining);
            Color refracted = RefractedColor(comp, remaining);

            Material mat = comp.object->material;

            // Fresnel needs to be applied if the materail is both reflective and transparent
            if (mat.reflective > 0 && mat.transparency > 0)
            {
                float reflectance = SchlickFresnel(comp);
                c += surface + reflected * reflectance + refracted * (1 - reflectance);
            }
            else
            {
                c += surface + reflected + refracted;
            }
        }

        return c;
    }

    Color World::ColorAt(const Ray &r, int remaining) const
    {
        IntersectionList xs = IntersectWorld(r);
        const Intersection *i = xs.Hit();

        if (i == nullptr)
        {
            return Color(0, 0, 0);
        }

        Comps comp = PrepareComputation(*i, r);
        Color c = ShadeHit(comp, remaining);
        return c;
    }

    Rendering::Color World::ReflectedColor(const Rendering::Comps &comp, int remaining) const
    {
        if (comp.object->material.reflective == 0 || remaining <= 0)
        {
            return Color(0, 0, 0);
        }

        Ray reflectRay(comp.overPoint, comp.reflect);
        Color color = ColorAt(reflectRay, remaining - 1);

        return color * comp.object->material.reflective;
    }

    Rendering::Color World::RefractedColor(const Rendering::Comps &comp, int remaining) const
    {
        if (remaining <= 0 || comp.object->material.transparency == 0)
        {
            return Color(0, 0, 0);
        }

        // Snells law (Angle of refracted ray)
        float ratio = comp.n1 / comp.n2;
        float cosI = DotProduct(comp.eye, comp.normal);
        float sin2T = (ratio * ratio) * (1 - cosI * cosI);

        // There is total internal reflection
        if (sin2T > 1)
        {
            return Color(0, 0, 0);
        }

        float cosT = sqrtf(1.0 - sin2T);
        Vector direction = comp.normal * (ratio - cosI - cosT) - comp.eye * ratio;

        // Create new ray to get the refracted color
        Ray refractRay = Ray(comp.underPoint, direction);
        Color color = ColorAt(refractRay, remaining - 1);

        // Account for opacity
        return color * comp.object->material.transparency;
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
