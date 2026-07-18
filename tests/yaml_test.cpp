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

// This is the first test because a camera is required for the scene
TEST(YamlTest, MinimalCamera)
{
  const string yaml = R"(
- add: camera
  width: 128
  height: 128
  field-of-view: 1.0471975511965976
)";

  pair<Camera, World> result = YamlParser::getInstance().ParseYaml(yaml);
  Camera camera = result.first;

  EXPECT_EQ(camera.hsize, 128);
  EXPECT_EQ(camera.vsize, 128);
  EXPECT_FLOAT_EQ(camera.fov, 1.0471975511965976);
  EXPECT_EQ(camera.transform, IdentityMatrix /*Transformations::ViewTransform(Point(0, 0, 0), Point(1, 1, 1), Vector(0, 1, 0))*/);
}

TEST(YamlTest, SceneMissingCamera)
{
  const string yaml = R"(
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadCamera1)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1.3
  field-of-view: 90 
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadCamera2)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: [30, 2] 
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, FullCamera)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 
  from: [0, 0, 0]
  to: [1, 1, 1]
  up: [0, 1, 0]
)";

  Camera camera = YamlParser::getInstance().ParseYaml(yaml).first;
  EXPECT_EQ(camera.transform, Transformations::ViewTransform(Point(0, 0, 0), Point(1, 1, 1), Vector(0, 1, 0)));
}

TEST(YamlTest, BadCamera3)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 
  from: [0, 0, 0]
  to: [0, 1, 0]
  up: [0, 1, 0]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadCamera4)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 
  from: [0, 0, 0]
  to: [1, 1, 1]
  up: [0, 1]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadCamera5)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 
  from: [0, 0, 0]
  to: [1, 1, 1]
  up: [0, 1, 0]
  back: [0, 0, 1]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
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
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- add: light
  at: [50, 100, -50]
  intensity: [1, 1, 1]
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Light *light = &world.lights[0];

  EXPECT_EQ(light->position, Point(50, 100, -50));
  EXPECT_EQ(light->intensity, Color(1, 1, 1));
}

TEST(YamlTest, BadLight1)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- add: light
  at: 
  intensity: 
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadLight2)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- add: light
  at: [50, 100, -50]
  intenity: [30, 40, 50]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadLight3)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- add: light
  at: 3
  intensity: [30, 40, 50]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, BadLight4)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- add: light
  at: [50, 100, -50]
  intensity: [30, 40, 50]
  style: 3
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, InvalidAddKey)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- add: Rhombus
  material: 
  transform: 
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, DecodeDirectMaterial)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

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

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->material, Material(Color(0.373, 0.404, 0.550), 0.0, 0.2, 1.0, 200, 0.7, 0.7, 1.5));
}

TEST(YamlTest, DecodeDefinedMaterial)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

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

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->material, Material(Color(0.373, 0.404, 0.550), 0.0, 0.2, 1.0, 200, 0.7, 0.7, 1.5));
}

TEST(YamlTest, DecodeExtendedMaterial)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

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

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->material, Material(Color(1, 1, 1), 0.0, 0.5, 1.0, 200, 0.7, 0.7, 1.5));
}

TEST(YamlTest, DecodeDirectPattern)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- add: plane
  transform:
  material:
    pattern:
      type: checkers
      colors:
        - [ 0.15, 0.15, 0.15 ]
        - [ 0.85, 0.85, 0.85 ]
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *plane = world.shapes[0].get();

  EXPECT_FALSE(dynamic_cast<StripePattern *>(plane->material.pattern.get()));
  EXPECT_TRUE(dynamic_cast<Checker *>(plane->material.pattern.get()));

  SolidColor s1(Color(0.15, 0.15, 0.15));
  SolidColor s2(Color(0.85, 0.85, 0.85));
  Checker c = Checker(&s1, &s2);

  // Make sure that the pattern is correct as it travels, and they don't just return the same color all the time
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(0, 0, 0)));
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0)));
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(1, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0)));
}

TEST(YamlTest, DecodeDefinedPattern)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- define: my-pattern
  value:
    type: checkers
    colors:
      - [ 0.15, 0.15, 0.15 ]
      - [ 0.85, 0.85, 0.85 ]

- add: plane
  transform:
  material:
    pattern: my-pattern
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *plane = world.shapes[0].get();

  EXPECT_FALSE(dynamic_cast<StripePattern *>(plane->material.pattern.get()));
  EXPECT_TRUE(dynamic_cast<Checker *>(plane->material.pattern.get()));

  SolidColor s1(Color(0.15, 0.15, 0.15));
  SolidColor s2(Color(0.85, 0.85, 0.85));
  Checker c = Checker(&s1, &s2);

  // Make sure that the pattern is correct as it travels, and they don't just return the same color all the time
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(0, 0, 0)));
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0)));
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(1, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0)));
}

TEST(YamlTest, DecodeDefinedMaterialPattern)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- define: my-material
  value:
    pattern:
      type: checkers
      colors:
        - [ 0.15, 0.15, 0.15 ]
        - [ 0.85, 0.85, 0.85 ]
    ambient: 0.8
    diffuse: 0.2
    specular: 0

- add: plane
  transform:
  material: my-material
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *plane = world.shapes[0].get();

  EXPECT_FALSE(dynamic_cast<StripePattern *>(plane->material.pattern.get()));
  EXPECT_TRUE(dynamic_cast<Checker *>(plane->material.pattern.get()));

  SolidColor s1(Color(0.15, 0.15, 0.15));
  SolidColor s2(Color(0.85, 0.85, 0.85));
  Checker c = Checker(&s1, &s2);

  // Make sure that the pattern is correct as it travels, and they don't just return the same color all the time
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(0, 0, 0)));
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0)));
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(1, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0)));
  EXPECT_FLOAT_EQ(plane->material.diffuse, 0.2);
}

TEST(YamlTest, DecodeDefinedPatternTree1)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- define: stripe-pattern
  value:
    type: stripes
    colors:
      - [ 0.15, 0.15, 0.15 ]
      - [ 0.85, 0.85, 0.85 ]

- define: check
  value:
    type: checkers
    colors:
      - stripe-pattern
      - [ 0.4, 0.4, 0.4 ]

- add: plane
  transform:
  material:
    pattern: check
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *plane = world.shapes[0].get();

  EXPECT_FALSE(dynamic_cast<StripePattern *>(plane->material.pattern.get()));
  EXPECT_TRUE(dynamic_cast<Checker *>(plane->material.pattern.get()));

  SolidColor s1(Color(0.15, 0.15, 0.15));
  SolidColor s2(Color(0.85, 0.85, 0.85));
  SolidColor s3(Color(.4, .4, .4));
  StripePattern stripe = StripePattern(&s1, &s2);
  Checker c = Checker(&stripe, &s3);

  // Make sure that the pattern is correct as it travels, and they don't just return the same color all the time
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(0, 0, 0))); // .15 == .15
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0))); // .15 != .4
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(1, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0))); // .4 == .4
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(1, 1, 0)), c.SampleAt(*plane, Point(1, 0, 0))); // .85 != .4
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(1, 1, 0)), c.SampleAt(*plane, Point(0, 0, 0))); // .85 != .15
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(1, 1, 0)), c.SampleAt(*plane, Point(1, 1, 0))); // .85 == .85
}

TEST(YamlTest, DecodeDefinedPatternTree2)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- define: stripe-pattern
  value:
    type: stripes
    colors:
      - [ 0.15, 0.15, 0.15 ]
      - [ 0.85, 0.85, 0.85 ]

- add: plane
  transform:
  material:
    pattern:
      type: checkers
      colors:
        - stripe-pattern
        - [ 0.4, 0.4, 0.4 ]
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *plane = world.shapes[0].get();

  EXPECT_FALSE(dynamic_cast<StripePattern *>(plane->material.pattern.get()));
  EXPECT_TRUE(dynamic_cast<Checker *>(plane->material.pattern.get()));

  SolidColor s1(Color(0.15, 0.15, 0.15));
  SolidColor s2(Color(0.85, 0.85, 0.85));
  SolidColor s3(Color(.4, .4, .4));
  StripePattern stripe = StripePattern(&s1, &s2);
  Checker c = Checker(&stripe, &s3);

  // Make sure that the pattern is correct as it travels, and they don't just return the same color all the time
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(0, 0, 0))); // .15 == .15
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(0, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0))); // .15 != .4
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(1, 0, 0)), c.SampleAt(*plane, Point(1, 0, 0))); // .4 == .4
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(1, 1, 0)), c.SampleAt(*plane, Point(1, 0, 0))); // .85 != .4
  EXPECT_NE(plane->material.pattern.get()->SampleAt(*plane, Point(1, 1, 0)), c.SampleAt(*plane, Point(0, 0, 0))); // .85 != .15
  EXPECT_EQ(plane->material.pattern.get()->SampleAt(*plane, Point(1, 1, 0)), c.SampleAt(*plane, Point(1, 1, 0))); // .85 == .85
}

TEST(YamlTest, DecodeDirectTransform)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- add: sphere
  material:
  transform:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->transform, Transformations::Scaling(0.5, 0.5, 0.5) * Transformations::Translation(1, -1, 1) * IdentityMatrix);
}

TEST(YamlTest, DecodeDefinedTransform)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

- define: my-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]

- add: sphere
  material:
  transform: my-transform
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->transform, Transformations::Scaling(0.5, 0.5, 0.5) * Transformations::Translation(1, -1, 1) * IdentityMatrix);
}

TEST(YamlTest, DecodeExtendedTransform)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

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

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->transform, Transformations::Scaling(2, 2, 2) * Transformations::Scaling(0.5, 0.5, 0.5) * Transformations::Translation(1, -1, 1) * IdentityMatrix);
}

TEST(YamlTest, DecodeDefinedTransformTree)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

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

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->transform, Transformations::Scaling(2, 2, 2) * Transformations::Scaling(3.5, 3.5, 3.5) * Transformations::Scaling(0.5, 0.5, 0.5) * Transformations::Translation(1, -1, 1) * IdentityMatrix);
}

TEST(YamlTest, DecodeDefinedTransformTreeWithExtendedTransform)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

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

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->transform, Transformations::Scaling(2.1, 2.1, 2.1) * Transformations::Scaling(3.5, 3.5, 3.5) * Transformations::Scaling(2, 2, 2) * Transformations::Scaling(0.25, 0.25, 0.25) * Transformations::Translation(1, -1, 1) * IdentityMatrix);
}

TEST(YamlTest, FailedMaterialExtend1)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90 

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

TEST(YamlTest, FailedMaterialExtend2)
{
  const string yaml = R"(
- add: camera
  width: 100
  height: 100
  field-of-view: 0.785
  from: [-6, 6, -10]
  to: [6, 0, 6]
  up: [-0.45, 1, 0]
- define: white-material
  value:
    color: [1, 1, 1]
    diffuse: 0.7
    ambient: 0.1
    specular: 0.0
    reflective: 0.1
- define: blue-material
  extend: white-material
  value:
    color: [0.537, 0.831, 0.914]
- define: red-material
  extend: white-material
  value:
    color: [0.941, 0.322, 0.388]
- define: purple-material
  extend: white-material
  value:
    color: [0.404, 0.550]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, FailedPatternExtend1)
{
  const string yaml = R"(
- add: camera
  width: 100
  height: 100
  field-of-view: 0.785
- define: white-material
  value:
    color: [1, 1, 1]
    diffuse: 0.7
    ambient: 0.1
    specular: 0.0
    reflective: 0.1
- define: blue-material
  extend: white-material
  value:
    color: [0.537, 0.831, 0.914]
- define: stripe-pattern
  value:
    type: stripes
    colors:
      - white-material
      - blue-material
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, FailedPatternExtend2)
{
  const string yaml = R"(
- add: camera
  width: 100
  height: 100
  field-of-view: 0.785
- define: stripe-pattern
  value:
    type: stripes
    colors:
      - [ 0.15, 0.15, 0.15 ]
      - [ 0.85, 0.85, 0.85 ]
- define: stripes2
  extend: stripe-pattern
  value:
    type: stripes
    colors:
      - [ 0.1, 0.1, 0.1 ]
      - [ 0.8, 0.8, 0.8 ]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, FailedTransformExtend1)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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

TEST(YamlTest, FailedTransformExtend2)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

- define: base-transform
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]

- define: extended-transform
  value:
    - bas-transform
    - [ scale, 2, 2, 2 ]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, ExtendedWrongType1)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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

TEST(YamlTest, ExtendedWrongType3)
{
  const string yaml = R"(
- add: camera
  width: 100
  height: 100
  field-of-view: 0.785
- define: white-material
  value:
    color: [1, 1, 1]
    diffuse: 0.7
    ambient: 0.1
    specular: 0.0
    reflective: 0.1
- define: blue-material
  extend: white-material
  value:
    color: [0.537, 0.831, 0.914]
- define: stripe-pattern
  value:
    type: stripes
    colors:
      - [ 0.15, 0.15, 0.15 ]
      - [ 0.85, 0.85, 0.85 ]
- define: red-material
  extend: stripe-pattern
  value:
    color: [0.941, 0.322, 0.388]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, ExtendedWrongType4)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

- define: stripe-pattern
  value:
    type: stripes
    colors:
      - [ 0.15, 0.15, 0.15 ]
      - [ 0.85, 0.85, 0.85 ]

- define: extended-transform
  extend: stripe-pattern
  value:
    - [ translate, 1, -1, 1 ]
    - [ scale, 0.5, 0.5, 0.5 ]
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);
}

TEST(YamlTest, CatchMissingShapeTags)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

- add: sphere
  transform:
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml), std::runtime_error);

  const string yaml1 = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

- add: sphere
  material:
)";

  EXPECT_THROW(YamlParser::getInstance().ParseYaml(yaml1), std::runtime_error);
}

TEST(YamlTest, DecodeDefaultSphere)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

- add: sphere
  material:
  transform:
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *sphere = world.shapes[0].get();

  EXPECT_EQ(sphere->material, Material());
  EXPECT_EQ(sphere->transform, IdentityMatrix);
}

TEST(YamlTest, DecodeDefaultCube)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

- add: cube
  material:
  transform:
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *cube = world.shapes[0].get();

  EXPECT_EQ(cube->material, Material());
  EXPECT_EQ(cube->transform, IdentityMatrix);
}

TEST(YamlTest, DecodeDefaultPlane)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

- add: plane
  material:
  transform:
)";

  World world = std::move(YamlParser::getInstance().ParseYaml(yaml).second);
  Shape *plane = world.shapes[0].get();

  EXPECT_EQ(plane->material, Material());
  EXPECT_EQ(plane->transform, IdentityMatrix);
}

TEST(YamlTest, BadShapeMaterial1)
{
  const string yaml = R"(
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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
- add: camera
  width: 1
  height: 1
  field-of-view: 90

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
