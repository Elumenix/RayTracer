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
    class Cube : public Shape
    {
    public:
        Cube() = default;
        Rendering::IntersectionList CustomIntersects(const Rendering::Ray &rayOS) const override;
        Math::Vector CustomNormal(const Math::Point &pointOS) const override;
        std::pair<float, float> CheckAxis(float rayOrigin, float rayDirection) const;
    };
}
