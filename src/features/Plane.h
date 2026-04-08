#pragma once
#include "Shape.h"

class Plane : public Shape {
public:
    Plane() = default;
    IntersectionList custom_intersects(const Ray rayOS) const override;
    Vector custom_normal(Point pointOS) const override;
};