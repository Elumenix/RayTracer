#pragma once
#include <memory>
#include "Color.h"
#include "Matrix.h"

inline Color White = Color(1, 1, 1);
inline Color Black = Color(0, 0, 0);

class Shape; // Forward Declaration

class Pattern
{
public:
    Color a;
    Color b;
    Matrix<4, 4> transform = IdentityMatrix;

    std::unique_ptr<Pattern> clone() const
    {
        return std::make_unique<Pattern>(*this);
    }

    // protected:
    Pattern(Color c_A, Color c_B) : a(c_A), b(c_B) {}

    Color StripeAt(Point worldPoint);
    Color StripeAtObject(const Shape &object, Point worldPoint);
};

/*class StripePattern : public Pattern {
public:
    StripePattern(Color c_A, Color c_B) : Pattern(c_A, c_B) {}
    Color StripeAt(Point p) { return (static_cast<int>(floor(p.x)) % 2 == 0 ? a : b); }
};*/

inline bool operator==(const Pattern &self, const Pattern &other)
{
    return typeid(self) == typeid(other) && self.a == other.a && self.b == other.b;
}