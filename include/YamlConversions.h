#pragma once
#include <yaml-cpp/node/convert.h>
#include "Tuple.h"
#include "Color.h"
#include "Camera.h"
#include "Matrix.h"
#include "Transformations.h"
#include "Light.h"

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

    /*
    template <>
    struct convert<Math::Matrix<4, 4>>
    {
        static bool decode(const Node &node, Math::Matrix<4, 4> &rhs)
        {
            if (!node.IsSequence() || node.size() != 16)
                return false;

            rhs = Math::Matrix<4, 4>({node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>(),
                                      node[4].as<float>(), node[5].as<float>(), node[6].as<float>(), node[7].as<float>(),
                                      node[8].as<float>(), node[9].as<float>(), node[10].as<float>(), node[11].as<float>(),
                                      node[12].as<float>(), node[13].as<float>(), node[14].as<float>(), node[15].as<float>()});
            return true;
        }
    };

    // First, I need to handle matrices as they work weird
    static Math::Matrix<4, 4> decodeTransform(const Node &node)
    {
        Math::Matrix<4, 4> result = Math::IdentityMatrix;

        for (const auto &step : node)
        {
            std::string type = step[0].as<std::string>();

            if (type == "rotate-x")
                result = result * Transformations::RotationX(step[1].as<float>());
            else if (type == "rotate-y")
                result = result * Transformations::RotationY(step[1].as<float>());
            else if (type == "rotate-z")
                result = result * Transformations::RotationZ(step[1].as<float>());
            else if (type == "translate")
                result = result * Transformations::Translation(
                                      step[1].as<float>(),
                                      step[2].as<float>(),
                                      step[3].as<float>());
            else if (type == "scale")
                result = result * Transformations::Scaling(
                                      step[1].as<float>(),
                                      step[2].as<float>(),
                                      step[3].as<float>());
            else if (type == "shear")
                result = result * Transformations::Shearing(
                                      step[1].as<float>(),
                                      step[2].as<float>(),
                                      step[3].as<float>(),
                                      step[4].as<float>(),
                                      step[5].as<float>(),
                                      step[6].as<float>());
        }

        return result;
    }*/

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
