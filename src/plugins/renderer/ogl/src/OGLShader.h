#ifndef OGLSHADER_H_
#define OGLSHADER_H_

#include <plugin/IShaderBase.h>

class OGLShader : public IShaderBase
{
public: OGLShader( CGprogram program, TShaderType type );
	virtual ~OGLShader();

	virtual void setParameter( const CGparameter parameter, const float* value, const int size ) const;
};

#endif /* OGLSHADER_H_ */
