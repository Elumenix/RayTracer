#pragma once
#include "IntersectionList.h"
#include "Ray.h"
#include "Matrix.h"
#include "Material.h"
#include "Tuple.h"
#include "Light.h"

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
    virtual Vector custom_normal(Point pointOS) const = 0;

public:
    const int id;
    Matrix<4, 4> transform = IdentityMatrix;
    Material material;

    virtual ~Shape() = default;

    IntersectionList intersects(const Ray rayWS) const
    {
        Ray rayOS = rayWS.transform(transform.inverse());
        return custom_intersects(rayOS);
    }

    virtual Vector normal_at(Point pointWS) const
    {
        // An assumption is made that the point is on the shape
        Point pointOS = transform.inverse() * pointWS;

        // Subclass needs to handle object space
        Vector normalOS = custom_normal(pointOS);

        Vector normalWS = transform.inverse().transpose() * normalOS;
        normalWS.w = 0; // Correct the w
        return normalWS.Normalized();
    }

    Color lighting(const Light light, const Point position, const Vector eye, const Vector normal, const bool inShadow = false) const
    {
        Color diffuse = Color(0, 0, 0);
        Color specular = Color(0, 0, 0);

        Color effectiveColor = (material.pattern == nullptr ? material.color : material.pattern->StripeAt(position)) * light.intensity;

        Vector lightDir = (light.position - position).Normalized();
        Color ambient = effectiveColor * material.ambient;

        if (inShadow)
            return ambient;

        // Negative number here means that the light is on the other side of the surface
        float lightDotNormal = DotProduct(lightDir, normal);
        if (lightDotNormal >= 0)
        {
            diffuse = effectiveColor * material.diffuse * lightDotNormal;
            Vector reflectionVector = Reflect(-lightDir, normal);

            // Negative number means light reflects away from the eye
            float reflectDotEye = DotProduct(reflectionVector, eye);
            if (reflectDotEye > 0)
            {
                float factor = powf(reflectDotEye, material.shininess);
                specular = light.intensity * material.specular * factor;
            }
        }

        return ambient + diffuse + specular;
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
    Point point;
    Vector eye;
    Vector normal;
    Vector over_point;
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
