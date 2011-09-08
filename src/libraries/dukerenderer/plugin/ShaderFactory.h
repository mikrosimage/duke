/*
 * ShaderFactory.h
 *
 *  Created on: 21 juin 2010
 *      Author: Guillaume Chatelet
 */

#ifndef SHADERFACTORY_H_
#define SHADERFACTORY_H_

#include "Enums.h"
#include "IShaderBase.h"
#include "RenderingContext.h"
#include "ProtoBufResource.h"
#include <dukeapi/protocol/player/communication.pb.h>
#include <dukeio/ImageDescription.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

class IRenderer;

/**
 * Take a shader and modifies it to take care of image interpretation.
 * ShaderFactory can for instance :
 * - unpack R10G10B10A2 pixels stored in a R8G8B8A8 texture
 * - correct the channel ordering if the created texture is not the same as source, eg : RGBA not available so ARGB created
 * - swap the endianness
 * - inverse the vertical axis if the image is stored upside-down
 */
class ShaderFactory
{
public:
    ShaderFactory( IRenderer& renderer, const ::protocol::duke::Shader& shader, RenderingContext& context, const TShaderType type );

	ShaderBasePtr getShader() const;

private:
	CGprogram       createProgram( const std::string&, const std::string &name) const;
	TResourcePtr    getParam(const std::string &) const;
	void            applyParameters();
	void            applyParameter(const std::string &);
	void            applyParameter(const std::string &, const ::protocol::duke::AutomaticParameter&);
	void            applyParameter(const std::string &, const ::protocol::duke::StaticParameter&);

	ShaderBasePtr		m_pShader;

	// ShaderFactory is meant to last for a very short amount of time
	// so we can keep references here instead of copies
	IRenderer& 								m_Renderer;
	const ::protocol::duke::Shader			&m_Shader;
	RenderingContext					    &m_RenderingContext;
	const ImageDescriptions					&m_Images;
	const TShaderType 						m_Type;
	ResourceManager                         &m_ResourceManager;
};

#endif /* SHADERFACTORY_H_ */
