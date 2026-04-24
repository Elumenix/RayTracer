#include "Pattern.h"
#include "Shape.h"

Color Pattern::SampleAt(const Shape &object, Point worldPoint)
{
    Point objPoint = object.transform.inverse() * worldPoint;
    Point patternPoint = transform.inverse() * objPoint;
    return CustomSampleAt(patternPoint);
}

Color RingPattern::CustomSampleAt(Point patternPoint) const
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

Color CheckerPattern::CustomSampleAt(Point patternPoint) const
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
