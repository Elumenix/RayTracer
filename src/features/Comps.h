#pragma once
#include "Tuple.h"

// Forward Declare
namespace Scene
{
    class Shape;
}

namespace Rendering
{
    class Intersection;
    class Ray;

    // Holds useful data about the intersection
    struct Comps
    {
        float t;
        bool isInside;
        const Scene::Shape *object;
        Math::Point point;
        Math::Vector eye;
        Math::Vector normal;
        Math::Vector over_point;
    };

    Comps PrepareComputation(const Intersection &i, const Ray &r);
}
