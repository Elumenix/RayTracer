#include "YamlParser.h"
#include <yaml-cpp/yaml.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include "World.h"
#include "YamlConversions.h"
#include "Light.h"
#include "Sphere.h"

using namespace Scene;
using namespace std;

namespace YAML
{
    // If the material is in the map, we need to substitute it in
    Node resolveMaterial(const Node &node, const unordered_map<string, Node> &defines)
    {
        if (node.IsScalar())
        {
            string materialName = node.as<string>();

            if (!defines.count(materialName))
            {
                cerr << "Material not found: " << materialName << endl;
                throw runtime_error("Material not found: " + materialName);
            }

            return defines.at(materialName);
        }

        return node;
    }

    // If the transform is in the map, we need to substitute it in
    Node resolveTransform(const Node &node, const unordered_map<string, Node> &defines)
    {
        if (node.IsScalar())
        {
            string transformName = node.as<string>();

            if (!defines.count(transformName))
            {
                cerr << "Transform not found: " << transformName << endl;
                throw runtime_error("Transform not found: " + transformName);
            }

            return resolveTransform(defines.at(transformName), defines);
        }

        if (node.IsSequence())
        {
            Node result;
            for (const auto &entry : node)
            {
                if (entry.IsScalar() && defines.count(entry.as<string>()))
                {
                    Node expanded = resolveTransform(defines.at(entry.as<string>()), defines);
                    for (const auto &subEntry : expanded)
                    {
                        result.push_back(subEntry);
                    }
                }
                else
                {
                    result.push_back(entry);
                }
            }
            return result;
        }

        return node;
    }
}

World YamlParser::ParseFile(const string &filename)
{
    YAML::Node root = YAML::LoadFile(filename);
    return ParseYaml(root);
}

World YamlParser::ParseYaml(const string &yaml)
{
    YAML::Node root = YAML::Load(yaml);
    return ParseYaml(root);
}

World YamlParser::ParseYaml(const YAML::Node &root)
{
    World world = World();
    unordered_map<string, YAML::Node> defines; // Material and transform definitions

    // TODO: Camera is also a part of this file. I need to figure out how to also return it

    for (const auto &item : root)
    {
        // Defines will be put in a map so they can be used later
        if (item["define"])
        {
            string name = item["define"].as<string>();

            // If the item has an extend, we are making a new material or transform that builds on top of one in the map
            if (item["extend"])
            {
                string extendName = item["extend"].as<string>();

                // Make sure the original definition exists
                if (defines.count(extendName))
                {
                    // Merge the extend value with the existing definition
                    YAML::Node extendedNode = Clone(defines.at(extendName));

                    // value should be a required field, even if it's empty
                    if (!item["value"])
                    {
                        cerr << "No value provided for extended definition: " << name << endl;
                        throw runtime_error("No value provided for extended definition: " + name);
                    }

                    // Fill with values, then place on the map
                    for (const auto &pair : item["value"])
                    {
                        extendedNode[pair.first.as<string>()] = pair.second;
                    }

                    defines[name] = extendedNode;
                }
                else
                {
                    cerr << "Extended definition not found: " << extendName << endl;
                    throw runtime_error("Extended definition not found: " + extendName);
                }

                continue;
            }

            defines[name] = item["value"];
            continue;
        }

        // Item Type
        string type = item["add"].as<string>();

        if (type == "light")
        {
            Light light = item.as<Light>();
            world.Add(light);
        }

        if (type == "sphere")
        {
            // We'll be creating a new node that turns into a sphere, since it's ambiguous whether the user
            // is going to manually define a material/object or use a variable for it
            YAML::Node expanded;
            expanded["material"] = YAML::resolveMaterial(item["material"], defines);
            expanded["transform"] = YAML::resolveTransform(item["transform"], defines);

            Sphere sphere = expanded.as<Sphere>();

            // The world takes ownership of the sphere
            world.Add(move(sphere));
        }
    }

    return world;
}
