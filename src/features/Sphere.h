#pragma once
#include "Shape.h"
#include "Transformations.h"
#include "Ray.h"
#include "Intersection.h"
#include "Light.h"

class Sphere : public Shape {
public:
    Sphere() = default;
    IntersectionList intersects(const Ray r) const override;
    Vector normal_at(Point worldPoint) const override;
    Color lighting(const Light light, const Point position, const Vector eye, const Vector normal) const override;
};