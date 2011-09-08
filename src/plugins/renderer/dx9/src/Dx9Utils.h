/*
 * Dx9Utils.h
 *
 *  Created on: 3 juin 2011
 *      Author: Guillaume Chatelet
 */

#ifndef DX9UTILS_H_
#define DX9UTILS_H_

#include <d3d9.h>

long check(long result, const char* msg, const char* filename, const char* function, const unsigned long line);
//#define CHECK(X, MSG) check(X, MSG, __FILE__, __FUNCTION__, __LINE__);
#define CHECK(X) check(X, "", __FILE__, __FUNCTION__, __LINE__);

struct RAIITextureLock {
    RAIITextureLock(IDirect3DTexture9* ptr, D3DLOCKED_RECT &lockedRect, const RECT *pRect, DWORD flags);
    ~RAIITextureLock();
private:
    IDirect3DTexture9* m_Ptr;
};

struct RAIISurfaceLock {
    RAIISurfaceLock(IDirect3DSurface9* ptr, D3DLOCKED_RECT &lockedRect, const RECT *pRect, DWORD flags);
    ~RAIISurfaceLock();
private:
    IDirect3DSurface9* m_Ptr;
};

#endif /* DX9UTILS_H_ */
