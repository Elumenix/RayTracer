#include <yaml-cpp/node/convert.h>
#include "Camera.h"
#include "Matrix.h"
#include "Transformations.h"

namespace YAML
{
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
    }

    template <>
    struct convert<Scene::Camera>
    {
        static Node encode(const Scene::Camera &rhs)
        {
            Node node;
            node["hsize"] = rhs.hsize;
            node["vsize"] = rhs.vsize;
            node["fov"] = rhs.fov;
            return node;
        }

        static bool decode(const Node &node, Scene::Camera &rhs)
        {
            if (!node["hsize"] || !node["vsize"] || !node["fov"])
                return false;

            rhs = Scene::Camera(
                node["hsize"].as<int>(),
                node["vsize"].as<int>(),
                node["fov"].as<float>());

            if (node["from"] && node["to"] && node["up"])
            {
                Math::Vector from = node["from"].as<Math::Vector>();
                Math::Vector to = node["to"].as<Math::Vector>();
                Math::Vector up = node["up"].as<Math::Vector>();

                rhs.transform = Transformations::ViewTransform(from, to, up);
            }

            return true;
        }
    };

}
