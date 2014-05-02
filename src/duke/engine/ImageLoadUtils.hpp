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
ReadFrameResult loadImage(IImageReader* pRawReader, const LoadCallback& callback, ReadFrameResult&& result);

ReadFrameResult load(const attribute::Attributes& options, const LoadCallback& callback, ReadFrameResult&& result);

struct Texture;
ReadFrameResult load(const char* pFilename, Texture& texture);

} /* namespace duke */
