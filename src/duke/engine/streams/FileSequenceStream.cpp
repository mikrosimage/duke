/*
 * FileSequenceStream.cpp
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#include "FileSequenceStream.h"
#include <duke/engine/renderers/ImageRenderer.h>
#include <duke/engine/Context.h>

namespace duke {

FileSequenceStream::FileSequenceStream(const std::shared_ptr<ImageRenderer> &pRenderer, const char* filename) :
		m_pRenderer(pRenderer) {
	const GLenum textureMode = GL_NEAREST;
//	const GLenum textureMode = GL_LINEAR;
//	const char* filename = "ScanLines/StillLife.exr";
//	const char* filename = "ScanLines/MtTamWest.exr";
//	const char* filename = "ScanLines/Tree.exr";
//	const char* filename = "test.tga";
//	const char* filename = "sample1920X1080dpx10bit.dpx";
//	const char* filename = "checker.png";
	m_Texture.load(filename, textureMode, textureMode, GL_CLAMP_TO_EDGE);
}

void FileSequenceStream::doRender(const Context& context) const {
	checkError();
	m_pRenderer->draw(context, m_Texture);
	checkError();
}

const Attributes& FileSequenceStream::getAttributes() const {
	return m_Texture.attributes;
}

}
/* namespace duke */
