#pragma once
#include <memory>
#include "Color.h"
#include "Pattern.h"

namespace Rendering
{
    struct Material
    {
        Color color;
        std::unique_ptr<Pattern> pattern;
        float ambient;
        float diffuse;
        float specular;
        float shininess;

        // Regular Constructor
        Material(Color c_color = Color(1, 1, 1), float c_ambient = 0.1f, float c_diffuse = 0.9f, float c_specular = 0.9f, float c_shininess = 200.0f);
        Material(const Material &other);            // Copy Constructor
        Material &operator=(const Material &other); // Copy assignment
        Material(Material &&) = default;            // Move constructor
        Material &operator=(Material &&) = default; // Move Assignment

        bool operator==(const Material &other) const;
    };
}
