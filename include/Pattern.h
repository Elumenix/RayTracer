#pragma once
#include <memory>
#include "Color.h"
#include "Noise.h"
#include "Matrix.h"
#include <assert.h>

// Forward declare
namespace Scene
{
    class Shape;
}

inline Rendering::Color White = Rendering::Color(1, 1, 1);
inline Rendering::Color Black = Rendering::Color(0, 0, 0);

namespace Rendering
{
    // Abstract base class, can't be created directly
    class Pattern
    {
    public:
        Math::Matrix<4, 4> transform = Math::IdentityMatrix;
        virtual std::unique_ptr<Pattern> Clone() const = 0;

        // Expected access
        Color SampleAt(const Scene::Shape &object, const Math::Point &worldPoint);

        virtual ~Pattern() = default;

        // equal operator is being given access to protected members of the class
        friend bool operator==(const Pattern &p1, const Pattern &p2)
        {
            return typeid(p1) == typeid(p2) && p1.transform == p2.transform && p1.EqualsSameType(p2);
        }

    protected:
        virtual Color CustomSampleAt(const Math::Point &patternPoint) const = 0;
        friend Color Sample(const Pattern *p, const Math::Point &pt); // Allows access to the function below
        virtual bool EqualsSameType(const Pattern &other) const { return transform == other.transform; }
    };

    // This is a free function because a->Sample(b, pos) is bad syntax when 'a' is not used in the result at all
    inline Color Sample(const Pattern *child, const Math::Point &parentSpacePoint)
    {
        Math::Point childPoint = child->transform.Inverse() * parentSpacePoint;
        return child->CustomSampleAt(childPoint);
    }

    // Leaf Node, essentially
    class SolidColor : public Pattern
    {
    public:
        SolidColor(Color c) : color(c) {}

        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<SolidColor>(*this); }
        Color CustomSampleAt(const Math::Point &patternPoint) const override { return color; }
        bool EqualsSameType(const Pattern &other) const override
        {
            auto &o = static_cast<const SolidColor &>(other); // safe because of preconditions
            return Pattern::EqualsSameType(other) && color == o.color;
        }

        Color color;
    };

    // Useful default values
    inline SolidColor SolidWhite = SolidColor(White);
    inline SolidColor SolidBlack = SolidColor(Black);

    // Abstract base for patterns that hold only 1 child
    class UnaryPattern : public Pattern
    {
    public:
        Pattern *a;

    protected:
        UnaryPattern(Pattern *input) : a(input) {}

        virtual bool EqualsSameType(const Pattern &other) const override
        {
            auto &o = static_cast<const UnaryPattern &>(other); // safe because of preconditions
            return Pattern::EqualsSameType(other) && *a == *o.a;
        }
    };

    // Abstract base for patterns that hold 2 children
    class BinaryPattern : public Pattern
    {
    public:
        Pattern *a;
        Pattern *b;

    protected:
        BinaryPattern(Pattern *a, Pattern *b) : a(a), b(b) {}

        virtual bool EqualsSameType(const Pattern &other) const override
        {
            auto &o = static_cast<const BinaryPattern &>(other); // safe because of preconditions
            return Pattern::EqualsSameType(other) && *a == *o.a && *b == *o.b;
        }
    };

    class TestPattern : public BinaryPattern
    {
    public:
        TestPattern(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : BinaryPattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<TestPattern>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class StripePattern : public BinaryPattern
    {
    public:
        StripePattern(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : BinaryPattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<StripePattern>(*this); }

        // Test Function Only
        Color StripeAt(Math::Point p) { return CustomSampleAt(p); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Gradient : public BinaryPattern
    {
    public:
        Gradient(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : BinaryPattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Gradient>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Ring : public BinaryPattern
    {
    public:
        Ring(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : BinaryPattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Ring>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Checker : public BinaryPattern
    {
    public:
        Checker(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : BinaryPattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Checker>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class RadialGradient : public BinaryPattern
    {
    public:
        RadialGradient(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : BinaryPattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<RadialGradient>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Blend : public BinaryPattern
    {
    public:
        Blend(Pattern *c_A = &SolidWhite, Pattern *c_B = &SolidBlack) : BinaryPattern(c_A, c_B) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Blend>(*this); }

    private:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
    };

    class Perturb : public UnaryPattern
    {
    public:
        Perturb(Pattern *c_A, int seed = 1337) : UnaryPattern(c_A), seed(seed), p(Noise::Perlin(seed)) {}
        std::unique_ptr<Pattern> Clone() const override { return std::make_unique<Perturb>(*this); }

    protected:
        Color CustomSampleAt(const Math::Point &patternPoint) const override;
        Noise::Perlin p;
        int seed;

        // Overriden to check for new variables
        bool EqualsSameType(const Pattern &other) const override
        {
            auto &o = static_cast<const Perturb &>(other); // safe because of preconditions

            // p will always equal o.p if the seed is the same, as we don't allow changing the seed after
            return Pattern::EqualsSameType(other) && UnaryPattern::EqualsSameType(other) && seed == o.seed;
        }
    };

    // Factory Methods for creating patterns that will automatically be in a smart pointer
    // Unary version
    template <typename T, typename... Args>
    std::unique_ptr<T> MakePattern(Pattern *a, Args &&...args)
    {
        static_assert(std::is_base_of_v<UnaryPattern, T>, "MakePattern(Pattern*, ...) is only for unary patterns.");
        return std::make_unique<T>(a, std::forward<Args>(args)...);
    }

    // Binary Version
    template <typename T>
    std::unique_ptr<T> MakePattern(Pattern *a = &SolidWhite, Pattern *b = &SolidBlack)
    {
        static_assert(std::is_base_of_v<BinaryPattern, T>, "MakePattern(Pattern*, Pattern*) is only for binary patterns.");
        return std::make_unique<T>(a, b);
    }
}
