#pragma once
#include "Tuple.h"
#include "Ray.h"
class Shape; // We don't need to operate on shape parameters, this breaks circular dependency 

struct Intersection {
    float t; // should be const but that breaks the move operator
    const Shape* object;

    Intersection(float m_t, const Shape* m_object) : t(m_t), object(m_object){};

    bool operator<(const Intersection &other) const { return t <  other.t; }
    bool operator>(const Intersection &other) const { return t >  other.t; }
    bool operator<=(const Intersection &other) const { return t <=  other.t; }
    bool operator>=(const Intersection &other) const { return t >=  other.t; }
};
