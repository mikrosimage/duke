#pragma once

#include <duke/gl/GlUtils.hpp>
#include <duke/image/ImageDescription.hpp>

#include <functional>
#include <tuple>

namespace duke {

struct ImageDescriptionLess : std::binary_function<ImageDescription, ImageDescription, bool> {
  std::tuple<uint32_t, uint32_t, int> asTuple(const ImageDescription& d) const {
    return std::make_tuple(d.width, d.height, d.opengl_format);
  }

  bool operator()(const ImageDescription& x, const ImageDescription& y) const { return asTuple(x) < asTuple(y); }
};

struct TexturePoolPolicy : public pool::PoolBase<ImageDescription, Texture, ImageDescriptionLess> {
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
