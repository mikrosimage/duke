#pragma once

#include <string>
#include <functional>

struct PackedFrameDescription;
struct Attributes;

namespace duke {

struct RawPackedFrame;

typedef std::function<void(RawPackedFrame&& packedFrame, const void* pVolatileData)> LoadCallback;

bool load(const char* pFilename, const char* pExtension, const LoadCallback& callback, std::string &error);

struct Texture;
bool load(const char* pFilename, Texture& texture, Attributes &attributes, std::string &error);

} /* namespace duke */
