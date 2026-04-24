#include "Pattern.h"
#include "Shape.h"

Color Pattern::SampleAt(const Shape &object, Point worldPoint)
{
    Point objPoint = object.transform.inverse() * worldPoint;
    Point patternPoint = transform.inverse() * objPoint;
    return CustomSampleAt(patternPoint);
}

Color Ring::CustomSampleAt(Point patternPoint) const
{
    float root = std::sqrt(patternPoint.x * patternPoint.x + patternPoint.z * patternPoint.z);
    if (static_cast<int>(std::floor(root)) % 2 == 0)
    {
        return a;
    }
    else
    {
        return b;
    }
}

Color Checker::CustomSampleAt(Point patternPoint) const
{
    float t = std::floor(patternPoint.x) + std::floor(patternPoint.y) + std::floor(patternPoint.z);

    if (static_cast<int>(t) % 2 == 0)
    {
        return a;
    }
    else
    {
        return b;
    }
}

Color RadialGradient::CustomSampleAt(Point patternPoint) const
{
    float root = std::sqrt(patternPoint.x * patternPoint.x + patternPoint.z * patternPoint.z);
    float t = std::fmod(root, 2);
    if (t <= 1)
    {
        return lerp(a, b, t);
    }
    else
    {
        return lerp(b, a, t - 1);
    }
}
