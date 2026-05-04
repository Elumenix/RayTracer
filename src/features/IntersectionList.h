#pragma once
#include <vector>
#include "Intersection.h"

namespace Rendering
{
    class IntersectionList
    {
    private:
        std::vector<Intersection> list;

    public:
        // Constructor
        IntersectionList() = default;
        IntersectionList(std::vector<Intersection> m_list) : list(m_list) {}
        IntersectionList(std::initializer_list<Intersection> values) : list(values) {}

        // Access
        Intersection *operator[](std::size_t index) { return &list[index]; }
        const Intersection *operator[](std::size_t index) const { return &list[index]; }

        // Methods
        const Intersection *hit() const;
        std::size_t size() const { return list.size(); }
        void merge(IntersectionList &&other);
    };
}
