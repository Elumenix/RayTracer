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
    bool CheckShapeFields(const Node &item, int itemNumber, const string shapeType)
    {
        if (!item["material"].IsDefined())
        {
            cerr << "Item " << itemNumber << ": " << shapeType << " must have a material tag" << endl;
            throw runtime_error(shapeType + " must have a material tag");
        }

        if (!item["transform"].IsDefined())
        {
            cerr << "Item " << itemNumber << ": " << shapeType << " must have a transform tag" << endl;
            throw runtime_error(shapeType + " must have a transform tag");
        }

        return true;
    }

    // If the material is in the map, we need to substitute it in
    Node ResolveMaterial(const Node &node, const unordered_map<string, Node> &defines, int itemNumber)
    {
        if (node.IsScalar())
        {
            string materialName = node.as<string>();

            if (!defines.count(materialName))
            {
                cerr << "Item " << itemNumber << ": Material not found: " << materialName << endl;
                throw runtime_error("Material not found: " + materialName);
            }

            if (!defines.at(materialName).IsMap())
            {
                cerr << "Item " << itemNumber << ": " << materialName << " is not a material" << endl;
                throw runtime_error(materialName + " is not a material");
            }

            return defines.at(materialName);
        }

        // Material was already unrolled or empty, so we can just return it
        return node;
    }

    // If the transform is in the map, we need to substitute it in
    Node ResolveTransform(const Node &node, const unordered_map<string, Node> &defines, int itemNumber)
    {
        // Another defined transform is being used, we need to resolve it
        if (node.IsScalar())
        {
            string transformName = node.as<string>();

            if (!defines.count(transformName))
            {
                cerr << "Item " << itemNumber << ": Transform not found: " << transformName << endl;
                throw runtime_error("Transform not found: " + transformName);
            }

            if (!defines.at(transformName).IsSequence())
            {
                cerr << "Item " << itemNumber << ": " << transformName << " is not a transform" << endl;
                throw runtime_error(transformName + " is not a transform");
            }

            return ResolveTransform(defines.at(transformName), defines, itemNumber);
        }

        // If the transform is a sequence, we need to expand each entry
        if (node.IsSequence())
        {
            Node result;
            for (const auto &entry : node)
            {
                // If the entry is a scalar and exists in the defines map, we need to resolve it
                if (entry.IsScalar() && defines.count(entry.as<string>()))
                {
                    Node expanded = ResolveTransform(defines.at(entry.as<string>()), defines, itemNumber);
                    for (const auto &subEntry : expanded)
                    {
                        result.push_back(subEntry);
                    }
                }
                else
                {
                    // This is just a transform entry, so we can add it directly
                    result.push_back(entry);
                }
            }

            // Return the fully unrolled sequence of transformations
            return result;
        }

        // This return statement means that the node was empty
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

    int itemNumber = 0;
    for (const auto &item : root)
    {
        itemNumber++;

        // Defines will be put in a map so they can be used later
        if (item["define"])
        {
            string name = item["define"].as<string>();

            // If the item has an extend, we are making a new material or transform that builds on top of one in the map
            if (item["extend"])
            {
                string parentName = item["extend"].as<string>();

                // Make sure the original definition exists
                if (!defines.count(parentName))
                {
                    cerr << "Item " << itemNumber << ": Extend failed because of undefined parent: " << parentName << endl;
                    throw runtime_error("Extend failed because of undefined parent: " + parentName);
                }

                // Merge the extend value with the existing definition
                YAML::Node extendedNode = Clone(defines.at(parentName));

                // value should be a required field, even if it's empty
                if (!item["value"])
                {
                    cerr << "Item " << itemNumber << ": No value field on extended definition: " << name << endl;
                    throw runtime_error("No value field on extended definition: " + name);
                }

                // Fill with values, then place on the map
                const YAML::Node &valueNode = item["value"];
                if (valueNode.IsMap()) // Material Map
                {
                    // In case we tried to extend a transform instead of a material
                    if (!extendedNode.IsMap())
                    {
                        cerr << "Item " << itemNumber << ": " << "(Material) " << name << " cannot extend (Transform) " << parentName << endl;
                        throw runtime_error(parentName + " is not the same type as " + name);
                    }

                    for (const auto &pair : valueNode)
                    {
                        string key = pair.first.as<string>();

                        // If this is an original field, we'll update it
                        if (extendedNode[key].IsDefined())
                        {
                            extendedNode[key] = pair.second;
                            continue;
                        }

                        // If this is a new field, we'll add it
                        extendedNode.push_back(pair);
                    }
                }
                else if (valueNode.IsSequence()) // Transform Sequence
                {
                    // In case we tried to extend a material instead of a transform
                    if (!extendedNode.IsSequence())
                    {
                        cerr << "Item " << itemNumber << ": " << "(Transform) " << name << " cannot extend (Material) " << parentName << endl;
                        throw runtime_error(parentName + " is not the same type as " + name);
                    }

                    for (const auto &entry : valueNode)
                    {
                        extendedNode.push_back(entry);
                    }
                }
                else
                {
                    cerr << "Item " << itemNumber << ": Unsupported value field type for extended definition: " << name << endl;
                    throw runtime_error("Unsupported value field type for extended definition: " + name);
                }

                // Extend node is saved
                defines[name] = extendedNode;
                continue;
            }

            // value should be a required field, even if it's empty
            if (!item["value"])
            {
                cerr << "Item " << itemNumber << ": No value field on extended definition: " << name << endl;
                throw runtime_error("No value field on extended definition: " + name);
            }

            // Defined node is saved
            defines[name] = item["value"];
            continue;
        }

        // Item Type
        string type = item["add"].as<string>();

        if (type == "light")
        {
            try
            {
                Light light = item.as<Light>();
                world.Add(light);
            }
            catch (const std::exception &e)
            {
                cerr << "Item " << itemNumber << ": " << e.what() << endl;
                throw;
            }
        }

        if (type == "sphere")
        {
            // We need to catch if the user missed a required field
            if (!CheckShapeFields(item, itemNumber, "Sphere"))
            {
                continue;
            }

            // We'll be creating a new node that turns into a sphere, since it's ambiguous whether the user
            // is going to manually define a material/object or use a variable for it
            YAML::Node expanded;
            expanded["material"] = YAML::ResolveMaterial(item["material"], defines, itemNumber);
            expanded["transform"] = YAML::ResolveTransform(item["transform"], defines, itemNumber);

            try
            {
                // Sphere gets decoded finally
                Sphere sphere = expanded.as<Sphere>();

                // The world takes ownership of the sphere
                world.Add(move(sphere));
            }
            catch (const std::exception &e)
            {
                cerr << "Item " << itemNumber << ": " << e.what() << endl;
                throw;
            }
        }
    }

    return world;
}
