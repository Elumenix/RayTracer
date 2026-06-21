#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include "YamlConversions.h"

using namespace Scene;
using namespace Math;
using namespace Rendering;

TEST(YamlTest, FileFound)
{
    YAML::Node testfile = YAML::LoadFile("tests/YamlReadTest.yaml");
    EXPECT_TRUE(testfile.IsDefined());
}

TEST(YamlTest, EncodeLight)
{
    Light light = Light(Point(50, 100, -50), Color(1, 1, 1));
    YAML::Node node = YAML::convert<Light>::encode(light);
    EXPECT_EQ(node["at"].as<Point>(), light.position);
    EXPECT_EQ(node["intensity"].as<Color>(), light.intensity);
}

TEST(YamlTest, DecodeLight)
{
    YAML::Node node;
    node["at"] = Point(50, 100, -50);
    node["intensity"] = Color(1, 1, 1);

    Light light;
    EXPECT_TRUE(YAML::convert<Light>::decode(node, light));
    EXPECT_EQ(light.position, Point(50, 100, -50));
    EXPECT_EQ(light.intensity, Color(1, 1, 1));
}

/*TEST(yamlTest, CreateCamera)
{
    // This class has 3 variables that need to be set, none of which has default values
    // Additionally, it has optional variables to change the from/to/up direction of the camera
    YAML::Node root = YAML::LoadFile("tests/YamlTestFile1.yaml");

    Camera camera = root[0].as<Camera>();
    Camera camera1 = root[1].as<Camera>();

    EXPECT_EQ(Vector(-0.805731, -0.143256, -0.574696), Vector(camera.transform[0][0], camera.transform[1][0], camera.transform[2][0]));
    EXPECT_EQ(Vector(-0.362579, 0.886548, 0.287348), Vector(camera.transform[0][1], camera.transform[1][1], camera.transform[2][1]));
    EXPECT_EQ(Vector(0.468331, 0.439898, -0.766261), Vector(camera.transform[0][2], camera.transform[1][2], camera.transform[2][2]));

    EXPECT_EQ(camera1.hsize, 340);
    EXPECT_EQ(camera1.vsize, 120);
    EXPECT_FLOAT_EQ(camera1.fov, 0.485);
    EXPECT_EQ(Vector(1, 0, 0), Vector(camera1.transform[0][0], camera1.transform[1][0], camera1.transform[2][0]));
    EXPECT_EQ(Vector(0, 1, 0), Vector(camera1.transform[0][1], camera1.transform[1][1], camera1.transform[2][1]));
    EXPECT_EQ(Vector(0, 0, 1), Vector(camera1.transform[0][2], camera1.transform[1][2], camera1.transform[2][2]));
}*/
