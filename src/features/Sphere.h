#pragma once
#include "Shape.h"
#include "Transformations.h"
#include "Ray.h"
#include "Intersection.h"
#include "Light.h"

class Sphere : public Shape {
public:
    Sphere() = default;
    Rendering::IntersectionList custom_intersects(const Rendering::Ray &rayOS) const override;
    Math::Vector custom_normal(const Math::Point &pointOS) const override;
};