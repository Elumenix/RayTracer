#pragma once
#include <vector>
#include <memory>
#include <concepts>
#include "Light.h"
#include "Shape.h"
#include "Sphere.h"

class World {
public:
    std::vector<Light> lights;
    std::vector<std::unique_ptr<Shape>> shapes;
    World() = default;
    static World Default();


    // This add function is designed so that the class takes full ownership of objects, making it so where they were passed from can't use them anymore
    template <typename T>
    void add(T&& shape) {
        static_assert(std::is_base_of<Shape, T>::value, "T must derive from S"); 
        shapes.push_back(std::make_unique<T>(std::move(shape))); 
    };
    void add(Light light) { lights.push_back(light); }
    const IntersectionList intersectWorld(const Ray r) const;
    Color shade_hit(const Comps& comp) const;
    Color color_at(const Ray r) const;
};
