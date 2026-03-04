#pragma once
#include <vector>
#include "Intersection.h"

class IntersectionList {
private:
    std::vector<Intersection> list;

public:
    // Constructor
    IntersectionList() = default;
    IntersectionList(std::vector<Intersection> m_list) : list(m_list){};
    IntersectionList(std::initializer_list<Intersection> values) : list(values){};

    // Access
    Intersection* operator[](std::size_t index) { return &list[index]; }
    const Intersection* operator[](std::size_t index) const { return &list[index]; }

    // Methods
    std::size_t size() { return list.size(); }
};