#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include "YamlParser.h"
#include "YamlConversions.h"
#include "World.h"
#include "Transformations.h"

using namespace Scene;
using namespace Math;
using namespace Rendering;
using namespace std;

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

// TODO: Tests needed, default sphere material/transform, sphere with defined materail, extended materail/transform

TEST(YamlTest, DecodeSphere)
{
    const string yaml = R"(
- define: standard-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]

- define: large-object
  value:
    - standard-transform
    - [ scale, 3.5, 3.5, 3.5 ]

- add: sphere
  material:
    color: [ 0.373, 0.404, 0.550 ]
    diffuse: 0.2
    ambient: 0.0
    specular: 1.0
    shininess: 200
    reflective: 0.7
    transparency: 0.7
    refractive-index: 1.5
  transform:
    - large-object
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->material, Material(Color(0.373, 0.404, 0.550), 0.0, 0.2, 1.0, 200, 0.7, 0.7, 1.5));
    EXPECT_EQ(sphere->transform, IdentityMatrix * Transformations::Translation(1, -1, 1) * Transformations::Scaling(0.5, 0.5, 0.5) * Transformations::Scaling(3.5, 3.5, 3.5));
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
