#pragma once
#include "Shape.h"

namespace Rendering
{
    class Ray;
    class IntersectionList;
}

namespace Math
{
    class Point;
    class Vector;
}

namespace Scene
{
    class Sphere : public Shape
    {
    public:
        Sphere() = default;
        Rendering::IntersectionList CustomIntersects(const Rendering::Ray &rayOS) const override;
        void CustomIntersects(const Rendering::Ray &rayOS, Rendering::IntersectionList &out) const override;
        Math::Vector CustomNormal(const Math::Point &pointOS) const override;
    };

    // Helper function for a transparent sphere
    inline Sphere GlassSphere()
    {
        Sphere s;
        s.material.transparency = 1;
        s.material.refractiveIndex = 1.5;
        return s;
    }
}
