#pragma once

#include <cstddef>
#include <tuple>

struct PackedFrameDescription {
	size_t width, height;
	size_t glPackFormat; // corresponds to OpenGL internal image format
	size_t dataSize;
	bool swapEndianness;
	bool swapRedAndBlue;
	PackedFrameDescription() :
			width(0), height(0), glPackFormat(0), dataSize(0), swapEndianness(false), swapRedAndBlue(false) {
	}
	const std::tuple<size_t, size_t, size_t, size_t> asTuple() const {
		return std::make_tuple(width, height, glPackFormat, dataSize);
	}

	bool operator<(const PackedFrameDescription &other) const {
		return asTuple() < other.asTuple();
	}
};
