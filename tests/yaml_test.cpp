#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

TEST(yamlTest, FileFound) {
    YAML::Node testfile = YAML::LoadFile("tests/YamlTestFile1.yaml");
    EXPECT_TRUE(testfile.IsDefined());
}