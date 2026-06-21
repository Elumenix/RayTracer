#pragma once

namespace std
{
    class string;
}

namespace Scene
{
    class World;
}

namespace YAML
{
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

        Scene::World ParseFile(const std::string &filename);

    private:
        YamlParser() {}
        ~YamlParser() = default;
    };
}