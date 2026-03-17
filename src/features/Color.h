#pragma once
#include "Tuple.h"

class Color : public Tuple{
public:
    Color() : Tuple(1,1,1,0) {};
    Color(float r, float g, float b) : Tuple(r, g, b, 0) {};
    Color(const Tuple& t) : Tuple(t.x, t.y, t.z, t.w) {}
    ~Color() = default;
};

inline Color operator*(const Color c1, const Color c2) { return Color(c1.x*c2.x, c1.y*c2.y, c1.z*c2.z); }
inline Color operator*(const Color c, const float scalar) { return Color(c.x * scalar, c.y * scalar, c.z * scalar); }
inline Color operator+(const Color c1, const Color c2) { return Color(c1.x+c2.x, c1.y+c2.y, c1.z+c2.z); }
inline void operator+=(Color &c1, const Color c2) { c1 = c1 + c2; }
