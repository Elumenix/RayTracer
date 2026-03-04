#pragma once
#include "Tuple.h"
#include "Sphere.h"

struct IntersectionResult {
    float t1;
    float t2;
    bool hit;
};

class Ray {
public:
    Tuple origin;
    Tuple direction;

    Ray(Tuple p_origin, Tuple v_direction) : origin(p_origin), direction(v_direction){};
    Tuple position(float t) const {return origin + t * direction; };
    IntersectionResult intersects(Sphere s) const {
        IntersectionResult res;

        // Sphere is currently hard-coded to be at the world origin
        Tuple sphereToRay = origin - Point(0,0,0);
        float a = DotProduct(direction, direction); // squared distance
        float b = 2 * DotProduct(direction, sphereToRay);
        float c = DotProduct(sphereToRay, sphereToRay) - 1;

        float discriminant = b * b - 4 * a * c;
        
        // Does the ray completely miss the sphere
        if (discriminant < 0) {
            res.hit = false;
            return res;
        }
        
        float discSqrt = sqrtf(discriminant);

        // Get two intersection points wether they are in front of or behind the sphere
        res.t1 = (-b - discSqrt) / (2 * a);
        res.t2 = (-b + discSqrt) / (2 * a);
        res.hit = true;
        
        return res;
    }
};
