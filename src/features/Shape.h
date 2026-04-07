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

    virtual ~Shape() = default;
    virtual IntersectionList intersects(const Ray r) const = 0;
    virtual Vector normal_at(Point worldPoint) const = 0;
    virtual Color lighting(const Light light, const Point position, const Vector eye, const Vector normal, const bool inShadow = false) const = 0;
    virtual bool operator==(const Shape& other) const { return other.material == material && other.transform == transform; };
    bool operator!=(const Shape& other) const { return !(*this == other); }
};

// ==============================================================================================================================================
// Comps code
// This would fit a bit better in Intersection or IntersectionList, but they don't have access to shape functions to prevent circular dependency

// Holds useful data about the intersection
struct Comps {
    float t;
    bool isInside;
    const Shape* object;
    Point point;
    Vector eye;
    Vector normal;
    Vector over_point;
};

inline Comps prepare_computation(Intersection i, Ray r) {
    Comps comp;
    comp.t = i.t;
    comp.object = i.object;

    // Precompute the useful values
    comp.point = r.position(i.t);
    comp.eye = -r.direction;
    comp.normal = i.object->normal_at(comp.point);

    if (DotProduct(comp.normal, comp.eye) < 0) {
        comp.isInside = true;
        comp.normal = -comp.normal;
    }
    else {
        comp.isInside = false;
    }

    comp.over_point = comp.point + comp.normal * 0.0001;

    return comp;
};
