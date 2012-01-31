#include "Dx9Utils.h"
#include <Cg/cgD3D9.h>

#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace std;

long check(long result, const char* msg, const char* filename, const char* function, const unsigned long line) {
    if (FAILED(result)) {
        ostringstream errorMsg;
        errorMsg << "FAILED ";
        errorMsg << filename << ':' << line << '\t' << function << '\t' << cgD3D9TranslateHRESULT(result);
        errorMsg << " (" << reinterpret_cast<void*> (result) << ')' << endl;
        if (msg)
            errorMsg << msg << endl;
        throw std::runtime_error(errorMsg.str());
    }
    return result;
}

RAIITextureLock::RAIITextureLock(IDirect3DTexture9* ptr, D3DLOCKED_RECT &lockedRect, const RECT *pRect, DWORD flags) :
    m_Ptr(ptr) {
    CHECK(m_Ptr->LockRect(0, &lockedRect, pRect, flags));
}

RAIITextureLock::~RAIITextureLock() {
    // we can't throw in a Dtor
    if (FAILED( m_Ptr->UnlockRect(0) ))
        cerr << "FAILED Dx9Renderer::dumpTexture UnlockRect" << endl;
}

RAIISurfaceLock::RAIISurfaceLock(IDirect3DSurface9* ptr, D3DLOCKED_RECT &lockedRect, const RECT *pRect, DWORD flags) :
    m_Ptr(ptr) {
    CHECK(m_Ptr->LockRect(&lockedRect, pRect, flags));
}

RAIISurfaceLock::~RAIISurfaceLock() {
    // we can't throw in a Dtor
    if (FAILED( m_Ptr->UnlockRect() ))
        cerr << "FAILED Dx9Renderer::dumpTexture UnlockRect" << endl;
}
