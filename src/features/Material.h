#pragma once
#include "Tuple.h"
#include "Color.h"

struct Material {
    Color color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;

    Material(Color c_color = Color(1,1,1), float c_ambient = 0.1f, float c_diffuse = 0.9f, float c_specular = 0.9f, float c_shininess = 200.0f) : color(c_color), ambient(c_ambient), diffuse(c_diffuse), specular(c_specular), shininess(c_shininess){};

    bool operator==(const Material& other) const {
        return color == other.color &&
            ambient == other.ambient &&
            diffuse == other.diffuse &&
            specular == other.specular &&
            shininess == other.shininess;
    }
};