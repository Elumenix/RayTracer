#include "IntersectionList.h"
#include <algorithm>

namespace Rendering
{
    // Returns Intersection with smallest t value greater than 0
    const Intersection *IntersectionList::Hit() const
    {
        const Intersection *result = nullptr;
        int n = Size();

        for (auto i = 0; i < n; i++)
        {
            if (list[i].t > 0 && (result == nullptr || list[i].t < result->t))
            {
                result = &(list[i]);
            }
        }

        return result;
    }

    void IntersectionList::Merge(IntersectionList &&other)
    {
        auto middle = list.insert(list.end(), other.list.begin(), other.list.end());
        std::inplace_merge(list.begin(), middle, list.end());
    }

    void IntersectionList::Sort()
    {
        std::sort(list.begin(), list.end());
    }
}
