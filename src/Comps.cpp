#include "Comps.h"
#include "Intersection.h"
#include "IntersectionList.h"
#include "Ray.h"
#include "Shape.h"
#include "Constants.h"
#include <algorithm>
#include <array>

using namespace Scene;

namespace Rendering
{
    /// @brief Compiles useful values created by a ray at an intersection so that they don't need to be recomputed later
    /// @param i The intersection we want to create values from
    /// @param r The ray that would hit at the intersection
    /// @return Compiled object of values deduced from the ray and intersection
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

    // This was the original implementation of GetNValues, but uncertain memory didn't play well with multi-threading, so it was replaced
    // This still works as a safe backup, if the assumptions for the original become false at runtime
    void GetNValuesSlow(Comps &comp, const Intersection &i, const IntersectionList &xs) {
        std::vector<const Shape *> container;
        container.reserve(xs.Size());
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

                return;
            }
        }

        assert(false && "The passed Intersection 'i' was not contained within the IntersectionList 'xs'"); // This should not happen
    }

    /// @brief Calculates the n values at an intersection, updating comp to define what a ray is inside before and after this intersection
    /// @param comp The computed values bundle for the shading at the current intersection
    /// @param i The Intersection that is being shaded. It is assumed this intersection is in the IntersectionList
    /// @param xs A list of all the Intersection that some Ray 'r' passes through.
    void GetNValues(Comps &comp, const Intersection &i, const IntersectionList &xs)
    {
        // As an optimization here, we're assuming a ray will not pass through more than 32 object sides (Shape: 2 sides, Plane 1 side)
        // There's potential that this isn't true if a ray manages to pass through more than 16 objects, so that case we'll use a backup function instead
        if (xs.Size() > 16) {
            GetNValuesSlow(comp, i, xs);
            return;
        }

        // This prevents array resizing, which will have a strong impact on perfomance as this is expected to be multi-threaded
        // 32 is a pretty safe number as it is unlikely for a ray in most scenes to pass through more than 16 objects
        constexpr std::size_t maxContainerSize = 32;
        std::array<const Shape*, maxContainerSize> container;
        std::size_t containerSize = 0;

        const Intersection *hit = &i;

        for (const Intersection &intersect : xs)
        {
            if (&intersect == hit) 
            {
                // Represents the refractive index of the material the ray is exiting (1.0 represent air)
                comp.n1 = (containerSize == 0) ? 1.0f : container[containerSize-1]->material.refractiveIndex;
            }

            // find out if intersect is already in the container
            std::size_t foundIdx = containerSize;
            for (std::size_t j = 0; j < containerSize; j++) {
                if (container[j] == intersect.object) { foundIdx = j; break; }
            }

            // If element is already in the container, remove it
            if (foundIdx != containerSize)
            {
                // We can erase by shifting the container down
                for (std::size_t j = foundIdx; j + 1 < containerSize; j++) {
                    container[j] = container[j+1];
                }
                containerSize--;
            }
            else
            { 
                // Add it otherwise
                container[containerSize++] = intersect.object;
            }

            if (&intersect == hit)
            {
                // Represents the refractive index of the material the ray is entering (1.0 represent air)
                comp.n2 = (containerSize == 0) ? 1.0f : container[containerSize-1]->material.refractiveIndex;
                return;
            }
        }

        assert(false && "The passed Intersection 'i' was not contained within the IntersectionList 'xs'"); // This should not happen
    }

    /// @brief Quickly assembles all important values from a ray/intersection
    /// @param i An intersection in the scene
    /// @param r The ray that hits the intersection
    /// @return The assembled values from the ray/intersection
    Comps PrepareComputation(const Intersection &i, const Ray &r)
    {
        Comps comp = CompCreation(i, r);
        IntersectionList xs = {i}; // List only has i
        GetNValues(comp, *xs[0], xs);
        return comp;
    }

    /// @brief Quickly assembles all important values from a ray/intersection
    /// @param i An intersection in the scene
    /// @param r The ray that hits the intersection
    /// @param xs A referenced intersection list, passed so additional memory doesn't need to be created for each call
    /// @return The assembled values from the ray/intersection
    Comps PrepareComputation(const Intersection &i, const Ray &r, const IntersectionList &xs)
    {
        Comps comp = CompCreation(i, r);
        GetNValues(comp, i, xs);
        return comp;
    }

    /// @brief Calculates a fresnel strength, which is essentially a factor of how shiny/reflective something is based on the angle it's viewed.
    /// @param comp Assempled list if values from an ray/intersection to allow for quicker math
    /// @return Calculated fresnel term
    float SchlickFresnel(const Comps &comp)
    {
        // cosine of angle between eye and normal
        float cos = DotProduct(comp.eye, comp.normal);

        // Internal reflection only occurs if n1 > n2 (We start inside the object)
        if (comp.n1 > comp.n2)
        {
            float n = comp.n1 / comp.n2;
            float sin2T = (n * n) * (1.0f - cos * cos);
            if (sin2T > 1.0f)
                return 1.0f;

            float cosT = std::sqrt(1.0f - sin2T);
            cos = cosT; // n1 > n2, so we'll use cosT
        }

        // This is from "Reflections and Refraction in Ray Tracing" by Bram de Greve.
        // Fresnel in my waves project was (1 - dot(normal, viewDir))^5, so I assume this is similar
        float r0 = std::pow((comp.n1 - comp.n2) / (comp.n1 + comp.n2), 2.0f);
        return r0 + (1.0f - r0) * std::pow(1.0f - cos, 5.0f);
    }
}
