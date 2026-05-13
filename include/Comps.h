#pragma once
#include "Tuple.h"

// Forward Declare
namespace Scene
{
    class Shape;
}

namespace Rendering
{
    class IntersectionList;
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
        Math::Vector overPoint;
        Math::Vector underPoint;
        Math::Vector reflect;
        float n1;
        float n2;
    };

    Comps PrepareComputation(const Intersection &i, const Ray &r);
    Comps PrepareComputation(const Intersection &i, const Ray &r, const IntersectionList &xs);
}
