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
        if (abs(rayOS.direction.y) < EPSILON)
        {
            return {}; // Technically infinitly intersecting, but better to pretend it missed
        }

        float t = -rayOS.origin.y / rayOS.direction.y;
        return {Intersection(t, this)};
    }

    Vector Plane::CustomNormal(const Point &pointOS) const
    {
        // A plane is uniform, regardless of the point
        return Vector(0, 1, 0);
    }
}
