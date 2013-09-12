#include <gtest/gtest.h>
#include <tuple>
#include <utility>

#include <duke/cmdline/CmdLineParameters.hpp>

duke::CmdLineParameters build(std::initializer_list<const char*> args) {
    std::vector<const char*> all_args = { "duke" };
    std::copy(begin(args), end(args), std::back_inserter(all_args));
    return duke::CmdLineParameters(all_args.size(), all_args.data());
}

TEST(CmdLine,SwapInterval) {
    EXPECT_EQ(build( { "--swapinterval", "4" }).swapBufferInterval, 4);
}

TEST(CmdLine,fullscreen) {
    EXPECT_FALSE(build( { }).fullscreen);
    EXPECT_TRUE(build( { "--fullscreen" }).fullscreen);
    EXPECT_TRUE(build( { "-f" }).fullscreen);
}

TEST(CmdLine,threads) {
    EXPECT_GE(build( { }).workerThreadDefault, 1);
    EXPECT_EQ(build( { "--threads", "4" }).workerThreadDefault, 4);
    EXPECT_EQ(build( { "-t", "4" }).workerThreadDefault, 4);
}

TEST(CmdLine,default_max_cache) {
    const auto default_cache = build( { }).imageCacheSizeDefault;
    const auto max_cache = build( { "--max-cache-size" }).imageCacheSizeDefault;
    EXPECT_GE(default_cache, 0);
    EXPECT_GE(max_cache, 0);
    EXPECT_NE(default_cache, max_cache);
}

TEST(CmdLine,cache) {
    {
        const auto cache = build( { "--cache-size", "5" }).imageCacheSizeDefault;
        EXPECT_EQ(cache, 5 * 1024 * 1024);
    }
    {
        const auto cache = build( { "-s", "5" }).imageCacheSizeDefault;
        EXPECT_EQ(cache, 5 * 1024 * 1024);
    }
}

TEST(CmdLine,mode) {
    EXPECT_EQ(build( { "--benchmark" }).mode, duke::ApplicationMode::BENCHMARK);
    EXPECT_EQ(build( { "--version" }).mode, duke::ApplicationMode::VERSION);
    EXPECT_EQ(build( { "-v" }).mode, duke::ApplicationMode::VERSION);
    EXPECT_EQ(build( { "--help" }).mode, duke::ApplicationMode::HELP);
    EXPECT_EQ(build( { "-h" }).mode, duke::ApplicationMode::HELP);
    EXPECT_EQ(build( { "--list" }).mode, duke::ApplicationMode::LIST_SUPPORTED_FORMAT);
    EXPECT_EQ(build( { "-l" }).mode, duke::ApplicationMode::LIST_SUPPORTED_FORMAT);
}

TEST(CmdLine,files) {
    EXPECT_EQ(build( { "file" }).additionnalOptions, std::vector<std::string> { "file" });
}

TEST(CmdLine,framerate) {
    EXPECT_EQ(build( { "--framerate", "noskip" }).unlimitedFPS, true);
    EXPECT_EQ(build( { "--framerate", "5" }).defaultFrameRate, FrameDuration(1, 5));
    EXPECT_EQ(build( { "--framerate", "29.97" }).defaultFrameRate, FrameDuration(100, 2997));
    EXPECT_EQ(build( { "--framerate", "30000/1001" }).defaultFrameRate, FrameDuration::NTSC);
}
