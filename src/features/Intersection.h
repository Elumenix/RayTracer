#pragma once
class Shape; // The program simply needs to know that the class exists

struct Intersection {
    float t; // should be const but that breaks the move operator
    const Shape* object;

    Intersection(float m_t, const Shape* m_object) : t(m_t), object(m_object){};

    bool operator<(const Intersection &other) const { return t <  other.t; }
    bool operator>(const Intersection &other) const { return t >  other.t; }
    bool operator<=(const Intersection &other) const { return t <=  other.t; }
    bool operator>=(const Intersection &other) const { return t >=  other.t; }
};