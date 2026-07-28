#include "World.h"
#include "Sphere.h"
#include "Transformations.h"
#include "IntersectionList.h"
#include "Ray.h"
#include "Constants.h"

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
        xs.Reserve(shapes.size() * 2); // rough guess. There is at max 2 intersections per shape
        for (auto &shape : shapes)
        {
            shape->Intersects(r, xs);
        }

        xs.Sort(); // result does need to be sorted in ascending order
        return xs;
    }

    Color World::ShadeHit(const Comps &comp, int remaining, float contribution) const
    {
        const Material &mat = comp.object->material;

        // Most parameters here are actually light-independant, and so can be calculated ahead of time
        Color reflected = ReflectedColor(comp, remaining, contribution);
        Color refracted = RefractedColor(comp, remaining, contribution);

        // Fresnel needs to be applied if the material is both reflective and transparent
        Color c;
        if (mat.reflective > 0 && mat.transparency > 0)
        {
            float reflectance = SchlickFresnel(comp);
            c = reflected * reflectance + refracted * (1 - reflectance);
        }
        else
        {
            c = reflected + refracted;
        }

        // Diffuse is calculated per light and added to the result
        for (const Light &light : lights)
        {
            bool isShadowed = IsShadowed(comp.overPoint, light);
            c += light.Lighting(*comp.object, comp.overPoint, comp.eye, comp.normal, isShadowed);
        }

        return c;
    }

    Color World::ColorAt(const Ray &r, int remaining, float contribution) const
    {
        // If what we're sampling doesn't contribute enough for a noticable difference, return early
        if (contribution < FALLOFF)
        {
            return Color(0, 0, 0);
        }

        IntersectionList xs = IntersectWorld(r);
        const Intersection *i = xs.Hit();

        if (i == nullptr)
        {
            return Color(0, 0, 0);
        }

        Comps comp = PrepareComputation(*i, r, xs);
        Color c = ShadeHit(comp, remaining, contribution);
        return c;
    }

    Rendering::Color World::ReflectedColor(const Rendering::Comps &comp, int remaining, float contribution) const
    {
        const float reflective = comp.object->material.reflective;

        if (reflective < EPSILON || remaining <= 0)
        {
            return Color(0, 0, 0);
        }

        Ray reflectRay(comp.overPoint, comp.reflect);
        Color color = ColorAt(reflectRay, remaining - 1, contribution * reflective);

        return color * reflective;
    }

    Rendering::Color World::RefractedColor(const Rendering::Comps &comp, int remaining, float contribution) const
    {
        const float transparent = comp.object->material.transparency;
        if (transparent < EPSILON || remaining <= 0)
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

        float cosT = std::sqrt(1.0 - sin2T);
        Vector direction = comp.normal * (ratio * cosI - cosT) - comp.eye * ratio;

        // Create new ray to get the refracted color
        Ray refractRay = Ray(comp.underPoint, direction.Normalized());
        Color color = ColorAt(refractRay, remaining - 1, contribution * transparent);

        // Account for opacity
        return color * transparent;
    }

    bool World::IsShadowed(const Point &p, const Light &light) const
    {
        Vector v = light.position - p;
        float distance = v.Magnitude();
        Vector direction = v.Normalized();

        Ray r = Ray(p, direction);

        for (auto &shape : shapes)
        {
            // We can skip transparent objects by default
            // Transparent Refractive surfaces would apparently make caustics but that's too complex right now
            if (shape->material.transparency >= 1.0f)
                continue;

            IntersectionList local;
            shape->Intersects(r, local);

            for (const Intersection &i : local)
            {
                if (i.t > 0 && i.t < distance)
                    return true; // early exit — no need to test remaining shapes
            }
        }

        // Nothing got in the way of this light
        return false;
    }
}
