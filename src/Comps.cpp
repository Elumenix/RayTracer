#include "Comps.h"
#include "Intersection.h"
#include "IntersectionList.h"
#include "Ray.h"
#include "Shape.h"
#include <algorithm>

using namespace Scene;

namespace Rendering
{
    Comps CompCreation(const Intersection &i, const Ray &r)
    {
        Comps comp;
        comp.t = i.t;
        comp.object = i.object;

        // Precompute the useful values
        comp.point = r.Position(i.t);
        comp.eye = -r.direction;
        comp.normal = i.object->NormalAt(comp.point);

        assert(!std::isnan(comp.point.z) && "point.z is NaN");
        assert(!std::isnan(comp.normal.z) && "normal.z is NaN");

        if (DotProduct(comp.normal, comp.eye) < 0)
        {
            comp.isInside = true;
            comp.normal = -comp.normal;
        }
        else
        {
            comp.isInside = false;
        }

        comp.overPoint = comp.point + comp.normal * Scene::EPSILON;
        comp.underPoint = comp.point - comp.normal * Scene::EPSILON;
        comp.reflect = Reflect(r.direction, comp.normal);

        return comp;
    }

    void GetNValues(Comps &comp, const Intersection &i, const Ray &r, const IntersectionList &xs)
    {
        std::vector<const Shape *> container;
        const Intersection *hit = &i;

        for (const Intersection &intersect : xs)
        {
            if (&intersect == hit) // This line broke my brain a bit
            {
                if (container.empty())
                { // container is empty
                    comp.n1 = 1.0;
                }
                else
                {
                    comp.n1 = container.back()->material.refractiveIndex;
                }
            }

            // find out if intersect is already in the container
            auto it = std::find_if(container.begin(), container.end(),
                                   [&](const Shape *shape)
                                   { return shape == intersect.object; });

            // If element is already in the container, remove it
            if (it != container.end())
            {
                container.erase(it);
            }
            else
            { // Add it otherwise
                container.push_back(intersect.object);
            }

            if (&intersect == hit)
            {
                if (container.empty())
                {
                    comp.n2 = 1.0;
                }
                else
                {
                    comp.n2 = container.back()->material.refractiveIndex;
                }

                break;
            }
        }
    }

    Comps PrepareComputation(const Intersection &i, const Ray &r)
    {
        Comps comp = CompCreation(i, r);
        IntersectionList xs; // Empty list if no list is given
        GetNValues(comp, i, r, xs);
        return comp;
    }

    Comps PrepareComputation(const Intersection &i, const Ray &r, const IntersectionList &xs)
    {
        Comps comp = CompCreation(i, r);
        GetNValues(comp, i, r, xs);
        return comp;
    }
}
