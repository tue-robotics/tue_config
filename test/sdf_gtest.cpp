#include "tue/config/reader_writer.h"
#include "tue/config/loaders/sdf.h"
#include <gtest/gtest.h>

#include <ros/package.h>

#include "tue/filesystem/path.h"

tue::config::ReaderWriter config;

TEST(SDF, LoadSDF)
{
    std::string path = ros::package::getPath("tue_config");
    path += "/test/model.sdf";
    EXPECT_TRUE(tue::config::loadFromSDFFile(path, config));
    EXPECT_TRUE(config.readGroup("sdf"));
}


TEST(SDF, AttributeString)
{
    std::string name;
    EXPECT_TRUE(config.readGroup("model"));
    EXPECT_TRUE(config.value("name", name));
    EXPECT_TRUE(config.endGroup());
}

TEST(SDF, AttributeDouble)
{
    double version = 0;
    EXPECT_TRUE(config.value("version", version));
}

TEST(SDF, AttributeInt)
{
    EXPECT_TRUE(true);
}

// Run all the tests that were declared with TEST()
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
