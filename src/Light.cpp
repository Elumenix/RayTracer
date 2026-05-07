#include "Light.h"
#include "Shape.h"

using namespace Rendering;
using namespace Math;
using namespace Scene;

namespace Scene
{
    Light::Light(Point c_position, Color c_intensity) : position(c_position), intensity(c_intensity) {};

    Color Light::Lighting(const Shape &shape, const Point &pointPos, const Vector &eye, const Vector &normal, const bool inShadow) const
    {
        Color diffuse = Color(0, 0, 0);
        Color specular = Color(0, 0, 0);

        Color effectiveColor = (shape.material.pattern == nullptr ? shape.material.color : shape.material.pattern->SampleAt(shape, pointPos)) * intensity;

        Vector lightDir = (position - pointPos).Normalized();
        Color ambient = effectiveColor * shape.material.ambient;

        if (inShadow)
            return ambient;

        // Negative number here means that the light is on the other side of the surface
        float lightDotNormal = DotProduct(lightDir, normal);
        if (lightDotNormal >= 0)
        {
            diffuse = effectiveColor * shape.material.diffuse * lightDotNormal;
            Vector reflectionVector = Reflect(-lightDir, normal);

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
}
