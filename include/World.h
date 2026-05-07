#pragma once

#include "Light.h" // Needes so vector knows sizing
#include "Shape.h" // Used in template, so can't be forward declared
#include "Comps.h" // This is the only class that uses this, so I'm not forward declaring it
#include <vector>

namespace Rendering
{
    class Ray;
}

namespace Math
{
    class Point;
}

namespace Scene
{
    class World
    {
    public:
        std::vector<Scene::Light> lights;
        std::vector<std::unique_ptr<Scene::Shape>> shapes;
        World() = default;
        static World Default();

        // Allow moving so that World can be used in returns
        World(World &&) = default;
        World &operator=(World &&) = default;

        // For the love of god, do not allow copy constructors. This bug was really hard to find. Unique ptrs make this problematic.
        World(const World &) = delete;
        World &operator=(const World &) = delete;

        // This add function is designed so that the class takes full ownership of objects, making it so where they were passed from can't use them anymore
        // Either put the constructor in here or use std::move on established objects
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<Scene::Shape, T>>>
        void Add(T &&shape)
        {
            static_assert(std::is_base_of<Scene::Shape, T>::value, "T must derive from S");
            shapes.push_back(std::make_unique<T>(std::move(shape)));
        };

        void Add(const Scene::Light &light) { lights.push_back(light); }
        const Rendering::IntersectionList IntersectWorld(const Rendering::Ray &r) const;
        Rendering::Color ShadeHit(const Rendering::Comps &comp) const;
        Rendering::Color ColorAt(const Rendering::Ray &r) const;
        bool IsShadowed(const Math::Point &p, const Scene::Light &light) const;
    };
}
