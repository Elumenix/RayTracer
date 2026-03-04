#pragma once
#include "Tuple.h"

struct Ray {
public:
    Tuple origin;
    Tuple direction;

    Ray(Tuple p_origin, Tuple v_direction) : origin(p_origin), direction(v_direction){};
    Tuple position(float t) const {return origin + t * direction; };
};
