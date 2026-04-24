#include "Pattern.h"
#include "Shape.h"

Color Pattern::SampleAt(const Shape &object, Point worldPoint)
{
    Point objPoint = object.transform.inverse() * worldPoint;
    Point patternPoint = transform.inverse() * objPoint;
    return CustomSampleAt(patternPoint);
}

Color StripePattern::CustomSampleAt(Point patternPoint) const
{
    Pattern *slot = (static_cast<int>(floor(patternPoint.x)) % 2 == 0 ? a : b);
    return Sample(slot, patternPoint);
}

Color Gradient::CustomSampleAt(Point patternPoint) const
{
    Color colorA = Sample(a, patternPoint);
    Color colorB = Sample(b, patternPoint);
    float t = patternPoint.x - std::floor(patternPoint.x);
    return lerp(colorA, colorB, t);
}

Color Ring::CustomSampleAt(Point patternPoint) const
{
    float root = std::sqrt(patternPoint.x * patternPoint.x + patternPoint.z * patternPoint.z);
    Pattern *slot = (static_cast<int>(floor(root)) % 2 == 0) ? a : b;
    return Sample(slot, patternPoint);
}

Color Checker::CustomSampleAt(Point patternPoint) const
{
    float t = std::floor(patternPoint.x) + std::floor(patternPoint.y) + std::floor(patternPoint.z);
    Pattern *slot = (static_cast<int>(t) % 2 == 0) ? a : b;
    return Sample(slot, patternPoint);
}

Color RadialGradient::CustomSampleAt(Point patternPoint) const
{
    float root = std::sqrt(patternPoint.x * patternPoint.x + patternPoint.z * patternPoint.z);
    float t = std::fmod(root, 2);

    Color colorA = Sample(a, patternPoint);
    Color colorB = Sample(b, patternPoint);

    if (t <= 1)
    {
        return lerp(colorA, colorB, t);
    }
    else
    {
        return lerp(colorB, colorA, t - 1);
    }
}
