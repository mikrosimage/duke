/*
 * Dx9Enum.h
 *
 *  Created on: 17 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef DX9ENUM_H_
#define DX9ENUM_H_

#include <renderer/plugin/common/Enums.h>
#include <d3d9.h>

class Dx9Enum
{
public:
	static unsigned long    BufferFlags( unsigned long flags );
	static unsigned long    LockFlags( unsigned long flags );
	static D3DPRIMITIVETYPE Get( TPrimitiveType type );
	static D3DFORMAT        Get( TPixelFormat format );
};

#endif /* DX9ENUM_H_ */
