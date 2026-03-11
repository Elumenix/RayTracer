#pragma once
#include "Tuple.h"

class Color : public Tuple{
public:
    Color() : Tuple(1,1,1,0) {};
    Color(float r, float g, float b) : Tuple(r, g, b, 0) {};
    Color(const Tuple& t) : Tuple(t.x, t.y, t.z, t.w) {}
    ~Color() = default;
};

Color operator*(const Color c1, const Color c2);
Color operator*(const Color c1, const float scalar);
