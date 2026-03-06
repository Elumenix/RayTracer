#include "IntersectionList.h"

const Intersection * IntersectionList::hit() const
{
    const Intersection* result = nullptr;
    int n = size();

    for (auto i = 0; i < n; i++) {
        if (list[i].t > 0 && (result == nullptr || list[i].t < result->t)) {
            result = &(list[i]);
        }
    }

    return result;
}