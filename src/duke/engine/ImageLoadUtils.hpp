#pragma once

#include <string>
#include <functional>
#include <duke/engine/streams/IIOOperation.hpp>

struct PackedFrameDescription;
struct Attributes;

namespace duke {

struct RawPackedFrame;

typedef std::function<void(RawPackedFrame& packedFrame, const void* pVolatileData)> LoadCallback;

InputFrameOperationResult load(const Attributes& readOptions,
                               const LoadCallback& callback,
                               InputFrameOperationResult&& result);

struct Texture;
InputFrameOperationResult load(const char* pFilename, Texture& texture);

} /* namespace duke */
