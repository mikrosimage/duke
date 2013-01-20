/*
 * ImageLoadUtils.h
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGELOADUTILS_H_
#define IMAGELOADUTILS_H_

#include <string>
#include <functional>

struct PackedFrameDescription;
struct Attributes;

namespace duke {

typedef std::function<void(const PackedFrameDescription& description, const Attributes &attributes, const void* pData)> LoadCallback;

bool load(const char* pFilename, const LoadCallback& callback, std::string &error);

class ITexture;
bool load(const char* pFilename, ITexture& texture, Attributes &attributes, std::string &error);

} /* namespace duke */
#endif /* IMAGELOADUTILS_H_ */
