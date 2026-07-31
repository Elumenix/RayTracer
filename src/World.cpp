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
    // Establishes static variable from World.h so it can be used
    thread_local std::vector<Rendering::IntersectionList> World::depthLists;

    // Helper function to get a quick test world
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

    /// @brief Precomputes all intersections in the scene for a ray
    /// @param r The ray we're watching to see if hits anything
    const IntersectionList World::IntersectWorld(const Ray &r) const
    {
        IntersectionList xs;
        xs.Reserve(shapes.size() * 2); // Rough Guess. This is a safe upper bound. There is at max 2 intersections per shape
        IntersectWorld(r, xs);         // Overload is used so we can continue using the same list
        return xs;
    }

    /// @brief Precomputes all intersections in the scene for a ray
    /// @param r The ray we're watching to see if hits anything
    /// @param xs A referenced IntersectionList, passed so that additional memory doesn't need to be created for every call
    void World::IntersectWorld(const Rendering::Ray &r, Rendering::IntersectionList &xs) const
    {
        for (auto &shape : shapes)
        {
            shape->Intersects(r, xs);
        }

        xs.Sort(); // Result needs to be sorted in ascending order so that Hit and GetNValues work
    }

    /// @brief Precomputes all computations that the ray needs. This is the starting function of the ray tracer for each recursion depth
    /// @param r The ray that we want to get the hit color of
    /// @param remaining Tracks how many more times the ray is allowed to bounce. This is a max value, the ray can terminate sooner
    /// @param contribution Keeps track of how much this ray contributes to the color of the current pixel. This is determined by ReflectedColor and RefractedColor
    /// @return The total color that the ray returns after all intersection
    Color World::ColorAt(const Ray &r, int remaining, float contribution) const
    {
        // If what we're sampling doesn't contribute enough for a noticable difference, return early
        if (contribution < FALLOFF)
        {
            return Color(0, 0, 0);
        }

        // If this is the first time this thread has ever made it to ColorAt, establish the depthLists now (becasue this is the only time we'll know the max recursion depth)
        // Second condition is for tests where I might call colorAt multiple times with different recursion depths; It will never happen naturally
        if (depthLists.empty() || remaining >= depthLists.size())
        {
            depthLists.resize(remaining + 1);

            for (int i = 0; i <= remaining; i++)
            {
                depthLists[i].Reserve(shapes.size() * 2); // Safe upper bounds for the number of intersection in an intersectionList, as a ray passes through a shape a max of 2 times
            }
        }

        // The intersection list for this level is retrieved, then cleared because reflections from earlier levels may have already visited this level and used it
        IntersectionList &xs = depthLists[remaining];
        xs.Clear();

        // Find all intersection points for this ray, then get the first one it would hit
        IntersectWorld(r, xs);
        const Intersection *i = xs.Hit();

        if (i == nullptr)
        {
            return Color(0, 0, 0);
        }

        Comps comp = PrepareComputation(*i, r, xs);
        Color c = ShadeHit(comp, remaining, contribution);
        return c;
    }

    /// @brief Combines color values to figure out what color should be returned by the ray.
    /// @param comp Holds precomputed data related to the ray and it's intersections
    /// @param remaining Tracks how many more times this ray is allowed to reflect/recurse. This is an upper limit.
    /// @param contribution Tracks an estimated percentage of how much the color from this function will affect the pixel.
    /// @return The summed color value a computed ray after all reflections
    Color World::ShadeHit(const Comps &comp, int remaining, float contribution) const
    {
        const Material &mat = comp.object->material;

        // Most parameters here are actually light-independant, and so can be calculated ahead of time
        // RelectedColor and Refracted color will both reflect the ray and lead to recursion if remaining > 0
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
            bool isShadowed = IsShadowed(comp.overPoint, light, remaining);
            c += light.Lighting(*comp.object, comp.overPoint, comp.eye, comp.normal, isShadowed);
        }

        return c;
    }

    /// @brief Gets the color of a ray that reflects at the current intersection
    /// @param comp Holds precomputed data related to the ray and it's intersections
    /// @param remaining Tracks how many more times this ray is allowed to reflect/recurse. This is an upper limit
    /// @param contribution Tracks an estimated percentage of how much the color from this function will affect the pixel
    /// @return The total color from a reflection at this intersection after all recursions
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

    /// @brief Gets the color of a ray that refracts at the current intersection
    /// @param comp Holds precomputed data related to the ray and it's intersections
    /// @param remaining Tracks how many more times this ray is allowed to reflect/recurse. This is an upper limit
    /// @param contribution Tracks an estimated percentage of how much the color from this function will affect the pixel
    /// @return The total color from a reflection at this intersection after all recursions
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

    /// @brief Determines if the point in the scene is in shadow
    /// @param p Represents a point in world space in the scene
    /// @param light The light object in the scene we want to see if reaches the point
    /// @param remaining Tracks how many more times this ray is allowed to reflect/recurse. This is an upper limit
    /// @return Whether the point is in a shadow created by this light
    bool World::IsShadowed(const Point &p, const Light &light, int remaining) const
    {
        Vector v = light.position - p;
        float distance = v.Magnitude();
        Vector direction = v.Normalized();

        // New ray is created to the light, so we can see if any shapes intersect it beforehand
        Ray r = Ray(p, direction);

        for (auto &shape : shapes)
        {
            // We can skip transparent objects by default
            // Transparent Refractive surfaces would apparently make caustics but that's too complex right now
            if (shape->material.transparency >= 1.0f)
                continue;

            if (remaining < depthLists.size())
            {
                // Because the previous intersections for this level will no longer be used, we can reuse the memory blocks from depthLists
                // This is already allocated to the thread, so it won't have problems with creating mutexes as this function keeps getting called
                IntersectionList &xs = depthLists[remaining];
                xs.Clear();
                shape->Intersects(r, xs); // Get intersections from shape if the ray intersects it

                // If an intersection is between the light and the point, then the point is in shadow
                for (const Intersection &i : xs)
                {
                    if (i.t > 0 && i.t < distance)
                        return true; // early exit — no need to test remaining shapes
                }
            }
            else
            {
                // This path still needs to be set up for unit tests that don't include colorAt to not fail.
                // The logic is the same in both blocks, the memory is just created locally in this one instead of having already existed
                IntersectionList local;
                shape->Intersects(r, local);

                for (const Intersection &i : local)
                {
                    if (i.t > 0 && i.t < distance)
                        return true;
                }
            }
        }

        // Nothing got in the way of this light
        return false;
    }
}
