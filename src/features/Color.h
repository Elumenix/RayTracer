#pragma once
#include "Tuple.h"

class Color : public Tuple{
public:
    Color(float r, float g, float b);
    ~Color() = default;
};

Color operator*(const Color c1, const Color c2);
