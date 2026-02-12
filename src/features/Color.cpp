#include "Color.h"

Color::Color(float r, float g, float b) : Tuple(r, g, b, 0) {};

Color operator*(const Color c1, const Color c2)
{
    return Color(c1.x*c2.x, c1.y*c2.y, c1.z*c2.z);
}
