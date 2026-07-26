#include "Shape.h"
#include "Ray.h"
#include "IntersectionList.h"

using namespace Rendering;
using namespace Math;

namespace Scene
{
    IntersectionList Shape::Intersects(const Ray &rayWS) const
    {
        Ray rayOS = rayWS.Transform(*GetInverseTransform());
        return CustomIntersects(rayOS);
    }

    Vector Shape::NormalAt(const Point &pointWS) const
    {
        const Matrix<4, 4> *inv = GetInverseTransform();

        // An assumption is made that the point is on the shape
        Point pointOS = *inv * pointWS;

        // Subclass needs to handle object space
        Vector normalOS = CustomNormal(pointOS);

        Vector normalWS = (*inv).Transpose() * normalOS;
        normalWS.w = 0; // Correct the w
        return normalWS.Normalized();
    }

    Math::Matrix<4, 4> const *Shape::GetInverseTransform() const
    {
        if (!hasInvTransform)
        {
            invTransform = transform.Inverse();
            hasInvTransform = true;
        }

        return &invTransform;
    }
}
