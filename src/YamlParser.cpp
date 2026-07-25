#include "YamlParser.h"
#include <yaml-cpp/yaml.h>
#include "YamlConversions.h"
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include "World.h"

using namespace Scene;
using namespace std;

namespace YAML
{
    bool CheckShapeFields(const Node &item, int itemNumber, const string shapeType)
    {
        if (!item["material"].IsDefined())
        {
            cerr << "Item " << itemNumber << ": " << shapeType << " must have a material key" << endl;
            throw runtime_error("Item " + to_string(itemNumber) + ": " + shapeType + " must have a material key");
        }

        if (!item["transform"].IsDefined())
        {
            cerr << "Item " << itemNumber << ": " << shapeType << " must have a transform key" << endl;
            throw runtime_error("Item " + to_string(itemNumber) + ": " + shapeType + " must have a transform key");
        }

        return true;
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
                throw runtime_error("Item " + to_string(itemNumber) + ": Transform not found: " + transformName);
            }

            if (!defines.at(transformName).IsSequence())
            {
                cerr << "Item " << itemNumber << ": " << transformName << " is not a transform" << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + transformName + " is not a transform");
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
                if (entry.IsScalar())
                {
                    // If it's a scalar and not in the map, the wrong name was used and we need to check it
                    string parentTransform = entry.as<string>();
                    if (!defines.count(parentTransform))
                    {
                        cerr << "Item " << itemNumber << ": Referenced transform was not defined: " << parentTransform << endl;
                        throw runtime_error("Item " + to_string(itemNumber) + ": Referenced transform was not defined: " + parentTransform);
                    }

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

    // If the pattern is in the map, we need to substitute it in
    Node ResolvePattern(const Node &node, const unordered_map<string, Node> &defines, int itemNumber)
    {
        if (node.IsScalar())
        {
            string patternName = node.as<string>();

            if (!defines.count(patternName))
            {
                cerr << "Item " << itemNumber << ": pattern not found: " << patternName << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": pattern not found: " + patternName);
            }

            if (!defines.at(patternName).IsMap() || !defines.at(patternName)["type"])
            {
                cerr << "Item " << itemNumber << ": " << patternName << " is not a pattern" << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + patternName + " is not a pattern");
            }

            return defines.at(patternName);
        }

        // Getting here means the pattern was defined inline on the shape rather than in a define, so it's partly unrolled already
        // If any colors field is a defined pattern, or the transform field is a defined transform, those will need to be unrolled
        Node colors = node["colors"];
        if (!node["colors"] || !((colors[0] && colors[0].IsScalar()) || (colors[1] && colors[1].IsScalar()) || (node["transform"] && node["transform"].IsScalar())))
            return node; // everything was already unrolled

        Node unroll = node;
        if (colors[0] && colors[0].IsScalar())
        {
            unroll["colors"][0] = ResolvePattern(colors[0], defines, itemNumber);
        }

        if (colors[1] && colors[1].IsScalar())
        {
            unroll["colors"][1] = ResolvePattern(colors[1], defines, itemNumber);
        }

        if (node["transform"] && node["transform"].IsScalar())
        {
            unroll["transform"] = ResolveTransform(node["transform"], defines, itemNumber);
        }

        return unroll;
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
                throw runtime_error("Item " + to_string(itemNumber) + ": Material not found: " + materialName);
            }

            if (!defines.at(materialName).IsMap())
            {
                cerr << "Item " << itemNumber << ": " << materialName << " is not a material" << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + materialName + " is not a material");
            }

            return defines.at(materialName);
        }

        // Getting down here means the material is defined on an object rather than using a define, so it is already unrolled
        // The exception this unrolling is if a define was used for a pattern that might be on the material, which will need to be added on
        if (node["pattern"])
        {
            Node unrolled = node;
            unrolled["pattern"] = ResolvePattern(node["pattern"], defines, itemNumber);
            return unrolled;
        }

        return node;
    }

    void CheckDefValidity(const Node &item, const unordered_map<string, Node> &defines, int itemNumber)
    {
        // Confirm this is a proper transform
        if (item["value"].IsSequence())
        {
            try
            {
                auto node = ResolveTransform(item["value"], defines, itemNumber);

                try
                {
                    auto ret = node.as<Math::Matrix<4, 4>>();
                }
                catch (const std::exception &e)
                {
                    // This error would be written out by YamlConversions, so we need to add item info
                    cerr << "Item " << itemNumber << ": " << e.what() << endl;
                    throw runtime_error("Item " + to_string(itemNumber) + ": " + e.what());
                }
            }
            catch (const std::exception &e)
            {
                // This was written out at the source already
                throw runtime_error(e.what());
            }
        }

        // Confirm material/pattern is set up correctly
        if (item["value"].IsMap())
        {
            try
            {
                Node val = item["value"];
                bool isMat = !val["type"];

                auto node = isMat ? ResolveMaterial(val, defines, itemNumber) : ResolvePattern(val, defines, itemNumber);

                try
                {
                    if (isMat)
                    {
                        auto ret = node.as<Rendering::Material>();
                    }
                    else
                    {
                        auto ret = node.as<Rendering::Pattern *>();
                    }
                }
                catch (const std::exception &e)
                {
                    // This error would be written out by YamlConversions, so we need to add item info
                    cerr << "Item " << itemNumber << ": " << e.what() << endl;
                    throw runtime_error("Item " + to_string(itemNumber) + ": " + e.what());
                }
            }
            catch (const std::exception &e)
            {
                // This was written out at the source already
                throw runtime_error(e.what());
            }
        }
    }

    void ExtendItem(const Node &item, const unordered_map<string, Node> &defines, int itemNumber, const string &name, Node &extendedNode)
    {
        string parentName = item["extend"].as<string>();

        // Make sure the original definition exists
        if (!defines.count(parentName))
        {
            cerr << "Item " << itemNumber << ": Extend failed because of undefined parent: " << parentName << endl;
            throw runtime_error("Item " + to_string(itemNumber) + ": Extend failed because of undefined parent: " + parentName);
        }

        // Merge the extend value with the existing definition
        extendedNode = Clone(defines.at(parentName));

        // Fill with values, then place on the map
        const Node &valueNode = item["value"];
        if (valueNode.IsMap()) // Material Map or Pattern
        {
            bool isMaterial = valueNode["type"] ? false : true;

            if (!isMaterial)
            {
                cerr << "Item " << itemNumber << ": The 'extend' keyword cannot be used on a defined pattern. To use a pattern as part of another pattern, please put the other pattern as one of the items in 'colors'" << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": The 'extend' keyword cannot be used on a defined pattern. To use a pattern as part of another pattern, please put the other pattern as one of the items in 'colors'");
            }

            // In case we tried to extend a transform instead of a material/pattern
            if (!extendedNode.IsMap())
            {
                string cur = isMaterial ? "(Material) " : "(Pattern) ";
                cerr << "Item " << itemNumber << ": " << cur << name << " cannot extend (Transform) " << parentName << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + cur + name + " cannot extend (Transform) " + parentName);
            }

            // In case we tried to mix up a material and a pattern
            if (isMaterial && extendedNode["type"])
            {
                string cur = isMaterial ? "(Material) " : "(Pattern) ";
                string other = !isMaterial ? "(Material) " : "(Pattern) ";

                cerr << "Item " << itemNumber << ": " << cur << name << " cannot extend " << other << parentName << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + cur + name + " cannot extend " + other + parentName);
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
            // In case we tried to extend a material/pattern instead of a transform
            if (!extendedNode.IsSequence())
            {
                string other = extendedNode["type"] ? "(Pattern) " : "(Material) ";
                cerr << "Item " << itemNumber << ": " << "(Transform) " << name << " cannot extend " << other << parentName << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + "(Transform) " + name + " cannot extend " + other + parentName);
            }

            for (const auto &entry : valueNode)
            {
                extendedNode.push_back(entry);
            }
        }
        else
        {
            cerr << "Item " << itemNumber << ": Unsupported value field type for extended definition: " << name << endl;
            throw runtime_error("Item " + to_string(itemNumber) + ": Unsupported value field type for extended definition: " + name);
        }
    }

    bool AddItem(const Node &item, const unordered_map<string, Node> &defines, int itemNumber, World &world, Camera &camera)
    {
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
                throw runtime_error("Item " + to_string(itemNumber) + ": " + e.what());
            }

            return false;
        }

        if (type == "sphere")
        {
            // We need to catch if the user missed a required field
            CheckShapeFields(item, itemNumber, "sphere");

            // We'll be creating a new node that turns into a sphere, since it's ambiguous whether the user
            // is going to manually define a material/object or use a variable for it
            Node expanded;
            expanded["material"] = ResolveMaterial(item["material"], defines, itemNumber);
            expanded["transform"] = ResolveTransform(item["transform"], defines, itemNumber);

            try
            {
                // Sphere gets decoded finally
                Sphere sphere = expanded.as<Sphere>();

                // The world takes ownership of the sphere
                world.Add(std::move(sphere));
            }
            catch (const std::exception &e)
            {
                cerr << "Item " << itemNumber << ": " << e.what() << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + e.what());
            }

            return false;
        }

        if (type == "cube")
        {
            // We need to catch if the user missed a required field
            CheckShapeFields(item, itemNumber, "cube");

            // We'll be creating a new node that turns into a cube, since it's ambiguous whether the user
            // is going to manually define a material/object or use a variable for it
            Node expanded;
            expanded["material"] = ResolveMaterial(item["material"], defines, itemNumber);
            expanded["transform"] = ResolveTransform(item["transform"], defines, itemNumber);

            try
            {
                // Cube gets decoded finally
                Cube cube = expanded.as<Cube>();

                // The world takes ownership of the cube
                world.Add(std::move(cube));
            }
            catch (const std::exception &e)
            {
                cerr << "Item " << itemNumber << ": " << e.what() << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + e.what());
            }

            return false;
        }

        if (type == "plane")
        {
            // We need to catch if the user missed a required field
            CheckShapeFields(item, itemNumber, "plane");

            // We'll be creating a new node that turns into a plane, since it's ambiguous whether the user
            // is going to manually define a material/object or use a variable for it
            Node expanded;
            expanded["material"] = ResolveMaterial(item["material"], defines, itemNumber);
            expanded["transform"] = ResolveTransform(item["transform"], defines, itemNumber);

            try
            {
                // Plane gets decoded finally
                Plane plane = expanded.as<Plane>();

                // The world takes ownership of the plane
                world.Add(std::move(plane));
            }
            catch (const std::exception &e)
            {
                cerr << "Item " << itemNumber << ": " << e.what() << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + e.what());
            }

            return false;
        }

        if (type == "camera")
        {
            try
            {
                Camera c = item.as<Camera>();
                camera = std::move(item.as<Camera>());
            }
            catch (const std::exception &e)
            {
                cerr << "Item " << itemNumber << ": " << e.what() << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": " + e.what());
            }

            return true;
        }

        // Getting here means the item is not a valid type
        cerr << "Item " << itemNumber << ": Invalid add key: " << type << endl;
        throw runtime_error("Item " + to_string(itemNumber) + ": Invalid add key: " + type);
    }
}

tuple<Camera, World, int> YamlParser::ParseFile(const string &filename)
{
    YAML::Node root = YAML::LoadFile(filename);
    return ParseYaml(root);
}

tuple<Camera, World, int> YamlParser::ParseYaml(const string &yaml)
{
    YAML::Node root = YAML::Load(yaml);
    return ParseYaml(root);
}

tuple<Camera, World, int> YamlParser::ParseYaml(const YAML::Node &root)
{
    // Cleaning up memory before starting
    YAML::convert<Rendering::Pattern *>::ClearAll();

    World world = World();
    unordered_map<string, YAML::Node> defines; // Material and transform definitions
    Camera camera;
    bool cameraSet = false;
    int itemNumber = 0;
    int maxDepth = 5; // default, may be overriden if the camera contains an override

    for (const auto &item : root)
    {
        itemNumber++;

        // Defines will be put in a map so they can be used later
        if (item["define"])
        {
            string name = item["define"].as<string>();

            // value should be a required field, even if it's empty
            if (!item["value"])
            {
                cerr << "Item " << itemNumber << ": No value field on extended definition: " << name << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": No value field on extended definition: " + name);
            }

            // First we should make sure that the 'value' field has properly formatted values
            // If not, there's no point in building out a new node, so an error will be emitted and handled in CheckDefValidity
            CheckDefValidity(item, defines, itemNumber);

            // If the item has an extend, we are making a new material or transform that builds on top of one in the map
            if (item["extend"])
            {
                // We'll build the new node
                YAML::Node extendedNode;
                ExtendItem(item, defines, itemNumber, name, extendedNode);

                // Extend node is saved
                defines[name] = extendedNode;
                continue;
            }

            // Defined node is saved
            defines[name] = item["value"];
            continue;
        }

        // If not an extend/define, we are adding an item, which the AddItem method handles
        // AddItem returns true only if a camera was just added, as we need to track the amount of cameras in the scene
        if (AddItem(item, defines, itemNumber, world, camera))
        {
            // Report a problem if an extra camera was added
            if (cameraSet)
            {
                cerr << "Item " << itemNumber << ": Only one camera can be defined" << endl;
                throw runtime_error("Item " + to_string(itemNumber) + ": Only one camera can be defined");
            }

            // Mark that we added a camera
            cameraSet = true;

            // We know that this is the camera, so we can check depth
            if (item["reflection-depth"])
            {
                try
                {
                    maxDepth = item["reflection-depth"].as<int>();
                    if (maxDepth < 0)
                    {
                        throw runtime_error("");
                    }
                }
                catch (const std::exception &e)
                {
                    cerr << "Item " << itemNumber << ": 'reflection-depth' field on the camera must be an integer >= 0" << endl;
                    throw runtime_error("Item " + to_string(itemNumber) + ": 'reflection-depth' field on the camera must be an integer >= 0");
                }
            }
        }
    }

    // Confirm the scene has exactly 1 camera before returning
    if (!cameraSet)
    {
        cerr << "A camera must be defined in the scene" << endl;
        throw runtime_error("A camera must be defined in the scene");
    }

    // move is required on world because of the smart pointers it contains.
    return {std::move(camera), std::move(world), maxDepth};
}
