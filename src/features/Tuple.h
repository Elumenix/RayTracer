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

Tuple Point(float x, float y, float z);
Tuple Vector(float x, float y, float z);
float DotProduct(Tuple a, Tuple b);
Tuple CrossProduct(Tuple a, Tuple b);

bool operator==(const Tuple lhs, const Tuple rhs);
Tuple operator+(const Tuple lhs, const Tuple rhs);
Tuple operator-(const Tuple lhs, const Tuple rhs);
Tuple operator-(const Tuple neg);
Tuple operator*(const Tuple t, float scalar);
Tuple operator/(const Tuple t, float div);
std::ostream& operator<<(std::ostream& os, const Tuple& t); 
