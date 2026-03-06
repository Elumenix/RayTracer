#include "Sphere.h"

IntersectionList Sphere::intersects(Ray r) const
{
    // Sphere is currently hard-coded to be at the world origin
    Tuple sphereToRay = r.origin - Point(0,0,0);
    float a = DotProduct(r.direction, r.direction); // squared distance
    float b = 2 * DotProduct(r.direction, sphereToRay);
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
