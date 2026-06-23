#pragma once
#include <yaml-cpp/node/convert.h>
#include "YamlParser.h"

// Math
#include "Tuple.h"

// Rendering
#include "Color.h"
#include "Material.h"

// Scene
#include "Light.h"
#include "Sphere.h"

#include "Camera.h"
#include "Matrix.h"
#include "Transformations.h"

// Because of how this project is designed, the program will only ever be reading Yaml files a user writes by hand not creating them
// Because of this, we don't need encode functions on anything, so you'll see only the first few objects have them before I realized this
namespace YAML
{
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

            rhs = Rendering::Color(node[0].as<float>(), node[1].as<float>(), node[2].as<float>());
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
                return false;

            rhs = Scene::Light(
                node["at"].as<Math::Point>(),
                node["intensity"].as<Rendering::Color>());
            return true;
        }
    };

    template <>
    struct convert<Scene::Sphere>
    {
        static bool decode(const Node &node, Scene::Sphere &rhs)
        {
            rhs = Scene::Sphere();

            // Structure of a decoded sphere is garunteed because the YamlParser enforces it
            rhs.material = node["material"].as<Rendering::Material>();
            rhs.transform = node["transform"].as<Math::Matrix<4, 4>>();

            return true;
        }
    };

    template <>
    struct convert<Rendering::Material>
    {
        static bool decode(const Node &node, Rendering::Material &rhs)
        {
            // Everything on material is optional, so we'll initialize it with default values
            rhs = Rendering::Material();

            if (node["color"])
                rhs.color = node["color"].as<Rendering::Color>();
            if (node["ambient"])
                rhs.ambient = node["ambient"].as<float>();
            if (node["diffuse"])
                rhs.diffuse = node["diffuse"].as<float>();
            if (node["specular"])
                rhs.specular = node["specular"].as<float>();
            if (node["shininess"])
                rhs.shininess = node["shininess"].as<float>();
            if (node["reflective"])
                rhs.reflective = node["reflective"].as<float>();
            if (node["transparency"])
                rhs.transparency = node["transparency"].as<float>();
            if (node["refractive-index"])
                rhs.refractiveIndex = node["refractive-index"].as<float>();

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
                    rhs = rhs * Transformations::RotationX(step[1].as<float>());
                else if (type == "rotate-y")
                    rhs = rhs * Transformations::RotationY(step[1].as<float>());
                else if (type == "rotate-z")
                    rhs = rhs * Transformations::RotationZ(step[1].as<float>());
                else if (type == "translate")
                    rhs = rhs * Transformations::Translation(
                                    step[1].as<float>(),
                                    step[2].as<float>(),
                                    step[3].as<float>());
                else if (type == "scale")
                    rhs = rhs * Transformations::Scaling(
                                    step[1].as<float>(),
                                    step[2].as<float>(),
                                    step[3].as<float>());
                else if (type == "shear")
                    rhs = rhs * Transformations::Shearing(
                                    step[1].as<float>(),
                                    step[2].as<float>(),
                                    step[3].as<float>(),
                                    step[4].as<float>(),
                                    step[5].as<float>(),
                                    step[6].as<float>());
            }

            return true;
        }
    };

    /*
    template <>
    struct convert<Scene::Camera>
    {

        static Node encode(const Scene::Camera &rhs)
        {
            Node node;
            node["width"] = rhs.hsize;
            node["height"] = rhs.vsize;
            node["field-of-view"] = rhs.fov;

            Transformations::CameraVectors cv = Transformations::InvertViewTransform(rhs.transform);
            node["from"] = cv.from;
            node["to"] = cv.to;
            node["up"] = cv.up;

            return node;
        }


        static bool decode(const Node &node, Scene::Camera &rhs)
        {
            if (!node["width"] || !node["height"] || !node["field-of-view"])
                return false;

            rhs = Scene::Camera(
                node["width"].as<int>(),
                node["height"].as<int>(),
                node["field-of-view"].as<float>());

            // Some default basis vectors
            Math::Point from(0, 0, 0);
            Math::Point to(0, 0, -1);
            Math::Vector up(0, 1, 0);

            if (node["from"])
            {
                from = node["from"].as<Math::Point>();
            }
            if (node["to"])
            {
                to = node["to"].as<Math::Point>();
            }
            if (node["up"])
            {
                up = node["up"].as<Math::Vector>();
            }

            rhs.transform = Transformations::ViewTransform(from, to, up);

            return true;
        }
    };*/
}
