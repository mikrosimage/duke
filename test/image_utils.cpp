#include "duke/image/ImageUtils.hpp"

#include <gtest/gtest.h>

// TEST(ImageUtils, ChannelsByteSize) {
//  Channels channels;
//  EXPECT_EQ(0, getChannelsByteSize(channels));
//  channels.emplace_back(NumericType::UNSIGNED, 8, "R");
//  EXPECT_EQ(1, getChannelsByteSize(channels));
//  channels.emplace_back(NumericType::FLOAT, 64, "R");
//  EXPECT_EQ(9, getChannelsByteSize(channels));
//}
//
// TEST(ImageUtils, ImageSize) {
//  ImageDescription description;
//  EXPECT_EQ(0, getImageSize(description));
//  description.width = 10;
//  description.height = 10;
//  description.channels.emplace_back(NumericType::UNSIGNED, 8, "R");
//  description.channels.emplace_back(NumericType::UNSIGNED, 8, "G");
//  description.channels.emplace_back(NumericType::UNSIGNED, 8, "B");
//  EXPECT_EQ(10 * 10 * 3, getImageSize(description));
//  description.channels.emplace_back(NumericType::FLOAT, 8, "A");
//  EXPECT_EQ(10 * 10 * 4, getImageSize(description));
//}
