#pragma once
#include <vector>
#include "Intersection.h" // Required for vector sizing

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
        const Intersection *Hit() const;
        std::size_t Size() const { return list.size(); }
        void Merge(IntersectionList &&other);
        void Append(Intersection &&intersection) { list.push_back(std::move(intersection)); };
        void Clear() { list.clear(); };
        void Sort();
        constexpr void Reserve(std::size_t __n) { list.reserve(__n); };

        // Needed for range-based for loop (They are making be break my naming convention)
        auto begin() { return list.begin(); }
        auto end() { return list.end(); }
        auto begin() const { return list.cbegin(); }
        auto end() const { return list.cend(); }
    };
}
