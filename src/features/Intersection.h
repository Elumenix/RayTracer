#pragma once
#include "Shape.h"

struct Intersection {
    float t;
    Shape& object;
    Intersection(float m_t, Shape& m_object) : t(m_t), object(m_object){};
};