#pragma once
#include "Material.h" // Member variable prevents forward declare
#include <cmath>

namespace Rendering
{
    class IntersectionList;
    class Ray;
}

namespace Scene
{
    inline float EPSILON = 0.0001f;

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
        virtual Rendering::IntersectionList CustomIntersects(const Rendering::Ray &rayOS) const = 0;
        virtual Math::Vector CustomNormal(const Math::Point &pointOS) const = 0;

    public:
        const int id;
        Math::Matrix<4, 4> transform = Math::IdentityMatrix;
        Rendering::Material material;

        // Needs to be public to work
        virtual ~Shape() = default;

        Rendering::IntersectionList Intersects(const Rendering::Ray &rayWS) const;
        virtual Math::Vector NormalAt(const Math::Point &pointWS) const;

        virtual bool operator==(const Shape &other) const { return other.material == material && other.transform == transform; };
        bool operator!=(const Shape &other) const { return !(*this == other); }
    };
}
