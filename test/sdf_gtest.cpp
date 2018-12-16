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

TEST(SDF, readGroup)
{
    EXPECT_FALSE(config.readArray("model"));
    EXPECT_TRUE(config.readGroup("model"));
    EXPECT_TRUE(config.endGroup());
}

TEST(SDF, String)
{
    std::string name;
    double nameD = 0.;
    int nameI = 0;
    config.readGroup("model");
    EXPECT_FALSE(config.value("name", nameD));
    EXPECT_FALSE(config.value("name", nameI));
    EXPECT_TRUE(config.value("name", name));
    config.endGroup();
    EXPECT_STREQ("robotics_testlabs_sdf/battery_table", name.c_str());
}

TEST(SDF, Double)
{
    double version = 0.;
    int versionI = 0;
    std::string versionS = "";
    EXPECT_FALSE(config.value("version", versionI));
    EXPECT_FALSE(config.value("version", versionS));
    EXPECT_TRUE(config.value("version", version));
    EXPECT_DOUBLE_EQ(1.6, version);
}

TEST(SDF, readArray)
{
    config.readGroup("model");
    EXPECT_FALSE(config.readGroup("link"));
    EXPECT_TRUE(config.readArray("link"));
    EXPECT_TRUE(config.nextArrayItem());
}

TEST(SDF, Int)
{
    int linkname = 0;
    double linknameD = 0.;
    std::string linknameS = "";
    EXPECT_TRUE(config.value("name", linknameD));
    EXPECT_FALSE(config.value("name", linknameS));
    EXPECT_TRUE(config.value("name", linkname));
    EXPECT_TRUE(config.endArray());
}



// Run all the tests that were declared with TEST()
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
