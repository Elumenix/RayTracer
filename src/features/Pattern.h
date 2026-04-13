#pragma once
#include <memory>
#include "Color.h"

inline Color White = Color(1,1,1);
inline Color Black = Color(0,0,0);

class Pattern {
public:
    Color a;
    Color b;

    std::unique_ptr<Pattern> clone() const {
        return std::make_unique<Pattern>(*this);
    }

//protected:
    Pattern(Color c_A, Color c_B) : a(c_A), b(c_B) {}
    Color StripeAt(Point p) { return (static_cast<int>(floor(p.x)) % 2 == 0 ? a : b); }
};

/*class StripePattern : public Pattern {
public:
    StripePattern(Color c_A, Color c_B) : Pattern(c_A, c_B) {}
    Color StripeAt(Point p) { return (static_cast<int>(floor(p.x)) % 2 == 0 ? a : b); }
};
*/

bool operator==(const Pattern& self, const Pattern& other) {
    return typeid(self) == typeid(other) && self.a == other.a && self.b == other.b;
}