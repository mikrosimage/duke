/*
 * OGLShader.cpp
 *
 *  Created on: 02 juin 2010
 *      Author: Nicolas Rondaud
 */

#include "OGLShader.h"
#include <Cg/cgGL.h>

OGLShader::OGLShader( CGprogram program, TShaderType type )
	: IShaderBase( program, type )
{
	cgGLLoadProgram( m_Program );
}

OGLShader::~OGLShader() {}

void OGLShader::setParameter( const CGparameter parameter, const float* value, const int size ) const
{
	assert( parameter );
	cgSetParameterValuefr( parameter, size, value );
}

