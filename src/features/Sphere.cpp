#include "Sphere.h"

IntersectionList Sphere::intersects(const Ray r) const
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

Vector Sphere::normal_at(Point worldPoint) const
{
    // An assumption is made that the point is on the sphere
    Point objectPoint = transform.inverse() * worldPoint;
    Vector objectNormal = objectPoint - Point(0,0,0); 


    Vector worldNormal = transform.inverse().transpose() * objectNormal;
    worldNormal.w = 0; // Correct the w 
    return worldNormal.Normalized();
}

Color Sphere::lighting(const Light light, const Point position, const Vector eye, const Vector normal, const bool inShadow) const
{
    Color diffuse = Color(0,0,0);
    Color specular = Color(0,0,0);

    Color effectiveColor = material.color * light.intensity;
    Vector lightDir = (light.position - position).Normalized();
    Color ambient = effectiveColor * material.ambient;

    if (inShadow) return ambient;

    // Negative number here means that the light is on the other side of the surface
    float lightDotNormal = DotProduct(lightDir, normal);
    if (lightDotNormal >= 0) {
        diffuse = effectiveColor * material.diffuse * lightDotNormal;
        Vector reflectionVector = Reflect(-lightDir, normal);

        // Negative number means light reflects away from the eye
        float reflectDotEye = DotProduct(reflectionVector, eye);
        if (reflectDotEye > 0) {
            float factor = powf(reflectDotEye, material.shininess);
            specular = light.intensity * material.specular * factor;
        }
    }

    return ambient + diffuse + specular;
}
