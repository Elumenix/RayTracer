#pragma once
class Shape; // The program simply needs to know that the class exists

struct Intersection {
    const float t;
    const Shape* object;
    Intersection(float m_t, const Shape* m_object) : t(m_t), object(m_object){};
};