#pragma once

#include "duke/shading/ShadingModel.hpp"
#include "duke/image/Channel.hpp"

namespace duke {
namespace shader {

/**
 * Returns a shading function to sample from a texture with channels.
 */
Function createSampler(const StringSlice name, const Channels& channels);

/**
 * Returns true is channels semantic are R, RG, RGB or RGBA.
 */
bool isOrderedSemantic(const Channels& channels);

/**
 * Returns whether channels needs an unpacking function.
 * ie. RGB10A2 needs unpacking, RGB8 maps directly to a GL texture.
 */
bool needsUnpacking(const Channels& channels);

/**
 * Returns Channels compatible with an efficient OpenGL texture to store this packed texture.
 */
Channels getBestPackingTexture(const Channels& channels);

/**
 * Returns a function returning a vec4 that interprets 'stored' as 'packedAs'.
 */
Function unpackToVec4(const Channels& storage, const Channels& packedAs);

/**
 * Returns a function returning a vec4 from input.
 */
Function asVec4(const Channels& input);

/**
 * Creates a bunch of functions to interpret 'input' as a vec4.
 * 'texture' is set to an OpenGL texture that can store 'input'.
 * 'functions' contains all the shading functions needed to sample 'input'.
 */
void sampleAsVec4(const StringSlice sampleFunctionName, const Channels& input, Channels& texture,
                  std::vector<Function>& functions);

} /* namespace shader */
} /* namespace duke */
