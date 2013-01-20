/*
 * texturepool.cpp
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#include <gtest/gtest.h>

#include <duke/engine/TexturePool.h>

struct TestTexture: public duke::ITexture {
	void initialize(const PackedFrameDescription &description, const GLvoid * data = nullptr){
	}
	virtual void initialize(const PackedFrameDescription &description, GLint internalFormat, GLenum format, GLenum type, const GLvoid * data) {
	}
	virtual void update(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * data) {
	}
	virtual void doBind(bool) const {
	}
	std::string id;
};

TEST(TexturePool,automaticrecycling) {
	const PackedFrameDescription emptyDesc;
	duke::TexturePool<TestTexture> pool;

	{ // texture created and recycled
		auto pTexture = pool.get(emptyDesc);
		pTexture->id = "first";
	}
	// getting the previously recycled texture
	ASSERT_EQ("first", pool.get(emptyDesc)->id);
}
