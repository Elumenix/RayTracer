#include "Plane.h"
#include "Ray.h"
#include "IntersectionList.h"
#include "Intersection.h"

using namespace Math;
using namespace Rendering;

namespace Scene
{
    IntersectionList Plane::CustomIntersects(const Ray &rayOS) const
    {
        if (std::abs(rayOS.direction.y) < EPSILON)
        {
            return {}; // Technically infinitly intersecting, but better to pretend it missed
        }

        float t = -rayOS.origin.y / rayOS.direction.y;
        return {Intersection(t, this)};
    }

    void Plane::CustomIntersects(const Rendering::Ray &rayOS, Rendering::IntersectionList &out) const
    {
        if (std::abs(rayOS.direction.y) < EPSILON)
        {
            return; // Technically infinitly intersecting, but better to pretend it missed
        }

        float t = -rayOS.origin.y / rayOS.direction.y;
        out.Append({Intersection(t, this)});
    }

    Vector Plane::CustomNormal(const Point &pointOS) const
    {
        // A plane is uniform, regardless of the point
        return Vector(0, 1, 0);
    }
}
