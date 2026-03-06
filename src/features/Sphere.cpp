#include "Sphere.h"

IntersectionList Sphere::intersects(Ray r) const
{
    // We are transforming the ray instead of the sphere to determine the intersection
    Ray r2 = r.transform(transform.inverse());

    // Sphere is currently hard-coded to be at the world origin
    Tuple sphereToRay = r2.origin - Point(0,0,0); // Creates a vector, which is why r2.origin isn't used
    float a = DotProduct(r2.direction, r2.direction); // squared distance
    float b = 2 * DotProduct(r2.direction, sphereToRay);
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
