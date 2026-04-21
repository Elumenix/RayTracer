#include "Pattern.h"
#include "Shape.h"

Color Pattern::SampleAt(const Shape &object, Point worldPoint)
{
    Point objPoint = object.transform.inverse() * worldPoint;
    Point patternPoint = transform.inverse() * objPoint;
    return CustomSampleAt(patternPoint);
}
