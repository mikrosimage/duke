#pragma once

#include <duke/imageio/PackedFrameDescription.hpp>
#include <duke/gl/GLUtils.hpp>

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
		return pValue;
	}

	key_type retrieveKey(const value_type* pData) {
		return pData->description;
	}
public:
	size_t count = 0;
};

typedef pool::Pool<TexturePoolPolicy> TexturePool;

}  // namespace duke
