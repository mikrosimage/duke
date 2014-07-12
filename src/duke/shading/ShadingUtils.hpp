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
 * Returns a function to adapt a channel format.
 * It adapts channel dynamics, handle swizzling, and dpx channel unpacking
 * New channels are set to their default value ( max for alpha, 0 for the other channels ).
 */
Function createChannelAdapter(const StringSlice name, const Channels& channelsIn, const Channels& channelsOut);

} /* namespace shader */
} /* namespace duke */
