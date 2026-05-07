#include "Shape.h"
#include "Ray.h"
#include "IntersectionList.h"

using namespace Rendering;
using namespace Math;

namespace Scene
{
    IntersectionList Shape::Intersects(const Ray &rayWS) const
    {
        Ray rayOS = rayWS.Transform(transform.Inverse());
        return CustomIntersects(rayOS);
    }

    Vector Shape::NormalAt(const Point &pointWS) const
    {
        // An assumption is made that the point is on the shape
        Point pointOS = transform.Inverse() * pointWS;

        // Subclass needs to handle object space
        Vector normalOS = CustomNormal(pointOS);

        Vector normalWS = transform.Inverse().Transpose() * normalOS;
        normalWS.w = 0; // Correct the w
        return normalWS.Normalized();
    }
}
