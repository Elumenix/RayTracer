#pragma once
#include "Shape.h"

class Plane : public Shape {
public:
    Plane() = default;
    Rendering::IntersectionList custom_intersects(const Rendering::Ray &rayOS) const override;
    Math::Vector custom_normal(const Math::Point &pointOS) const override;
};