/*
 * TexturePool.h
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTUREPOOL_H_
#define TEXTUREPOOL_H_

#include <duke/NonCopyable.h>
#include <duke/gl/Textures.h>
#include <map>
#include <stack>
#include <memory>

namespace duke {

template<typename TEXTURE>
struct TexturePool: public noncopyable {
	typedef std::shared_ptr<TEXTURE> SharedTexture;
	SharedTexture get(const PackedFrameDescription &key) {
		SharedTextures& sharedTextures = m_Pool[key];
		if (!sharedTextures.empty()) {
			SharedTexture pTexture = sharedTextures.top();
			sharedTextures.pop();
			return pTexture;
		}
		SharedTexture pTexture(new TEXTURE(), recycleFunc());
		pTexture->initialize(key);
		return pTexture;
	}
	void recycle(TEXTURE* pTexture) {
		m_Pool[pTexture->description].emplace(pTexture, recycleFunc());
	}
private:
	inline std::function<void(TEXTURE*)> recycleFunc() {
		using namespace std;
		return bind(&TexturePool::recycle, this, placeholders::_1);
	}
	typedef std::stack<SharedTexture> SharedTextures;
	typedef std::map<PackedFrameDescription, SharedTextures> SharedTexturesMap;
	SharedTexturesMap m_Pool;
};

} /* namespace duke */

#endif /* TEXTUREPOOL_H_ */
