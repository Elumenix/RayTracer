#pragma once
#include "Tuple.h"
#include "Matrix.h"

namespace Rendering
{
    struct Ray
    {
    public:
        Math::Point origin;
        Math::Vector direction;

        Ray(const Math::Point &p_origin, const Math::Vector &v_direction) : origin(p_origin), direction(v_direction) {};
        Math::Point Position(float t) const { return origin + t * direction; };
        Ray Transform(Math::Matrix<4, 4> m) const { return Ray(m * origin, m * direction); };
    };
}
