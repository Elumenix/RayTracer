#pragma once
#include "Shape.h"
#include "Transformations.h"
#include "Ray.h"
#include "Intersection.h"
#include "Light.h"

class Sphere : public Shape {
public:
    Sphere() = default;
    IntersectionList custom_intersects(const Ray rayOS) const override;
    Vector custom_normal(Point pointOS) const override;
};