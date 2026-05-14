#include "Comps.h"
#include "Intersection.h"
#include "IntersectionList.h"
#include "Ray.h"
#include "Shape.h"
#include "Constants.h"
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

        comp.overPoint = comp.point + comp.normal * EPSILON;
        comp.underPoint = comp.point - comp.normal * EPSILON;
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

    float SchlickFresnel(const Comps &comp)
    {
        // cosine of angle between eye and normal
        float cosAngle = DotProduct(comp.eye, comp.normal);

        // Internal reflection only occurs if n1 > n2 (We start inside the object)
        if (comp.n1 > comp.n2)
        {
            float n = comp.n1 / comp.n2;
            float sin2T = (n * n) * (1.0f - cosAngle * cosAngle);
            if (sin2T > 1.0f)
                return 1.0f;

            float cosT = sqrtf(1.0f - sin2T);
            cosAngle = cosT; // n1 > n2, so we'll use cosT
        }

        // This is from "Reflections and Refraction in Ray Tracing" by Bram de Greve.
        // Fresnel in my waves project was (1 - dot(normal, viewDir))^5, so I assume this is similar
        float r0 = powf((comp.n1 - comp.n2) / (comp.n1 + comp.n2), 2.0f);
        return r0 + (1.0f - r0) * powf(1.0f - cosAngle, 5.0f);
    }
}
