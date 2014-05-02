#pragma once

#include <duke/attributes/Attributes.hpp>
#include <duke/engine/streams/IIOOperation.hpp>

#include <string>
#include <functional>

struct FrameDescription;

namespace duke {

struct FrameData;

typedef std::function<void(FrameData& frame, const void* pVolatileData)> LoadCallback;

class IImageReader;
ReadFrameResult loadImage(IImageReader* pRawReader, const LoadCallback& callback, ReadFrameResult&& result);

ReadFrameResult load(const attribute::Attributes& options, const LoadCallback& callback, ReadFrameResult&& result);

struct Texture;
ReadFrameResult load(const char* pFilename, Texture& texture);

} /* namespace duke */
