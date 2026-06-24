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
    const string yaml = R"(
- add: light
  at: [50, 100, -50]
  intensity: [1, 1, 1]
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Light *light = &world.lights[0];

    EXPECT_EQ(light->position, Point(50, 100, -50));
    EXPECT_EQ(light->intensity, Color(1, 1, 1));
}

TEST(YamlTest, BadLight1)
{
    const string yaml = R"(
- add: light
  at: 
  intensity: 
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadLight2)
{
    const string yaml = R"(
- add: light
  at: [50, 100, -50]
  intenity: [30, 40, 50]
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadLight3)
{
    const string yaml = R"(
- add: light
  at: 3
  intensity: [30, 40, 50]
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadLight4)
{
    const string yaml = R"(
- add: light
  at: [50, 100, -50]
  intensity: [30, 40, 50]
  style: 3
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, DecodeDirectMaterial)
{
    const string yaml = R"(
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
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->material, Material(Color(0.373, 0.404, 0.550), 0.0, 0.2, 1.0, 200, 0.7, 0.7, 1.5));
}

TEST(YamlTest, DecodeDefinedMaterial)
{
    const string yaml = R"(
- define: my-material
  value:
    color: [ 0.373, 0.404, 0.550 ]
    diffuse: 0.2
    ambient: 0.0
    specular: 1.0
    shininess: 200
    reflective: 0.7
    transparency: 0.7
    refractive-index: 1.5

- add: sphere
  material: my-material
  transform:
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->material, Material(Color(0.373, 0.404, 0.550), 0.0, 0.2, 1.0, 200, 0.7, 0.7, 1.5));
}

TEST(YamlTest, DecodeExtendedMaterial)
{
    const string yaml = R"(
- define: base-material
  value:
    color: [ 0.373, 0.404, 0.550 ]
    diffuse: 0.2
    ambient: 0.0
    specular: 1.0
    shininess: 200
    reflective: 0.7
    transparency: 0.7
    refractive-index: 1.5

- define: extended-material
  extend: base-material
  value:
    color: [ 1, 1, 1 ]
    diffuse: 0.5

- add: sphere
  material: extended-material
  transform:
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->material, Material(Color(1, 1, 1), 0.0, 0.5, 1.0, 200, 0.7, 0.7, 1.5));
}

TEST(YamlTest, DecodeDirectTransform)
{
    const string yaml = R"(
- add: sphere
  material:
  transform:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->transform, IdentityMatrix * Transformations::Translation(1, -1, 1) * Transformations::Scaling(0.5, 0.5, 0.5));
}

TEST(YamlTest, DecodeDefinedTransform)
{
    const string yaml = R"(
- define: my-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]

- add: sphere
  material:
  transform: my-transform
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->transform, IdentityMatrix * Transformations::Translation(1, -1, 1) * Transformations::Scaling(0.5, 0.5, 0.5));
}

TEST(YamlTest, DecodeExtendedTransform)
{
    const string yaml = R"(
- define: base-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]

- define: extended-transform
  extend: base-transform
  value:
    - [ scale, 2, 2, 2 ]

- add: sphere
  material:
  transform: extended-transform
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->transform, IdentityMatrix * Transformations::Translation(1, -1, 1) * Transformations::Scaling(0.5, 0.5, 0.5) * Transformations::Scaling(2, 2, 2));
}

TEST(YamlTest, DecodeDefinedTransformTree)
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

- define: extra-large-object
  value:
    - large-object
    - [ scale, 2, 2, 2 ]

- add: sphere
  material:
  transform:
    - extra-large-object
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->transform, IdentityMatrix * Transformations::Translation(1, -1, 1) * Transformations::Scaling(0.5, 0.5, 0.5) * Transformations::Scaling(3.5, 3.5, 3.5) * Transformations::Scaling(2, 2, 2));
}

TEST(YamlTest, DecodeDefinedTransformTreeWithExtendedTransform)
{
    const string yaml = R"(
- define: base-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.25, 0.25, 0.25 ]

- define: extended-transform
  extend: base-transform
  value:
    - [ scale, 2, 2, 2 ]

- define: large-object
  value:
    - extended-transform
    - [ scale, 3.5, 3.5, 3.5 ]

- define: extra-large-object
  value:
    - large-object
    - [ scale, 2.1, 2.1, 2.1 ]

- add: sphere
  material:
  transform:
    - extra-large-object
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->transform, IdentityMatrix * Transformations::Translation(1, -1, 1) * Transformations::Scaling(0.25, 0.25, 0.25) * Transformations::Scaling(2, 2, 2) * Transformations::Scaling(3.5, 3.5, 3.5) * Transformations::Scaling(2.1, 2.1, 2.1));
}

TEST(YamlTest, FailedMaterialExtend)
{
    const string yaml = R"(
- define: base-material
  value:
    color: [ 0.373, 0.404, 0.550 ]
    diffuse: 0.2
    ambient: 0.0
    specular: 1.0
    shininess: 200
    reflective: 0.7
    transparency: 0.7
    refractive-index: 1.5

- define: extended-material
  extend: bae-material
  value:
    color: [ 1, 1, 1 ]
    diffuse: 0.5
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, FailedTransformExtend)
{
    const string yaml = R"(
- define: base-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]

- define: extended-transform
  extend: base-transorm
  value:
    - [ scale, 2, 2, 2 ]
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, ExtendedWrongType1)
{
    const string yaml = R"(
- define: base-material
  value:
    color: [ 0.373, 0.404, 0.550 ]
    diffuse: 0.2
    ambient: 0.0
    specular: 1.0
    shininess: 200
    reflective: 0.7
    transparency: 0.7
    refractive-index: 1.5

- define: extended-transform
  extend: base-material
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, ExtendedWrongType2)
{
    const string yaml = R"(
- define: base-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]

- define: extended-material
  extend: base-transform
  value:
    color: [ 1, 1, 1 ]
    diffuse: 0.5
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, CatchMissingShapeTags)
{
    const string yaml = R"(
- add: sphere
  transform:
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);

    const string yaml1 = R"(
- add: sphere
  material:
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml1), std::runtime_error);
}

TEST(YamlTest, DecodeDefaultSphere)
{
    const string yaml = R"(
- add: sphere
  material:
  transform:
)";

    World world = YamlParser::getInstance().ParseYaml(yaml);
    Shape *sphere = world.shapes[0].get();

    EXPECT_EQ(sphere->material, Material());
    EXPECT_EQ(sphere->transform, IdentityMatrix);
}

TEST(YamlTest, BadShapeMaterial1)
{
    const string yaml = R"(
- define: base-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]    

- add: sphere
  material: base-transform
  transform:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadShapeMaterial2)
{
    const string yaml = R"(
- define: base-material
  value:
    color: [ 0.373, 0.404, 0.550 ]
    diffuse: 0.2
    ambient: 0.0
    specular: 1.0
    shininess: [ 200, 200, 150 ]
    reflective: 0.7
    transparency: 0.7
    refractive-index: 1.5

- add: sphere
  material: base-material
  transform:
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadShapeMaterial3)
{
    const string yaml = R"(
- define: base-material
  value:
    color: [ 0.373, 0.404, 0.550 ]
    diffue: 0.2
    refractive-index: 1.5

- add: sphere
  material: base-material
  transform:
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadShapeTransform1)
{
    const string yaml = R"(
- define: base-material
  value:
    color: [ 0.373, 0.404, 0.550 ]
    diffuse: 0.2
    ambient: 0.0
    specular: 1.0
    shininess: [ 200, 200, 150 ]
    reflective: 0.7
    transparency: 0.7
    refractive-index: 1.5

- add: sphere
  material:
  transform: base-material
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadShapeTransform2)
{
    const string yaml = R"(
- define: base-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ 1, 0.5, 0.5, 0.5 ]    

- add: sphere
  material:
  transform: base-transform
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadShapeTransform3)
{
    const string yaml = R"(
- define: base-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5 ]    

- add: sphere
  material:
  transform: base-transform
)";

    EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
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
