#pragma once

#include <duke/attributes/Attributes.hpp>
#include <duke/engine/streams/IIOOperation.hpp>

#include <string>
#include <functional>

struct PackedFrameDescription;

namespace duke {

struct RawPackedFrame;

typedef std::function<void(RawPackedFrame& packedFrame, const void* pVolatileData)> LoadCallback;

class IImageReader;
InputFrameOperationResult loadImage(IImageReader* pRawReader, const LoadCallback& callback,
                                    InputFrameOperationResult&& result);

InputFrameOperationResult load(const attribute::Attributes& options, const LoadCallback& callback,
                               InputFrameOperationResult&& result);

struct Texture;
InputFrameOperationResult load(const char* pFilename, Texture& texture);

} /* namespace duke */
