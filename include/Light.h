#pragma once

#include "Tuple.h" // Needed for class sizing
#include "Color.h" // Nedded for class sizing

namespace Scene
{
    class Shape; // Forward Declare

    class Light
    {
    public:
        Math::Point position;
        Rendering::Color intensity;

        Light() = default;
        Light(Math::Point c_position, Rendering::Color c_intensity);

        Rendering::Color Lighting(const Scene::Shape &shape, const Math::Point &pointPos, const Math::Vector &eye, const Math::Vector &normal, const bool inShadow = false) const;

        bool operator==(const Light &other) const { return other.intensity == intensity && other.position == position; };
        bool operator!=(const Light &other) const { return !(*this == other); }
    };
}
