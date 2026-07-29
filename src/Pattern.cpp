#include "Pattern.h"
#include "Shape.h"
#include <cmath>

using namespace Math;
using namespace Scene;

namespace Rendering
{
    Color Pattern::SampleAt(const Shape &object, const Point &worldPoint)
    {
        Point objPoint = *object.GetInverseTransform() * worldPoint;
        Point patternPoint = *GetInverseTransform() * objPoint;
        return CustomSampleAt(patternPoint);
    }

    Math::Matrix<4, 4> const *Pattern::GetInverseTransform() const
    {
        std::call_once(invTransformFlag, [this]()
                       { m_invTransform = transform.Inverse(); });

        return &m_invTransform;
    }

    Color StripePattern::CustomSampleAt(const Point &patternPoint) const
    {
        Pattern *slot = (static_cast<int>(std::floor(patternPoint.x)) % 2 == 0 ? a : b);
        return Sample(slot, patternPoint);
    }

    Color Gradient::CustomSampleAt(const Point &patternPoint) const
    {
        Color colorA = Sample(a, patternPoint);
        Color colorB = Sample(b, patternPoint);
        float t = patternPoint.x - std::floor(patternPoint.x);
        return Lerp(colorA, colorB, t);
    }

    Color Ring::CustomSampleAt(const Point &patternPoint) const
    {
        float root = std::sqrt(patternPoint.x * patternPoint.x + patternPoint.z * patternPoint.z);
        Pattern *slot = (static_cast<int>(std::floor(root)) % 2 == 0) ? a : b;
        return Sample(slot, patternPoint);
    }

    Color Checker::CustomSampleAt(const Point &patternPoint) const
    {
        float t = std::floor(patternPoint.x) + std::floor(patternPoint.y) + std::floor(patternPoint.z);
        Pattern *slot = (static_cast<int>(t) % 2 == 0) ? a : b;
        return Sample(slot, patternPoint);
    }

    Color RadialGradient::CustomSampleAt(const Point &patternPoint) const
    {
        float root = std::sqrt(patternPoint.x * patternPoint.x + patternPoint.z * patternPoint.z);
        float t = std::fmod(root, 2);

        Color colorA = Sample(a, patternPoint);
        Color colorB = Sample(b, patternPoint);

        if (t <= 1)
        {
            return Lerp(colorA, colorB, t);
        }
        else
        {
            return Lerp(colorB, colorA, t - 1);
        }
    }

    Color Blend::CustomSampleAt(const Point &patternPoint) const
    {
        Color colorA = Sample(a, patternPoint);
        Color colorB = Sample(b, patternPoint);

        return (colorA + colorB) * .5f;
    }

    Color Perturb::CustomSampleAt(const Point &patternPoint) const
    {
        float jitterX = p.Sample(0, patternPoint.x, patternPoint.y, patternPoint.z);
        float jitterY = p.Sample(32, patternPoint.x, patternPoint.y, patternPoint.z);
        float jitterZ = p.Sample(64, patternPoint.x, patternPoint.y, patternPoint.z);

        float scale = strength;

        Point jitteredPoint = Point(
            patternPoint.x + jitterX * scale,
            patternPoint.y + jitterY * scale,
            patternPoint.z + jitterZ * scale);

        // The pattern this is perturbing will sample this
        return Sample(a, jitteredPoint);
    }

    Color TestPattern::CustomSampleAt(const Math::Point &patternPoint) const
    {
        return Color(patternPoint.x, patternPoint.y, patternPoint.z);
    }
}