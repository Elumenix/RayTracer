#pragma once
#include "IntersectionList.h"
#include "Ray.h"
#include "Matrix.h"
#include "Material.h"
#include "Tuple.h"
#include <cmath>

class Shape
{
protected:
    inline static int idIterator = 0; // Each shape gets a unique id
    Shape() : id(idIterator++) {};    // Protected constructor, class can't be made by programmer

    // Copy Constructor
    Shape(const Shape &other) : id(idIterator++), transform(other.transform), material(other.material) {}

    // Copy Assignment (This object already exists and is copying data)
    Shape &operator=(const Shape &other)
    {
        transform = other.transform;
        material = other.material;
        return *this;
    }

    // Move Constructor
    Shape(Shape &&other) : id(idIterator++), transform(std::move(other.transform)), material(std::move(other.material)) {}
    Shape &operator=(Shape &&) = default; // Move Assignment

    // Force subclass to make implementations of these
    virtual IntersectionList custom_intersects(const Ray rayOS) const = 0;
    virtual Math::Vector custom_normal(Math::Point pointOS) const = 0;

public:
    const int id;
    Math::Matrix<4, 4> transform = Math::IdentityMatrix;
    Material material;

    virtual ~Shape() = default;

    IntersectionList intersects(const Ray rayWS) const
    {
        Ray rayOS = rayWS.transform(transform.Inverse());
        return custom_intersects(rayOS);
    }

    virtual Math::Vector normal_at(Math::Point pointWS) const
    {
        // An assumption is made that the point is on the shape
        Math::Point pointOS = transform.Inverse() * pointWS;

        // Subclass needs to handle object space
        Math::Vector normalOS = custom_normal(pointOS);

        Math::Vector normalWS = transform.Inverse().Transpose() * normalOS;
        normalWS.w = 0; // Correct the w
        return normalWS.Normalized();
    }

    virtual bool operator==(const Shape &other) const { return other.material == material && other.transform == transform; };
    bool operator!=(const Shape &other) const { return !(*this == other); }
};

// ==============================================================================================================================================
// Comps code
// This would fit a bit better in Intersection or IntersectionList, but they don't have access to shape functions to prevent circular dependency

// Holds useful data about the intersection
struct Comps
{
    float t;
    bool isInside;
    const Shape *object;
    Math::Point point;
    Math::Vector eye;
    Math::Vector normal;
    Math::Vector over_point;
};

inline float EPSILON = 0.0001f;

inline Comps prepare_computation(Intersection i, Ray r)
{
    Comps comp;
    comp.t = i.t;
    comp.object = i.object;

    // Precompute the useful values
    comp.point = r.position(i.t);
    comp.eye = -r.direction;
    comp.normal = i.object->normal_at(comp.point);

    assert(!std::isnan(comp.point.z) && "point.z is NaN");
    assert(!std::isnan(comp.normal.z) && "normal.z is NaN");

    if (DotProduct(comp.normal, comp.eye) < 0)
    {
        comp.isInside = true;
        comp.normal = -comp.normal;
    }
    else
    {
        comp.isInside = false;
    }

    comp.over_point = comp.point + comp.normal * EPSILON;

    return comp;
};
