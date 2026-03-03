#pragma once
#include "Transformations.h"

class Sphere {
private:
    // Shared iterator makes it so every sphere will have a unique ID
    inline static int idIterator = 0;

public:
    const int id;
    Sphere() : id(idIterator++) {}
};