#include "Sphere.h"

using namespace Math;
using namespace Rendering;

IntersectionList Sphere::custom_intersects(const Ray &rayOS) const
{
    Tuple sphereToRay = rayOS.origin - Point(0,0,0); 
    float a = DotProduct(rayOS.direction, rayOS.direction); // squared distance
    float b = 2 * DotProduct(rayOS.direction, sphereToRay);
    float c = DotProduct(sphereToRay, sphereToRay) - 1;

    float discriminant = b * b - 4 * a * c;
    
    // Does the ray completely miss the sphere
    if (discriminant < 0) {
        return {};
    }
    
    float discSqrt = sqrtf(discriminant);

    // Get two intersection points wether they are in front of or behind the sphere
    Intersection t1 = Intersection((-b - discSqrt) / (2 * a), this);
    Intersection t2 = Intersection((-b + discSqrt) / (2 * a), this);
    
    return {t1, t2};
}

Vector Sphere::custom_normal(const Point &pointOS) const
{
    // Spheres are very simple. This is all thats needed to translate the points to the normal vector
    Vector normalOS = pointOS - Point(0,0,0);
    return normalOS;
}
