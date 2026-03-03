#pragma once
#include "Tuple.h"

class Ray {
public:
    Tuple origin;
    Tuple direction;

    Ray(Tuple p_origin, Tuple v_direction) : origin(p_origin), direction(v_direction){};
    Tuple position(float t) {return origin + t * direction; };
};