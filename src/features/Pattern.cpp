#include "Pattern.h"
#include "Shape.h"

Color Pattern::StripeAt(Point worldPoint)
{
    return (static_cast<int>(floor(worldPoint.x)) % 2 == 0 ? a : b);
}

Color Pattern::StripeAtObject(const Shape &object, Point worldPoint)
{
    Point objPoint = object.transform.inverse() * worldPoint;
    Point patternPoint = transform.inverse() * objPoint;
    return StripeAt(patternPoint);
}