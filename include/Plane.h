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
    class Plane : public Shape
    {
    public:
        Plane() = default;
        Rendering::IntersectionList CustomIntersects(const Rendering::Ray &rayOS) const override;
        void CustomIntersects(const Rendering::Ray &rayOS, Rendering::IntersectionList &out) const override;
        Math::Vector CustomNormal(const Math::Point &pointOS) const override;
    };
}
