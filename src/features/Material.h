#pragma once
#include "Tuple.h"
#include "Color.h"
#include "Pattern.h"
#include <memory>


struct Material {
    Color color;
    std::unique_ptr<Pattern> pattern;
    float ambient;
    float diffuse;
    float specular;
    float shininess;

    Material(Color c_color = Color(1,1,1), float c_ambient = 0.1f, float c_diffuse = 0.9f, float c_specular = 0.9f, float c_shininess = 200.0f) : color(c_color), ambient(c_ambient), diffuse(c_diffuse), specular(c_specular), shininess(c_shininess){};
    Material(Material&&) = default;
    Material& operator=(Material&&) = default;
    
    Material(const Material& other) 
        : color(other.color), ambient(other.ambient), diffuse(other.diffuse), 
          specular(other.specular), shininess(other.shininess),
          pattern(other.pattern ? other.pattern->clone() : nullptr) {}
    
    Material& operator=(const Material& other) {
        color = other.color;
        ambient = other.ambient;
        diffuse = other.diffuse;
        specular = other.specular;
        shininess = other.shininess;
        pattern = other.pattern ? other.pattern->clone() : nullptr;
        return *this;
    }

    bool operator==(const Material& other) const {
        return color == other.color &&
            ambient == other.ambient &&
            diffuse == other.diffuse &&
            specular == other.specular &&
            shininess == other.shininess && 
            pattern == other.pattern;
    }
};
