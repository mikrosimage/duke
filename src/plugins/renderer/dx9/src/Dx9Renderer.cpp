#include "Dx9Renderer.h"
#include "Dx9Buffer.h"
#include "Dx9Shader.h"
#include "Dx9Texture.h"
#include "DX9Enum.h"
#include "Dx9Utils.h"
#include <dukerenderer/plugin/Mesh.h>
#include <dukerenderer/plugin/utils/PixelUtils.h>
#include <dukerenderer/plugin/SfmlWindow.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <d3d9.h>
#include <Cg/cgD3D9.h>

using namespace ::google::protobuf;
using namespace ::protocol::duke;
using namespace ::std;

#define CUSTOMFVF ( D3DFVF_XYZ | D3DFVF_TEX1 )
const string HEADER("[Dx9Renderer] ");
static const D3DFORMAT g_BackBufferFormat = D3DFMT_X8R8G8B8;

static UINT getPresentationInterval(google::protobuf::uint32 interval) {
    switch (interval) {
        case 0:
            return D3DPRESENT_INTERVAL_IMMEDIATE;
        case 1:
            return D3DPRESENT_INTERVAL_ONE;
        case 2:
            return D3DPRESENT_INTERVAL_TWO;
        case 3:
            return D3DPRESENT_INTERVAL_THREE;
        case 4:
            return D3DPRESENT_INTERVAL_FOUR;
        default:
            throw runtime_error("Invalid presentation interval");
    }
}

Dx9Renderer::Dx9Renderer(const Renderer& renderer, sf::Window& window, const RendererSuite& suite) :
    IRenderer(renderer, window, suite), m_pD3d(Direct3DCreate9(D3D_SDK_VERSION)), m_iVertexCount(0), m_bEndSceneCalled(true) {
    const HWND hWnd = FindWindow("SFML_Window", SFML_WINDOW_TITLE);

    const bool fullscreen = renderer.fullscreen();

    D3DPRESENT_PARAMETERS d3dpp; // create a struct to hold various device information
    memset(&d3dpp, 0, sizeof(d3dpp)); // clear out the struct for use
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferFormat = g_BackBufferFormat; // set the back buffer format to 32-bit
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.hDeviceWindow = hWnd; // set the window to be used by Direct3D
    d3dpp.Windowed = true;
    d3dpp.EnableAutoDepthStencil = false;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.Flags = 0;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;//getPresentationInterval(renderer.presentinterval());

    if (fullscreen) {
        d3dpp.BackBufferWidth = renderer.width(); // set the width of the buffer
        d3dpp.BackBufferHeight = renderer.height(); // set the height of the buffer
        if (renderer.has_refreshrate())
            d3dpp.FullScreen_RefreshRateInHz = renderer.refreshrate();
    }

    // create a device class using this information and information from the d3dpp stuct
    unsigned adapterToUse = D3DADAPTER_DEFAULT;
    D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;

#if DEBUG
    const string PerfHUD("PerfHUD");
    cout << HEADER + "looking for " << PerfHUD << endl;
    for (unsigned adapter = 0; adapter < m_pD3d->GetAdapterCount(); ++adapter) {
        D3DADAPTER_IDENTIFIER9 id;
        CHECK(m_pD3d->GetAdapterIdentifier(adapter, 0, &id));
        cout << HEADER + "checking " << id.Description << endl;
        if (string(id.Description).find(PerfHUD) != string::npos) {
            adapterToUse = adapter;
            deviceType = D3DDEVTYPE_REF;
            cout << HEADER + "Using debug device " << id.DeviceName << " : " << id.Description << endl;
            break;
        }
    }
    cout << HEADER + "Using renderer configuration " << endl << renderer.DebugString() << endl;
#endif

    CHECK(m_pD3d->CreateDevice(adapterToUse, //
                    deviceType, //
                    hWnd, //
                    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, //
                    &d3dpp, //
                    &m_pD3dDevice));

    checkCaps();

    m_pD3dDevice->SetRenderState(D3DRS_DITHERENABLE, true);
    m_pD3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    m_pD3dDevice->SetRenderState(D3DRS_ZENABLE, false);
    m_pD3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
    m_pD3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    m_pD3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    m_pD3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pD3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xFF);
    m_pD3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFF);
    ClipCursor(NULL);

    cgD3D9SetDevice(m_pD3dDevice);
#ifdef DEBUG
    cgD3D9EnableDebugTracing( CG_TRUE );
#endif
    cgD3D9RegisterStates(getCgContext());

    m_VSProfile = cgD3D9GetLatestVertexProfile();
    m_PSProfile = cgD3D9GetLatestPixelProfile();
    m_VSOptions = cgD3D9GetOptimalOptions(m_VSProfile);
    m_PSOptions = cgD3D9GetOptimalOptions(m_PSProfile);

}

Dx9Renderer::~Dx9Renderer() {
}

IBufferBase* Dx9Renderer::createVB(unsigned long size, unsigned long stride, unsigned long flags) const {
    LPDIRECT3DVERTEXBUFFER9 vertexBuffer;

    CHECK(m_pD3dDevice->CreateVertexBuffer(size * stride, //
                    Dx9Enum::BufferFlags(flags), //
                    CUSTOMFVF, //
                    D3DPOOL_DEFAULT, //
                    &vertexBuffer, //
                    NULL));

    return new DX9VertexBuffer(size, vertexBuffer);
}

IBufferBase* Dx9Renderer::createIB(unsigned long size, unsigned long stride, unsigned long flags) const {
    LPDIRECT3DINDEXBUFFER9 indexBuffer;

    CHECK(m_pD3dDevice->CreateIndexBuffer(size * stride, //
                    Dx9Enum::BufferFlags(flags), //
                    stride == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, //
                    D3DPOOL_DEFAULT, //
                    &indexBuffer, //
                    NULL));

    return new DX9IndexBuffer(size, indexBuffer);
}

// IRenderer
IShaderBase* Dx9Renderer::createShader(CGprogram program, TShaderType type) const {
    return new Dx9Shader(program, type);
}

void Dx9Renderer::setVertexBuffer(unsigned int stream, const IBufferBase* buffer, unsigned long stride) {
    const DX9VertexBuffer* vertexBuffer = dynamic_cast<const DX9VertexBuffer*> (buffer);

    CHECK(m_pD3dDevice->SetStreamSource(stream, //
                    vertexBuffer ? vertexBuffer->getBuffer() : NULL,//
                    0, //
                    stride));

    m_iVertexCount = buffer->size();
}

TPixelFormat Dx9Renderer::getCompliantFormat(TPixelFormat format) const {
    switch (format) {
        case PXF_A2B10G10R10:
        case PXF_R10G10B10A2:
        case PXF_R8G8B8A8:
            return PXF_B8G8R8A8;
        default:
            return format;
    }
}

ITextureBase* Dx9Renderer::createTexture(const ImageDescription& description, unsigned long usageFlags) const {
    if (description.format == PXF_UNDEFINED)
        return NULL;
    return new Dx9Texture(description, usageFlags, *this);
}

void Dx9Renderer::setIndexBuffer(const IBufferBase* buffer) {
    const DX9IndexBuffer* indexBuffer = dynamic_cast<const DX9IndexBuffer*> (buffer);

    CHECK(m_pD3dDevice->SetIndices(indexBuffer ? indexBuffer->getBuffer() : NULL));
}

void Dx9Renderer::drawPrimitives(TPrimitiveType meshType, unsigned long count) {
    CHECK(m_pD3dDevice->DrawPrimitive(Dx9Enum::Get(meshType), 0, count));
}

void Dx9Renderer::drawIndexedPrimitives(TPrimitiveType meshType, unsigned long count) {
    CHECK(m_pD3dDevice->DrawIndexedPrimitive(Dx9Enum::Get(meshType), 0, 0 /*m_MinVertex*/, m_iVertexCount, 0 /*FirstIndex*/, count));
}

inline static DWORD Get(const Effect_Blend &value) {
    switch (value) {
        case Effect_Blend_BLEND_ZERO:
            return D3DBLEND_ZERO;
        case Effect_Blend_BLEND_ONE:
            return D3DBLEND_ONE;
        case Effect_Blend_BLEND_SRCALPHA:
            return D3DBLEND_SRCALPHA;
        case Effect_Blend_BLEND_INVSRCALPHA:
            return D3DBLEND_INVSRCALPHA;
        case Effect_Blend_BLEND_DESTALPHA:
            return D3DBLEND_DESTALPHA;
        case Effect_Blend_BLEND_INVDESTALPHA:
            return D3DBLEND_INVDESTALPHA;
    }
    std::cerr << "invalid render state value " << Effect_Blend_Name(value) << std::endl;
    assert(false);
    return D3DBLEND_FORCE_DWORD;
}

void Dx9Renderer::setRenderState(const Effect &renderState) const {
    if (renderState.has_alphablend())
        CHECK(m_pD3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, renderState.alphablend() ? TRUE : FALSE));
    if (renderState.has_srcblend())
        CHECK(m_pD3dDevice->SetRenderState(D3DRS_SRCBLEND, Get(renderState.srcblend())));
    if (renderState.has_dstblend())
        CHECK(m_pD3dDevice->SetRenderState(D3DRS_DESTBLEND, Get(renderState.dstblend())));
    if (renderState.has_alphasrcblend())
        CHECK(m_pD3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA, Get(renderState.alphasrcblend())));
    if (renderState.has_alphadstblend())
        CHECK(m_pD3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, Get(renderState.alphadstblend())));
}

inline static D3DSAMPLERSTATETYPE Get(const SamplerState_Type type) {
    switch (type) {
        case SamplerState_Type_MIN_FILTER:
            return D3DSAMP_MINFILTER;
        case SamplerState_Type_MAG_FILTER:
            return D3DSAMP_MAGFILTER;
        case SamplerState_Type_WRAP_S:
            return D3DSAMP_ADDRESSU;
        case SamplerState_Type_WRAP_T:
            return D3DSAMP_ADDRESSV;
    }
    std::cerr << "invalid sampler state type " << SamplerState_Type_Name(type) << std::endl;
    assert(false);
    return D3DSAMP_FORCE_DWORD;
}

inline static int Get(const SamplerState_Value value) {
    using namespace ::protocol::duke;

    switch (value) {
        case SamplerState_Value_TEXF_POINT:
            return D3DTEXF_POINT;
        case SamplerState_Value_TEXF_LINEAR:
            return D3DTEXF_LINEAR;
        case SamplerState_Value_WRAP_CLAMP:
            return D3DTADDRESS_CLAMP;
        case SamplerState_Value_WRAP_BORDER:
            return D3DTADDRESS_BORDER;
        case SamplerState_Value_WRAP_REPEAT:
            return D3DTADDRESS_WRAP;
    }
    std::cerr << "invalid sampler state value " << SamplerState_Value_Name(value) << std::endl;
    assert(false);
    return D3DTSS_FORCE_DWORD;
}

static IDirect3DTexture9* getDxTexture(const ITextureBase* pTexture) {
    const Dx9Texture* dxTexture = dynamic_cast<const Dx9Texture*> (pTexture);
    return dxTexture ? dxTexture->GetDxTexture() : NULL;
}

void Dx9Renderer::setTexture(const CGparameter sampler, const RepeatedPtrField<SamplerState>& samplerStates, const ITextureBase* pTexture) const {
    IDirect3DTexture9* pD3DTexture = getDxTexture(pTexture);
    CHECK(cgD3D9SetTexture(sampler, pD3DTexture));

    //    cout << "Sampler " << cgGetParameterName(sampler) << " setting texture " << pD3DTexture << endl;

    for (auto itr = samplerStates.begin(); itr != samplerStates.end(); ++itr)
        CHECK(cgD3D9SetSamplerState(sampler, Get(itr->type()), Get(itr->value())));
}

void Dx9Renderer::setShader(IShaderBase* shader) {
    cgD3D9BindProgram(shader ? shader->getProgram() : NULL);
}

void Dx9Renderer::beginScene(bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget) {
    assert( m_bEndSceneCalled );
    // saving back buffer for further use
    if (m_pBackBuffer == NULL) // saving the back buffer
        CHECK(m_pD3dDevice->GetRenderTarget(0, &m_pBackBuffer));
    if (pRenderTarget) {
        assert( pRenderTarget->isRenderTarget() );
        // retrieving the surface associated with the texture
        IDirect3DTexture9* pD3DTexture = getDxTexture(pRenderTarget);
        CHECK(pD3DTexture->GetSurfaceLevel(0, &m_pRenderSurface));
        CHECK(m_pD3dDevice->SetRenderTarget(0, m_pRenderSurface));
        //        cout << HEADER << "SetRenderTarget surface " << *&m_pRenderSurface << " associated to texture " << pD3DTexture << endl;
    } else {
        CHECK(m_pD3dDevice->SetRenderTarget(0, m_pBackBuffer));
        //        cout << HEADER << "SetRenderTarget beginScene backBuffer restore " << *&m_pBackBuffer << " associated to no texture " << endl;
    }
    if (shouldClean)
        CHECK(m_pD3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, cleanColor, 1.0f, 0));

    CHECK(m_pD3dDevice->BeginScene()); // begins the 3D scene
    CHECK(m_pD3dDevice->SetFVF(CUSTOMFVF));
    m_bEndSceneCalled = false;
}

void Dx9Renderer::endScene() {
    CHECK(m_pD3dDevice->EndScene()); // ends the 3D scene
    m_pRenderSurface.release();
    m_bEndSceneCalled = true;
}

void Dx9Renderer::presentFrame() {
    m_pD3dDevice->Present(NULL, NULL, NULL, NULL); // displays the created frame
}

void Dx9Renderer::waitForBlanking() const {
    D3DRASTER_STATUS status;
    UINT lastScanline = UINT_MAX;
    while (true) {
        m_pD3dDevice->GetRasterStatus(0, &status);
        if (lastScanline != UINT_MAX && status.ScanLine < lastScanline)
            return; // we're in VBlank or we just passed it
        lastScanline = status.ScanLine;
    }
}

Image Dx9Renderer::dumpTexture(ITextureBase* pTextureBase) {
    assert( pTextureBase );
    Dx9Texture* pTexture = dynamic_cast<Dx9Texture*> (pTextureBase);

    // creating temporary surface to get system memory
    ScopedIUnknownPtr<IDirect3DSurface9> shadowSurface;
    CHECK(m_pD3dDevice->CreateOffscreenPlainSurface( //
                    pTexture->getWidth(), //
                    pTexture->getHeight(), //
                    D3DFMT_A8R8G8B8, //Dx9Enum::Get(pTexture->getFormat()), //
                    D3DPOOL_SYSTEMMEM, //
                    &shadowSurface, //
                    NULL));

    // getting texture surface
    ScopedIUnknownPtr<IDirect3DSurface9> renderTargetSurface;
    CHECK(pTexture->GetDxTexture()->GetSurfaceLevel(0, &renderTargetSurface));

    // copying texture surface to in-memory surface
    CHECK(m_pD3dDevice->GetRenderTargetData(renderTargetSurface, shadowSurface));

    // locking the surface to get data back
    D3DLOCKED_RECT lockRect;
    lockRect.Pitch = 0;
    lockRect.pBits = NULL;

    RAIISurfaceLock lock(shadowSurface, lockRect, NULL, D3DLOCK_READONLY);

    const size_t pixelSize = 4 * sizeof(char);
    ImageDescription description;
    description.width = lockRect.Pitch / pixelSize;
    description.height = pTexture->getHeight();
    description.format = pTexture->getFormat();
    description.pImageData = reinterpret_cast<char*> (lockRect.pBits);
    description.imageDataSize = lockRect.Pitch * description.height;
    return Image(*this, "", description);
}

void Dx9Renderer::checkCaps() {
    D3DCAPS9 capabilities;

    m_pD3dDevice->GetDeviceCaps(&capabilities);

    m_Capabilities[CAP_HW_MIPMAPPING] = (capabilities.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) != 0;
    m_Capabilities[CAP_TEX_NON_POWER_2] = (capabilities.TextureCaps & D3DPTEXTURECAPS_POW2) != 0;
}

