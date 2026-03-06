#pragma once
#include "Tuple.h"
#include "Matrix.h"

struct Ray {
public:
    Tuple origin;
    Tuple direction;

    Ray(Tuple p_origin, Tuple v_direction) : origin(p_origin), direction(v_direction){};
    Tuple position(float t) const {return origin + t * direction; };
    Ray transform(Matrix<4,4> m) { return Ray(m * origin, m * direction); }
};
