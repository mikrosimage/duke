#ifndef ENUMS_H_
#define ENUMS_H_

#include <dukeio/Formats.h>

/////////////////////////////////////////////////////////////
/// Buffers creation flags
/////////////////////////////////////////////////////////////
enum
{
	BUF_STATIC = 1 << 0, BUF_DYNAMIC = 1 << 1
};

/////////////////////////////////////////////////////////////
/// Buffers lock flags
/////////////////////////////////////////////////////////////
enum
{
	LOCK_READONLY = 1 << 0, LOCK_WRITEONLY = 1 << 1
};

/////////////////////////////////////////////////////////////
/// Primitive types
/////////////////////////////////////////////////////////////
enum TPrimitiveType
{
	PT_POINTLIST = 1, PT_LINELIST = 2, PT_LINESTRIP = 3, PT_TRIANGLELIST = 4, PT_TRIANGLESTRIP = 5, PT_TRIANGLEFAN = 6
};

/////////////////////////////////////////////////////////////
/// Texture creation flag
/////////////////////////////////////////////////////////////
enum
{
	TEX_MIPMAP = 1 << 0, TEX_AUTOMIPMAP = 1 << 1, TEX_RENTERTARGET = 1 << 2
};

/////////////////////////////////////////////////////////////
/// Capabilities enum
/////////////////////////////////////////////////////////////
enum TCapability
{
	CAP_HW_MIPMAPPING, ///< Automatic hardware mipmap
	CAP_TEX_NON_POWER_2,	///< Allow creation of texture non power of two
	CAP_PIXEL_BUFFER_OBJECT	///< OGL PBO
};

enum TShaderType
{
	SHADER_VERTEX, SHADER_PIXEL
};
#include <dukeapi/protocol/player/communication.pb.h>
struct Enums
{
	static TPrimitiveType Get( const ::protocol::duke::Mesh_MeshType& type );
	static TPixelFormat   Get( const ::protocol::duke::Texture_TextureFormat& format );
	static ::protocol::duke::Texture_TextureFormat   Get( const TPixelFormat& format );
};

#endif /* ENUMS_H_ */
