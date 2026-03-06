#pragma once
#include "Tuple.h"
#include "Matrix.h"

struct Ray {
public:
    Point origin;
    Vector direction;

    Ray(Point p_origin, Vector v_direction) : origin(p_origin), direction(v_direction){};
    Point position(float t) const {return origin + t * direction; };
    Ray transform(Matrix<4,4> m) { return Ray(m * origin, m * direction); };
};
