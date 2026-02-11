#include "Tuple.h"
#include <cmath>;

Tuple::Tuple(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{
    
}

Tuple Point(float x, float y, float z)
{
    return Tuple(x, y, z, 1.0);
}

Tuple Vector(float x, float y, float z)
{
    return Tuple(x, y, z, 0.0);
}

bool operator==(const Tuple lhs, const Tuple rhs)
{
    if (abs(lhs.x - rhs.x) >= .0001) return false;
    if (abs(lhs.y - rhs.y) >= .0001) return false;
    if (abs(lhs.z - rhs.z) >= .0001) return false;
    if (abs(lhs.w - rhs.w) >= .0001) return false;
    return true;
}
