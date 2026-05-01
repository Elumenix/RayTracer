#pragma once
#include "Shape.h"

class Plane : public Shape {
public:
    Plane() = default;
    IntersectionList custom_intersects(const Ray rayOS) const override;
    Math::Vector custom_normal(Math::Point pointOS) const override;
};