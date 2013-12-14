#include <gtest/gtest.h>

#include <duke/filesystem/FsUtils.hpp>

TEST(FsUtils, extension) {
    EXPECT_EQ(nullptr, duke::fileExtension(nullptr));
    EXPECT_EQ(nullptr, duke::fileExtension(""));
    EXPECT_EQ(nullptr, duke::fileExtension("a"));
    EXPECT_STREQ("", duke::fileExtension("."));
    EXPECT_STREQ("b", duke::fileExtension("a.b"));
}
