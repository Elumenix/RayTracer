#pragma once
#include "Color.h"
#include "Tuple.h"
#include "Shape.h"
#include <cmath>

class Light
{
public:
    Math::Point position;
    Rendering::Color intensity;
    Light(Math::Point c_position, Rendering::Color c_intensity) : position(c_position), intensity(c_intensity) {};

    Rendering::Color lighting(const Scene::Shape &shape, const Math::Point pointPos, const Math::Vector eye, const Math::Vector normal, const bool inShadow = false) const
    {
        Rendering::Color diffuse = Rendering::Color(0, 0, 0);
        Rendering::Color specular = Rendering::Color(0, 0, 0);

        Rendering::Color effectiveColor = (shape.material.pattern == nullptr ? shape.material.color : shape.material.pattern->SampleAt(shape, pointPos)) * intensity;

        Math::Vector lightDir = (position - pointPos).Normalized();
        Rendering::Color ambient = effectiveColor * shape.material.ambient;

        if (inShadow)
            return ambient;

        // Negative number here means that the light is on the other side of the surface
        float lightDotNormal = DotProduct(lightDir, normal);
        if (lightDotNormal >= 0)
        {
            diffuse = effectiveColor * shape.material.diffuse * lightDotNormal;
            Math::Vector reflectionVector = Reflect(-lightDir, normal);

            // Negative number means light reflects away from the eye
            float reflectDotEye = DotProduct(reflectionVector, eye);
            if (reflectDotEye > 0)
            {
                float factor = powf(reflectDotEye, shape.material.shininess);
                specular = intensity * shape.material.specular * factor;
            }
        }

        return ambient + diffuse + specular;
    }

    bool operator==(const Light &other) const { return other.intensity == intensity && other.position == position; };
    bool operator!=(const Light &other) const { return !(*this == other); }
};