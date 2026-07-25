#pragma once
#include <string>
#include <tuple>

namespace Scene
{
    class Camera;
    class World;
}

namespace YAML
{
    class Node;
}

class YamlParser
{
public:
    // Prevent any copying or moving of the parser
    YamlParser(const YamlParser &) = delete;
    YamlParser &operator=(const YamlParser &) = delete;
    YamlParser(YamlParser &&) = delete;
    YamlParser &operator=(YamlParser &&) = delete;

    static YamlParser &getInstance()
    {
        static YamlParser instance;
        return instance;
    }

    std::tuple<Scene::Camera, Scene::World, int> ParseFile(const std::string &filename);
    std::tuple<Scene::Camera, Scene::World, int> ParseYaml(const std::string &yaml);
    std::tuple<Scene::Camera, Scene::World, int> ParseYaml(const YAML::Node &root);

private:
    YamlParser() {}
    ~YamlParser() = default;
};