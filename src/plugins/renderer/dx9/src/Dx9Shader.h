/*
 * Dx9Shader.h
 *
 *  Created on: 19 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef DX9SHADER_H_
#define DX9SHADER_H_

#include <renderer/plugin/common/IShaderBase.h>

class Dx9Shader : public IShaderBase
{
public:
	Dx9Shader( CGprogram program, TShaderType type );
	virtual ~Dx9Shader();

protected:
	virtual void setParameter( const CGparameter parameter, const float* value, const int size ) const;
};

#endif /* DX9SHADER_H_ */
