/*
 * Dx9Shader.cpp
 *
 *  Created on: 19 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "Dx9Shader.h"
#include <Cg/cgD3D9.h>

Dx9Shader::Dx9Shader( CGprogram program, TShaderType type )
	: IShaderBase( program, type )
{
	cgD3D9LoadProgram( m_Program, false, 0 );
}

Dx9Shader::~Dx9Shader() {}

void Dx9Shader::setParameter( const CGparameter parameter, const float* value, const int size ) const
{
	assert( parameter );
	cgSetParameterValuefr( parameter, size, value );
}
