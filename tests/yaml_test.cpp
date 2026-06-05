#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

TEST(yamlTest, FileFound)
{
    YAML::Node testfile = YAML::LoadFile("tests/YamlTestFile1.yaml");
    EXPECT_TRUE(testfile.IsDefined());
}

TEST(yamlTest, CreateCamera)
{
    // This class has 3 variables that need to be set, none of which has default values
    YAML::Node testfile = YAML::LoadFile("tests/YamlTestFile1.yaml");

    
}


// Camera ViewTransform should be checked, it might be editable