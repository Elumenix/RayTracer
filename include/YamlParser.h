#pragma once
#include <string>

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

    std::pair<Scene::Camera, Scene::World> ParseFile(const std::string &filename);
    std::pair<Scene::Camera, Scene::World> ParseYaml(const std::string &yaml);
    std::pair<Scene::Camera, Scene::World> ParseYaml(const YAML::Node &root);

private:
    YamlParser() {}
    ~YamlParser() = default;
};