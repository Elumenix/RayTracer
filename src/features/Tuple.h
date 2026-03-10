#pragma once
#include <ostream>
#include <cmath>


class Tuple {
public:
    Tuple(float x, float y, float z, float w);
    ~Tuple() = default;
    float x, y, z, w;

    float Magnitude();
    Tuple Normalized();
};

// Point and Vector are essentially just semantics of Tuple with a set w value if the constructor is used
class Point : public Tuple {
public:
    Point(float x, float y, float z) : Tuple(x, y, z, 1.0f) {}
    Point(const Tuple& t) : Tuple(t) {} // implicit conversion so math works
};

class Vector : public Tuple {
public:
    Vector(float x, float y, float z) : Tuple(x, y, z, 0.0f) {}
    Vector(const Tuple& t) : Tuple(t) {} // implicit conversion so math works
};

float DotProduct(Tuple a, Tuple b);
Tuple CrossProduct(Tuple a, Tuple b);
Vector Reflect(Vector in, Vector normal);

bool operator==(const Tuple lhs, const Tuple rhs);
Tuple operator+(const Tuple lhs, const Tuple rhs);
Tuple operator-(const Tuple lhs, const Tuple rhs);
Tuple operator-(const Tuple neg);
Tuple operator*(const Tuple t, float scalar);
Tuple operator*(float scalar, const Tuple t);
Tuple operator/(const Tuple t, float div);
std::ostream& operator<<(std::ostream& os, const Tuple& t); 
