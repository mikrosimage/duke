/*
 * TexturePool.h
 *
 *  Created on: Feb 16, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTUREPOOL_H_
#define TEXTUREPOOL_H_

#include <duke/imageio/PackedFrameDescription.h>
#include <duke/gl/GLUtils.h>

namespace duke {

struct TexturePoolPolicy: public pool::PoolBase<PackedFrameDescription, Texture> {
protected:
	value_type* evictAndCreate(const key_type& key, PoolMap &map) {
		auto *pValue = new Texture();
		{
			auto bound = pValue->scope_bind_texture();
			pValue->initialize(key, nullptr);
		}
		++count;
//		printf("creating texture of size %lux%lu, format '%s', pool size : %lu\n", key.width, key.height, getInternalFormatString(key.glPackFormat), count);
		return pValue;
	}

	key_type retrieveKey(const value_type* pData) {
//		printf("releasing texture\n");
		return pData->description;
	}
public:
	size_t count = 0;
};

typedef pool::Pool<TexturePoolPolicy> TexturePool;

}  // namespace duke

#endif /* TEXTUREPOOL_H_ */
