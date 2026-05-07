#include "Comps.h"
#include "Intersection.h"
#include "Ray.h"
#include "Shape.h"

namespace Rendering
{
    Comps PrepareComputation(const Intersection &i, const Ray &r)
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

        comp.over_point = comp.point + comp.normal * Scene::EPSILON;

        return comp;
    }
}
