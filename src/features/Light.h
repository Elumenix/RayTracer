#pragma once
#include "Color.h"
#include "Tuple.h"

class Light {
public:
    Point position;
    Color intensity;
    Light(Point c_position, Color c_intensity) : position(c_position), intensity(c_intensity) {}; 

    bool operator==(const Light& other) const { return other.intensity == intensity && other.position == position; };
    bool operator!=(const Light& other) const { return !(*this == other); }
};