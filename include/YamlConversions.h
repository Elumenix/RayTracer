#pragma once
#include <yaml-cpp/node/convert.h>
#include "YamlParser.h"

// Math
#include "Tuple.h"
#include "Matrix.h"
#include "Transformations.h" // Technically its own namespace

// Rendering
#include "Color.h"
#include "Material.h"
#include "Pattern.h"

// Scene
#include "Light.h"
#include "Sphere.h"
#include "Cube.h"
#include "Plane.h"
#include "Camera.h"

// Utility
#include <unordered_set>

// Because of how this project is designed, the program will only ever be reading Yaml files a user writes by hand not creating them
// Because of this, we don't need encode functions on anything, so you'll see only the first few objects have them before I realized this
namespace YAML
{
    // Utility function to make output for Basic data structures (Like Point, Vector, Color) much easier
    template <typename T>
    T safe_as(const YAML::Node &node, const std::string &fieldName, const std::string &objectName)
    {
        try
        {
            return node.as<T>();
        }
        catch (const YAML::Exception &e)
        {
            throw std::runtime_error("Value for '" + fieldName + "' on " + objectName + " is invalid");
        }
    }

    template <>
    struct convert<Math::Point>
    {
        static Node encode(const Math::Point &rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node &node, Math::Point &rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs = Math::Point(node[0].as<float>(), node[1].as<float>(), node[2].as<float>());
            return true;
        }
    };

    template <>
    struct convert<Math::Vector>
    {
        static Node encode(const Math::Vector &rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node &node, Math::Vector &rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs = Math::Vector(node[0].as<float>(), node[1].as<float>(), node[2].as<float>());
            return true;
        }
    };

    template <>
    struct convert<Rendering::Color>
    {
        static Node encode(const Rendering::Color &rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node &node, Rendering::Color &rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            float r = node[0].as<float>();
            float g = node[1].as<float>();
            float b = node[2].as<float>();

            // Valid color values are required, not just numbers
            if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1)
                return false;

            rhs = Rendering::Color(r, g, b);
            return true;
        }
    };

    template <>
    struct convert<Scene::Light>
    {
        static Node encode(const Scene::Light &rhs)
        {
            Node node;
            node["at"] = rhs.position;
            node["intensity"] = rhs.intensity;
            return node;
        }

        static bool decode(const Node &node, Scene::Light &rhs)
        {
            if (!node["at"] || !node["intensity"])
            {
                throw std::runtime_error("Missing required light fields: 'at' and/or 'intensity'");
            }

            if (node.size() != 3) // add, at, intensity
            {
                throw std::runtime_error("Light must have exactly 2 fields: 'at' and 'intensity'");
            }

            rhs = Scene::Light(
                safe_as<Math::Point>(node["at"], "at", "light"),
                safe_as<Rendering::Color>(node["intensity"], "intensity", "light"));
            return true;
        }
    };

    template <>
    struct convert<Scene::Sphere>
    {
        static bool decode(const Node &node, Scene::Sphere &rhs)
        {
            if (node.size() != 2) // material, transform
            {
                throw std::runtime_error("Sphere must have exactly 2 fields: 'material' and 'transform'");
            }

            rhs = Scene::Sphere();

            // Structure of a decoded sphere is guaranteed because the YamlParser enforces it
            rhs.material = node["material"].as<Rendering::Material>();
            rhs.transform = node["transform"].as<Math::Matrix<4, 4>>();

            return true;
        }
    };

    template <>
    struct convert<Scene::Cube>
    {
        static bool decode(const Node &node, Scene::Cube &rhs)
        {
            if (node.size() != 2) // material, transform
            {
                throw std::runtime_error("Cube must have exactly 2 fields: 'material' and 'transform'");
            }

            rhs = Scene::Cube();

            // Structure of a decoded cube is guaranteed because the YamlParser enforces it
            rhs.material = node["material"].as<Rendering::Material>();
            rhs.transform = node["transform"].as<Math::Matrix<4, 4>>();

            return true;
        }
    };

    template <>
    struct convert<Scene::Plane>
    {
        static bool decode(const Node &node, Scene::Plane &rhs)
        {
            if (node.size() != 2) // material, transform
            {
                throw std::runtime_error("Plane must have exactly 2 fields: 'material' and 'transform'");
            }

            rhs = Scene::Plane();

            // Structure of a decoded plane is guaranteed because the YamlParser enforces it
            rhs.material = node["material"].as<Rendering::Material>();
            rhs.transform = node["transform"].as<Math::Matrix<4, 4>>();

            return true;
        }
    };

    static const std::unordered_set<std::string> materialKeys = {
        "color",
        "pattern",
        "ambient",
        "diffuse",
        "specular",
        "shininess",
        "reflective",
        "transparency",
        "refractive-index"};

    template <>
    struct convert<Rendering::Material>
    {
        static bool decode(const Node &node, Rendering::Material &rhs)
        {

            // Everything on material is optional, so we'll initialize it with default values
            rhs = Rendering::Material();

            // Only one of these should be used
            if (node["color"] && node["pattern"])
            {
                throw std::runtime_error("Material cannot have both 'color' and 'pattern'");
            }

            // We'll iterate through each key-value pair in the node so that we can properly error check
            // This will, of course, be slightly slower than just checking keys, but we only need to run this code once
            for (const auto &kv : node)
            {
                std::string key = kv.first.as<std::string>();

                // If this is a fake key, we should return a warning to the user
                if (!materialKeys.count(key))
                {
                    throw std::runtime_error("Unknown material key: " + key);
                }

                if (key == "color")
                {
                    if (!kv.second.IsSequence() || kv.second.size() != 3)
                        throw std::runtime_error("Value for 'color' on material is invalid");

                    rhs.color = kv.second.as<Rendering::Color>();
                    continue;
                }

                if (key == "pattern")
                {
                    Rendering::Pattern *temp = kv.second.as<Rendering::Pattern *>();
                    rhs.pattern = std::unique_ptr<Rendering::Pattern>(temp);
                    continue;
                }

                // Everything else is a float, so we can check those
                if (!kv.second.IsScalar())
                {
                    throw std::runtime_error("Value for '" + key + "' on material is invalid");
                }

                float value = kv.second.as<float>();

                // Quick value confirmation
                if ((value < 0 || value > 1) && !(key == "shininess" || key == "refractive-index")) {
                    throw std::runtime_error("Value for '" + key + "' must be a number from 0 to 1");
                }
                else if (value < 0) {
                    throw std::runtime_error("Value for '" + key + "' must be greateater than or equal to 0");
                }

                if (key == "ambient")
                    rhs.ambient = value;
                else if (key == "diffuse")
                    rhs.diffuse = value;
                else if (key == "specular")
                    rhs.specular = value;
                else if (key == "shininess")
                    rhs.shininess = value;
                else if (key == "reflective")
                    rhs.reflective = value;
                else if (key == "transparency")
                    rhs.transparency = value;
                else if (key == "refractive-index")
                    rhs.refractiveIndex = value;
            }

            return true;
        }
    };

    template <>
    struct convert<Math::Matrix<4, 4>>
    {
        static bool decode(const Node &node, Math::Matrix<4, 4> &rhs)
        {
            // Everything on a transformation is optional, so we'll initialize it with default values
            rhs = Math::IdentityMatrix;

            for (const auto &step : node)
            {
                std::string type = step[0].as<std::string>();

                if (type == "rotate-x")
                {
                    if (step.size() != 2 || !step[1].IsScalar())
                    {
                        throw std::runtime_error("Value for 'rotation-x' on transform is invalid");
                    }
                    rhs = Transformations::RotationX(step[1].as<float>()) * rhs;
                }
                else if (type == "rotate-y")
                {
                    if (step.size() != 2 || !step[1].IsScalar())
                    {
                        throw std::runtime_error("Value for 'rotation-y' on transform is invalid");
                    }
                    rhs = Transformations::RotationY(step[1].as<float>()) * rhs;
                }
                else if (type == "rotate-z")
                {
                    if (step.size() != 2 || !step[1].IsScalar())
                    {
                        throw std::runtime_error("Value for 'rotation-z' on transform is invalid");
                    }
                    rhs = Transformations::RotationZ(step[1].as<float>()) * rhs;
                }
                else if (type == "translate")
                {
                    if (step.size() != 4 || !step[1].IsScalar() || !step[2].IsScalar() || !step[3].IsScalar())
                    {
                        throw std::runtime_error("Value for 'translation' on transform is invalid");
                    }
                    rhs = Transformations::Translation(
                              step[1].as<float>(),
                              step[2].as<float>(),
                              step[3].as<float>()) *
                          rhs;
                }
                else if (type == "scale")
                {
                    if (step.size() != 4 || !step[1].IsScalar() || !step[2].IsScalar() || !step[3].IsScalar())
                    {
                        throw std::runtime_error("Value for 'scale' on transform is invalid");
                    }
                    rhs = Transformations::Scaling(
                              step[1].as<float>(),
                              step[2].as<float>(),
                              step[3].as<float>()) *
                          rhs;
                }
                else if (type == "shear")
                {
                    if (step.size() != 7 || !step[1].IsScalar() || !step[2].IsScalar() || !step[3].IsScalar() || !step[4].IsScalar() || !step[5].IsScalar() || !step[6].IsScalar())
                    {
                        throw std::runtime_error("Value for 'shear' on transform is invalid");
                    }
                    rhs = Transformations::Shearing(
                              step[1].as<float>(),
                              step[2].as<float>(),
                              step[3].as<float>(),
                              step[4].as<float>(),
                              step[5].as<float>(),
                              step[6].as<float>()) *
                          rhs;
                }
                else
                {
                    throw std::runtime_error("Unknown transform type: " + type);
                }
            }

            return true;
        }
    };

    // These are all points or vetcors, so we can safely convert them
    static const std::unordered_set<std::string> cameraKeys = {
        "add", // Because add was required to create the camera, it is still part of the node
        "width",
        "height",
        "field-of-view",
        "reflection-depth", // Not actually used in camera class, but the user will put it on the camera
        "from",
        "to",
        "up",
    };

    template <>
    struct convert<Scene::Camera>
    {
        static bool decode(const Node &node, Scene::Camera &rhs)
        {
            // We'll check this at the beginning to make sure theres no funny business going on
            for (const auto &key : node)
            {
                if (!cameraKeys.count(key.first.as<std::string>()))
                {
                    throw std::runtime_error("Unknown key on camera: " + key.first.as<std::string>());
                }
            }

            // Make sure the required keys are present
            if (!node["width"] || !node["height"] || !node["field-of-view"])
            {
                throw std::runtime_error("Camera requires a 'width', 'height', and 'field-of-view' key");
            }

            rhs = Scene::Camera(
                safe_as<int>(node["width"], "width", "camera"),
                safe_as<int>(node["height"], "height", "camera"),
                safe_as<float>(node["field-of-view"], "field-of-view", "camera"));

            if (rhs.fov <= 0 || rhs.fov >= 3.14159)
            {
                throw std::runtime_error("Value given for 'field-of-view' is invalid: 0 < 'field-of-view' < 3.14159");
            }

            // Some default basis vectors
            Math::Point from(0, 0, 0);
            Math::Point to(0, 0, -1);
            Math::Vector up(0, 1, 0);

            if (node["from"])
            {
                from = safe_as<Math::Point>(node["from"], "from", "camera");
            }
            if (node["to"])
            {
                to = safe_as<Math::Point>(node["to"], "to", "camera");
            }
            if (node["up"])
            {
                up = safe_as<Math::Vector>(node["up"], "up", "camera");
            }

            rhs.transform = Transformations::ViewTransform(from, to, up);

            return true;
        }
    };

    // All pattern names
    static const std::unordered_set<std::string> patternValues = {
        "checkers",
        "stripes",
        "gradient",
        "ring",
        "radial",
        "blend",
        "perturb"};

    // Pattern is an abstract class, so we're returning a pointer to a pattern instead
    template <>
    struct convert<Rendering::Pattern *>
    {
        // We'll have problems if the first and second pointers are deallocated at the end of decode, so we're saving them until the next time yaml parses
        // This lets this method work and lets us still avoid memory leaks
        static inline std::vector<Rendering::Pattern *> allocated;

        static bool decode(const Node &node, Rendering::Pattern *&rhs)
        {
            // Look for required keys
            if (!node["type"] || !node["colors"])
            {
                throw std::runtime_error("Missing required pattern fields: 'type' and/or 'colors'");
            }

            // We'll confirm all keys are valid
            for (const auto &kv : node)
            {
                std::string key = kv.first.as<std::string>();

                // If this is a fake key, we should return a warning to the user
                if (key != "type" && key != "colors" && key != "transform" && key != "seed" && key != "strength")
                {
                    throw std::runtime_error("Unknown key on pattern: " + key);
                }
            }

            std::string type = node["type"].as<std::string>();
            Node colorNode = node["colors"];

            // We make sure the colorNode is correct at a glance
            if (!colorNode.IsSequence() || colorNode.size() != 2 || type == "perturb")
            {
                if (type != "perturb")
                {
                    throw std::runtime_error("'colors' should have 2 items for this pattern, with allowed values being of type 'color' and 'pattern'");
                }

                // perturb works differently, so we have a final check for improper formatting
                else if (!colorNode.IsSequence() || colorNode.size() != 1)
                {
                    throw std::runtime_error("'colors' should have 1 item for this pattern, with allowed values being of type 'color' and 'pattern'");
                }
            }

            Rendering::Pattern *first;
            if (colorNode[0].IsSequence())
            {
                // This is color that we need to turn into a solid color pattern
                Rendering::Color color = safe_as<Rendering::Color>(colorNode[0], "colors[0]", "pattern");
                first = new Rendering::SolidColor(color);
            }
            else
            {
                // This is another pattern and we need to treat it as such
                first = colorNode[0].as<Rendering::Pattern *>();
            }

            Rendering::Pattern *second;
            if (type != "perturb")
            {
                if (colorNode[1].IsSequence())
                {
                    // This is color that we need to turn into a solid color pattern
                    Rendering::Color color = safe_as<Rendering::Color>(colorNode[1], "colors[1]", "pattern");
                    second = new Rendering::SolidColor(color);
                }
                else
                {
                    // This is another pattern and we need to treat it as such
                    second = colorNode[1].as<Rendering::Pattern *>();
                }
            }

            // Seed will only affect perturb patterns, so it is completely optional
            // We won't throw errors whether or not a seed key is present regardless of circumstance, to keep things simple
            int seed = 1337;
            if (node["seed"])
            {
                seed = node["seed"].as<int>();
            }

            // Now we will finally make the actual pattern
            if (type == "checkers")
            {
                rhs = Rendering::MakePattern<Rendering::Checker>(first, second).release();
            }
            else if (type == "stripes")
            {
                rhs = Rendering::MakePattern<Rendering::StripePattern>(first, second).release();
            }
            else if (type == "gradient")
            {
                rhs = Rendering::MakePattern<Rendering::Gradient>(first, second).release();
            }
            else if (type == "ring")
            {
                rhs = Rendering::MakePattern<Rendering::Ring>(first, second).release();
            }
            else if (type == "radial")
            {
                rhs = Rendering::MakePattern<Rendering::RadialGradient>(first, second).release();
            }
            else if (type == "blend")
            {
                rhs = Rendering::MakePattern<Rendering::Blend>(first, second).release();
            }
            else if (type == "perturb")
            {
                float strength = .3f;

                if (node["strength"])
                {
                    strength = safe_as<float>(node["strength"], "strength", "pattern");
                }

                rhs = Rendering::MakePattern<Rendering::Perturb>(first, seed, strength).release();
            }
            else
            {
                throw std::runtime_error("Unknown pattern type: " + type);
            }

            // Transform is an optional key. The Transform converter above already handles errors for this properly
            // Transforms automatically set to an identity matrix, so it's fine if this isn't set from the get-go
            if (node["transform"])
            {
                rhs->transform = node["transform"].as<Math::Matrix<4, 4>>();
            }

            return true;
        }

        // Cleaning up previous pointers to avoid memory leaks
        static void ClearAll()
        {
            for (auto *p : allocated)
                delete p;
            allocated.clear();
        }
    };
}
