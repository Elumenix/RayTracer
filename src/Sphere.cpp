#include "Sphere.h"
#include "Ray.h"
#include "IntersectionList.h"
#include "Intersection.h"
#include "Tuple.h"

using namespace Math;
using namespace Rendering;

namespace Scene
{
    IntersectionList Sphere::CustomIntersects(const Ray &rayOS) const
    {
        Tuple sphereToRay = rayOS.origin - Point(0, 0, 0);
        float a = DotProduct(rayOS.direction, rayOS.direction); // squared distance
        float b = 2 * DotProduct(rayOS.direction, sphereToRay);
        float c = DotProduct(sphereToRay, sphereToRay) - 1;

        float discriminant = b * b - 4 * a * c;

        // Does the ray completely miss the sphere
        if (discriminant < 0)
        {
            return {};
        }

        float discSqrt = std::sqrt(discriminant);

        // Get two intersection points wether they are in front of or behind the sphere
        Intersection t1 = Intersection((-b - discSqrt) / (2 * a), this);
        Intersection t2 = Intersection((-b + discSqrt) / (2 * a), this);

        return {t1, t2};
    }

    void Sphere::CustomIntersects(const Rendering::Ray &rayOS, Rendering::IntersectionList &out) const
    {
        Tuple sphereToRay = rayOS.origin - Point(0, 0, 0);
        float a = DotProduct(rayOS.direction, rayOS.direction); // squared distance
        float b = 2 * DotProduct(rayOS.direction, sphereToRay);
        float c = DotProduct(sphereToRay, sphereToRay) - 1;

        float discriminant = b * b - 4 * a * c;

        // Does the ray completely miss the sphere
        if (discriminant < 0)
        {
            return;
        }

        float discSqrt = std::sqrt(discriminant);

        // Get two intersection points wether they are in front of or behind the sphere
        Intersection t1 = Intersection((-b - discSqrt) / (2 * a), this);
        Intersection t2 = Intersection((-b + discSqrt) / (2 * a), this);

        out.Append({t1, t2});
    }

    Vector Sphere::CustomNormal(const Point &pointOS) const
    {
        // Spheres are very simple. This is all thats needed to translate the points to the normal vector
        Vector normalOS = pointOS - Point(0, 0, 0);
        return normalOS;
    }
}
