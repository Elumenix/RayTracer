#include "Material.h"

namespace Rendering
{
    Material::Material(Color c_color, float c_ambient, float c_diffuse, float c_specular, float c_shininess, float c_reflectiveness)
        : color(c_color), ambient(c_ambient), diffuse(c_diffuse), specular(c_specular), shininess(c_shininess), reflective(c_reflectiveness) {}
    Material::Material(const Material &other) : color(other.color), ambient(other.ambient), diffuse(other.diffuse),
                                                specular(other.specular), shininess(other.shininess), reflective(other.reflective),
                                                pattern(other.pattern ? other.pattern->Clone() : nullptr) {}

    Material &Material::operator=(const Material &other)
    {
        color = other.color;
        ambient = other.ambient;
        diffuse = other.diffuse;
        specular = other.specular;
        shininess = other.shininess;
        reflective = other.reflective;
        pattern = other.pattern ? other.pattern->Clone() : nullptr;
        return *this;
    }

    bool Material::operator==(const Material &other) const
    {
        return color == other.color &&
                   ambient == other.ambient &&
                   diffuse == other.diffuse &&
                   specular == other.specular &&
                   shininess == other.shininess &&
                   reflective == other.reflective &&
                   (!pattern && !other.pattern) ||                       // Either niether has pattern
               (pattern && other.pattern && *pattern == *other.pattern); // Or both have equal patterns
    }
}
