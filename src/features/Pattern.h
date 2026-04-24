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
    Pattern(Pattern *c_A, Pattern *c_B) : a(c_A), b(c_B) {}
    virtual Color CustomSampleAt(Point patternPoint) const = 0;
    static Color Sample(const Pattern *pattern, Point p) { return pattern->CustomSampleAt(p); }

public:
    Pattern *a;
    Pattern *b;
    Matrix<4, 4> transform = IdentityMatrix;

    virtual std::unique_ptr<Pattern> Clone() const = 0;

    Color SampleAt(const Shape &object, Point worldPoint);
};

class SolidColor : public Pattern
{
public:
    SolidColor(Color c) : Pattern(nullptr, nullptr), color(c) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<SolidColor>(*this); }

    Color color;

private:
    Color CustomSampleAt(Point patternPoint) const override { return color; }
};

inline SolidColor SolidWhite = SolidColor(White);
inline SolidColor SolidBlack = SolidColor(Black);

class TestPattern : public Pattern
{
public:
    TestPattern(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<TestPattern>(*this); }

private:
    Color CustomSampleAt(Point patternPoint) const override { return Color(patternPoint.x, patternPoint.y, patternPoint.z); }
};

class StripePattern : public Pattern
{
public:
    StripePattern(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<StripePattern>(*this); }

    // Test Function Only
    Color StripeAt(Point p) { return CustomSampleAt(p); }

private:
    Color CustomSampleAt(Point patternPoint) const override;
};

class Gradient : public Pattern
{
public:
    Gradient(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Gradient>(*this); }

private:
    Color CustomSampleAt(Point patternPoint) const override;
};

class Ring : public Pattern
{
public:
    Ring(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Ring>(*this); }

private:
    Color CustomSampleAt(Point patternPoint) const override;
};

class Checker : public Pattern
{
public:
    Checker(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Checker>(*this); }

private:
    Color CustomSampleAt(Point patternPoint) const override;
};

class RadialGradient : public Pattern
{
public:
    RadialGradient(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
    std::unique_ptr<Pattern> Clone() const override { return std::make_unique<RadialGradient>(*this); }

private:
    Color CustomSampleAt(Point patternPoint) const override;
};

// Factory Method for creating patterns that will automatically be in a smart pointer
template <typename T>
std::unique_ptr<Pattern> MakePattern(Pattern *a = &SolidWhite, Pattern *b = &SolidBlack)
{
    if constexpr (std::is_same_v<T, SolidColor>)
    {
        static_assert(!std::is_same_v<T, SolidColor>, "Tried to make SolidColor as a smart pointer. Should just use material color instead.");
        return nullptr;
    }
    else
    {
        return std::make_unique<T>(a, b);
    }
}

inline bool operator==(const Pattern &self, const Pattern &other)
{
    return typeid(self) == typeid(other) && self.a == other.a && self.b == other.b && self.transform == other.transform;
}