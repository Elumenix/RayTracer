#pragma once
#include <memory>
#include "Color.h"
#include "Noise.h"
#include "Matrix.h"
#include <assert.h>

inline Rendering::Color White = Rendering::Color(1, 1, 1);
inline Rendering::Color Black = Rendering::Color(0, 0, 0);
class Shape; // Forward Declaration

namespace Rendering
{
    class Pattern
    {
    protected:
        Pattern(Pattern *c_A, Pattern *c_B) : a(c_A), b(c_B) {}
        virtual Color CustomSampleAt(const Math::Point &patternPoint) const = 0;
        static Color Sample(const Pattern *pattern, const Math::Point &p) { return pattern->CustomSampleAt(p); }

    public:
        Pattern *a;
        Pattern *b;
        Math::Matrix<4, 4> transform = Math::IdentityMatrix;

        virtual std::unique_ptr<Pattern> Clone() const = 0;

        Color SampleAt(const Shape &object, const Math::Point &worldPoint);
    };

    class SolidColor : public Pattern
    {
    public:
        SolidColor(Color c) : Pattern(nullptr, nullptr), color(c) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<SolidColor>(*this); }

        Color color;

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override { return color; }
    };

    inline SolidColor SolidWhite = SolidColor(White);
    inline SolidColor SolidBlack = SolidColor(Black);

    class TestPattern : public Pattern
    {
    public:
        TestPattern(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<TestPattern>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override { return Color(patternPoint.x, patternPoint.y, patternPoint.z); }
    };

    class StripePattern : public Pattern
    {
    public:
        StripePattern(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<StripePattern>(*this); }

        // Test Function Only
        Color StripeAt(Math::Point p) { return CustomSampleAt(p); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Gradient : public Pattern
    {
    public:
        Gradient(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Gradient>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Ring : public Pattern
    {
    public:
        Ring(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Ring>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Checker : public Pattern
    {
    public:
        Checker(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Checker>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class RadialGradient : public Pattern
    {
    public:
        RadialGradient(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<RadialGradient>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Blend : public Pattern
    {
    public:
        Blend(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : Pattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Blend>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Perturb : public Pattern
    {
    public:
        Perturb(Pattern *c_A, int seed = 1337) : Pattern(c_A, nullptr)
        {
            assert(a != nullptr && "Perturb specifically required a pattern reference to be passed in");
            p = Noise::Perlin(seed);
        }
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Perturb>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
        Noise::Perlin p;
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
        else if (std::is_same_v<T, Perturb>)
        {
            static_assert(!std::is_same_v<T, Perturb>, "Tried to make Perturb with the wrong overload.");
            return nullptr;
        }
        else
        {
            return std::make_unique<T>(a, b);
        }
    }

    template <typename T>
    std::unique_ptr<Pattern> MakePattern(Pattern *a, int seed)
    {
        if constexpr (!std::is_same_v<T, Perturb>)
        {
            if constexpr (std::is_same_v<T, Perturb>, "Wrong overload used to make this pattern. This wouldn't have a seed.")
                ;
            return nullptr;
        }

        return std::make_unique<Perturb>(a, seed);
    }

    inline bool operator==(const Pattern &self, const Pattern &other)
    {
        return typeid(self) == typeid(other) && self.a == other.a && self.b == other.b && self.transform == other.transform;
    }
}