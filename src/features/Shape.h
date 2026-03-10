#pragma once
#include "IntersectionList.h"
#include "Ray.h"
#include "Matrix.h"
#include "Material.h"
#include "Tuple.h"
#include "Light.h"

class Shape {
protected:
    inline static int idIterator = 0; // Each shape gets a unique id
    Shape() : id(idIterator++) {}; // Protected constructor, class can't be made by programmer

public:
    const int id;
    Matrix<4,4> transform = IdentityMatrix;
    Material material;
    virtual IntersectionList intersects(Ray r) const = 0;
    virtual Vector normal_at(Point worldPoint) const = 0;
    virtual Color lighting(const Light light, const Point position, const Vector eye, const Vector normal) const = 0;
};