#pragma once
#include "IntersectionList.h"

class Shape {
protected:
    inline static int idIterator = 0; // Each shape gets a unique id
    Shape() : id(idIterator++) {}; // Protected constructor, class can't be made by programmer

public:
    const int id;
    virtual IntersectionList intersects() const = 0;
};