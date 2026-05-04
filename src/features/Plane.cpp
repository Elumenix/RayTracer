#include "Plane.h"

using namespace Math;
using namespace Rendering;

IntersectionList Plane::custom_intersects(const Ray &rayOS) const
{
    if (abs(rayOS.direction.y) < EPSILON) {
        return {}; // Technically infinitly intersecting, but better to pretend it missed
    }

    float t = -rayOS.origin.y / rayOS.direction.y;
    return {Intersection(t, this)};
}

Vector Plane::custom_normal(const Point &pointOS) const
{
    // A plane is uniform, regardless of the point
    return Vector(0,1,0);
}
