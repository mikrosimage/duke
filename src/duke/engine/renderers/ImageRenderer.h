/*
 * ImageRenderer.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGERENDERER_H_
#define IMAGERENDERER_H_

#include <duke/engine/renderers/IRenderer.h>
#include <duke/engine/DukeWindow.h>
#include <duke/gl/Shader.hpp>
#include <duke/gl/Mesh.hpp>

struct LoadableTexture;

namespace duke {

struct Context;

class AbstractRenderer: public IRenderer {
protected:
	AbstractRenderer(SharedVertexShader, SharedFragmentShader);
	virtual ~AbstractRenderer() = 0;
	const Program m_Program;
	const GLuint gViewport;
	const GLuint gImage;
	const GLuint gPan;
	const GLuint gTextureSampler;
	const SharedMesh m_pMesh;
};

class ImageRenderer: public AbstractRenderer {
public:
	ImageRenderer();
	void draw(const Context &context, const LoadableTexture& texture) const;
private:
	const GLuint gZoom;
	const GLuint gShowChannel;
	const GLuint gExposure;
};

} /* namespace duke */
#endif /* IMAGERENDERER_H_ */
