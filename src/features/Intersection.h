#pragma once

namespace Scene
{
    class Shape;
}

namespace Rendering
{
    struct Intersection
    {
        float t; // should be const but that breaks the move operator
        const Scene::Shape *object;

        Intersection(float m_t, const Scene::Shape *m_object) : t(m_t), object(m_object) {};

        bool operator<(const Intersection &other) const { return t < other.t; }
        bool operator>(const Intersection &other) const { return t > other.t; }
        bool operator<=(const Intersection &other) const { return t <= other.t; }
        bool operator>=(const Intersection &other) const { return t >= other.t; }
    };
}