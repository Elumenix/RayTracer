#include "Tuple.h"


Tuple::Tuple(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{
    
}

float Tuple::Magnitude()
{
    return sqrt(x*x + y*y + z*z + w*w);
}

Tuple Tuple::Normalized()
{
    float magnitude = Magnitude();

    return Tuple(x/magnitude, y/magnitude, z/magnitude, w/magnitude);
}

Tuple Point(float x, float y, float z)
{
    return Tuple(x, y, z, 1.0);
}

Tuple Vector(float x, float y, float z)
{
    return Tuple(x, y, z, 0.0);
}

float DotProduct(Tuple a, Tuple b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Tuple CrossProduct(Tuple a, Tuple b)
{
    return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

bool operator==(const Tuple lhs, const Tuple rhs)
{
    if (abs(lhs.x - rhs.x) >= .0001) return false;
    if (abs(lhs.y - rhs.y) >= .0001) return false;
    if (abs(lhs.z - rhs.z) >= .0001) return false;
    if (abs(lhs.w - rhs.w) >= .0001) return false;
    return true;
}

Tuple operator+(const Tuple lhs, const Tuple rhs) {
  return Tuple(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

Tuple operator-(const Tuple lhs, const Tuple rhs) {
  return Tuple(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

Tuple operator-(const Tuple neg) {
  return Tuple(-neg.x, -neg.y, -neg.z, -neg.w);
}

Tuple operator*(const Tuple t, float scalar) {
  return Tuple(t.x * scalar, t.y * scalar, t.z * scalar, t.w * scalar);
}

Tuple operator/(const Tuple t, float div) {
  return Tuple(t.x / div, t.y / div, t.z / div, t.w / div);
}

std::ostream &operator<<(std::ostream &os, const Tuple &t) {
  os << "\n" << t.x << " " << t.y << " " << t.z << " " << t.w << std::endl;
  return os;
}
