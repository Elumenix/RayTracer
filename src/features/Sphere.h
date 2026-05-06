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
        Math::Vector CustomNormal(const Math::Point &pointOS) const override;
    };
}
