#pragma once
#include "Tuple.h"
#include "Color.h"
#include "Pattern.h"
#include <memory>

struct Material
{
    Color color;
    std::unique_ptr<Pattern> pattern;
    float ambient;
    float diffuse;
    float specular;
    float shininess;

    // Regular Constructor
    Material(Color c_color = Color(1, 1, 1), float c_ambient = 0.1f, float c_diffuse = 0.9f, float c_specular = 0.9f, float c_shininess = 200.0f)
        : color(c_color), ambient(c_ambient), diffuse(c_diffuse), specular(c_specular), shininess(c_shininess) {};

    // Copy Constructor
    Material(const Material &other)
        : color(other.color), ambient(other.ambient), diffuse(other.diffuse),
          specular(other.specular), shininess(other.shininess),
          pattern(other.pattern ? other.pattern->Clone() : nullptr) {}

    // Copy assignment
    Material &operator=(const Material &other)
    {
        color = other.color;
        ambient = other.ambient;
        diffuse = other.diffuse;
        specular = other.specular;
        shininess = other.shininess;
        pattern = other.pattern ? other.pattern->Clone() : nullptr;
        return *this;
    }

    Material(Material &&) = default;            // Move constructor
    Material &operator=(Material &&) = default; // Move Assignment

    bool operator==(const Material &other) const
    {
        return color == other.color &&
                   ambient == other.ambient &&
                   diffuse == other.diffuse &&
                   specular == other.specular &&
                   shininess == other.shininess &&
                   (!pattern && !other.pattern) || // Either niether has pattern
               (pattern && other.pattern && *pattern == *other.pattern); // Or both have equal patterns
    }
};
