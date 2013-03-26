#pragma once

#include <duke/imageio/ImageDescription.hpp>
#include <duke/imageio/Attributes.hpp>
#include <memory>
#include <string>

namespace duke {

struct Frame {
	PackedFrameDescription description;
	Attributes attributes;
	std::shared_ptr<void> pData;
};

} /* namespace duke */
