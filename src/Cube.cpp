#include "Cube.h"
#include "IntersectionList.h"
#include "Intersection.h"
#include "Ray.h"
#include "Constants.h"
#include <limits.h>
#include <algorithm>

using namespace Math;
using namespace Rendering;

namespace Scene
{
    IntersectionList Cube::CustomIntersects(const Ray &rayOS) const
    {
        auto [xMin, xMax] = CheckAxis(rayOS.origin.x, rayOS.direction.x);
        auto [yMin, yMax] = CheckAxis(rayOS.origin.y, rayOS.direction.y);
        auto [zMin, zMax] = CheckAxis(rayOS.origin.z, rayOS.direction.z);

        float tMin = std::max({xMin, yMin, zMin});
        float tMax = std::min({xMax, yMax, zMax});

        if (tMin > tMax)
            return {};
        return {Intersection(tMin, this), Intersection(tMax, this)};
    }

    void Cube::CustomIntersects(const Rendering::Ray &rayOS, Rendering::IntersectionList &out) const
    {
        auto [xMin, xMax] = CheckAxis(rayOS.origin.x, rayOS.direction.x);
        auto [yMin, yMax] = CheckAxis(rayOS.origin.y, rayOS.direction.y);
        auto [zMin, zMax] = CheckAxis(rayOS.origin.z, rayOS.direction.z);

        float tMin = std::max({xMin, yMin, zMin});
        float tMax = std::min({xMax, yMax, zMax});

        if (tMin > tMax)
            return;
        out.Append({Intersection(tMin, this), Intersection(tMax, this)});
    }

    Vector Cube::CustomNormal(const Point &pointOS) const
    {
        float maxSide = std::max({std::abs(pointOS.x), std::abs(pointOS.y), std::abs(pointOS.z)});

        if (maxSide == std::abs(pointOS.x))
            return Vector(pointOS.x, 0, 0);
        if (maxSide == std::abs(pointOS.y))
            return Vector(0, pointOS.y, 0);
        return Vector(0, 0, pointOS.z);
    }

    std::pair<float, float> Cube::CheckAxis(float rayOrigin, float rayDirection) const
    {
        // Ray plane intersection from the origin (which the cube sits at in object space)
        float tMinNumerator = -1 - rayOrigin;
        float tMaxNumerator = 1 - rayOrigin;

        constexpr float inf = std::numeric_limits<float>::infinity();
        float tMin = (tMinNumerator == 0 && rayDirection == 0) ? inf : tMinNumerator / rayDirection;
        float tMax = (tMaxNumerator == 0 && rayDirection == 0) ? -inf : tMaxNumerator / rayDirection;

        if (tMin > tMax)
            std::swap(tMin, tMax);

        return {tMin, tMax};
    }
}
