#include <gtest/gtest.h>

#include <duke/base/StringUtils.hpp>

TEST(StringUtils, digits) {
    EXPECT_EQ(1, digits(0));
    EXPECT_EQ(1, digits(1));
    EXPECT_EQ(1, digits(9));
    EXPECT_EQ(2, digits(10));
    EXPECT_EQ(2, digits(99));
    EXPECT_EQ(9, digits(123456789));
}

TEST(StringUtils, append) {
    const std::vector<std::string> results = { "", "4", "34", "234", "1234", "01234", "001234", "0001234", "00001234", "000001234" };
    for (int i = 0; i < results.size(); ++i) {
        std::string tmp;
        appendPaddedFrameNumber(1234, i, tmp);
        EXPECT_EQ(results.at(i), tmp);
    }
}
