#pragma once
#include "Shape.h"
#include "Transformations.h"
#include "Ray.h"
#include "Intersection.h"

class Sphere : public Shape {
public:
    Sphere() = default;
    IntersectionList intersects(Ray r) const override;
};