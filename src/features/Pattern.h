#pragma once
#include <memory>
#include "Color.h"
#include "Matrix.h"

inline Color White = Color(1, 1, 1);
inline Color Black = Color(0, 0, 0);
class Shape; // Forward Declaration

class Pattern
{
protected:
    Pattern(Color c_A, Color c_B) : a(c_A), b(c_B) {}
    virtual Color CustomSampleAt(Point patternPoint) const = 0;

public:
    Color a;
    Color b;
    Matrix<4, 4> transform = IdentityMatrix;

    virtual std::unique_ptr<Pattern> Clone() const = 0;

    Color SampleAt(const Shape &object, Point worldPoint);
};

class TestPattern : public Pattern
{
public:
    TestPattern(Color c_A = White, Color c_B = Black) : Pattern(c_A, c_B) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<TestPattern>(*this); }

private:
    Color CustomSampleAt(Point patternPoint) const override { return Color(patternPoint.x, patternPoint.y, patternPoint.z); }
};

class StripePattern : public Pattern
{
public:
    StripePattern(Color c_A = White, Color c_B = Black) : Pattern(c_A, c_B) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<StripePattern>(*this); }

    // Test Function Only
    Color StripeAt(Point p) { return CustomSampleAt(p); }

private:
    Color CustomSampleAt(Point patternPoint) const override { return (static_cast<int>(floor(patternPoint.x)) % 2 == 0 ? a : b); }
};

inline bool operator==(const Pattern &self, const Pattern &other)
{
    return typeid(self) == typeid(other) && self.a == other.a && self.b == other.b && self.transform == other.transform;
}