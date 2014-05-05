#pragma once

#include <duke/gl/GLUtils.hpp>
#include <duke/image/FrameDescription.hpp>

namespace duke {

struct TexturePoolPolicy : public pool::PoolBase<FrameDescription, Texture> {
 protected:
  value_type* evictAndCreate(const key_type& key, PoolMap& map) {
    auto* pValue = new Texture();
    {
      auto bound = pValue->scope_bind_texture();
      pValue->initialize(key, nullptr);
    }
    ++count;
    return pValue;
  }

  key_type retrieveKey(const value_type* pData) { return pData->description; }

 public:
  size_t count = 0;
};

typedef pool::Pool<TexturePoolPolicy> TexturePool;

}  // namespace duke
