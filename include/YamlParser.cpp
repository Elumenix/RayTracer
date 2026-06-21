#include "YamlParser.h"
#include <yaml-cpp/yaml.h>
#include <string>
#include "World.h"
#include "YamlConversions.h"
#include "Light.h";

using namespace Scene;
using namespace std;

namespace YAML
{
    World YamlParser::ParseFile(const string &filename)
    {
        World world = World();
        Node root = LoadFile(filename);
        // TODO: Camera is also a part of this file. I need to figure out how to also return it

        for (const auto &item : root)
        {
            // TODO: I still need to work on groups so everything should be add currently
            if (!item["add"])
            {
                continue;
            }

            string type = item["add"].as<string>();
            if (type == "light")
            {
                Light light = item.as<Light>();
                world.Add(light);
            }
        }

        return world;
    }
}
