#include "D3D11Renderer.hpp"
#include "DxAssert.hpp"
#include "FrameBuffer.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

D3D11Renderer::D3D11Renderer(unsigned int winWidth, unsigned int winHeight)
{
    mWinWidth = winWidth;
    mWinHeight = winHeight;
    mClose = false;

    // Window.
    InitialiseGLFW();

    // D3D11.
    InitialiseD3D11();
}

D3D11Renderer::~D3D11Renderer()
{
    DeInitialiseD3D11();
    DeInitialiseGLFW();
}

bool D3D11Renderer::Running() const
{
    if (mClose)
        return true;

    glfwPollEvents();

    return !glfwWindowShouldClose(mGLFWwindow);
}

void D3D11Renderer::Close()
{
    mClose = true;
}

void D3D11Renderer::Present(FrameBuffer* frameBuffer)
{
    mWinFrameBuffer->Copy(frameBuffer);
    DxAssert(mSwapChain->Present(0, 0), S_OK);
}

void D3D11Renderer::InitialiseGLFW()
{
    /* Initialize the library */
    if (!glfwInit())
        assert(0 && "GLFWERROR: Initialize the library.");

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create window */
    mGLFWwindow = glfwCreateWindow(mWinWidth, mWinHeight, "D3D11 window", NULL, NULL);
}

void D3D11Renderer::DeInitialiseGLFW()
{
    glfwTerminate();
}

void D3D11Renderer::InitialiseD3D11()
{
    assert(mGLFWwindow != NULL);

    // We initiate the device, device context and swap chain.
    DXGI_SWAP_CHAIN_DESC scDesc;
    scDesc.BufferDesc.Width = mWinWidth; 		// Using the window's size avoids weird effects. If 0 the window's client width is used.
    scDesc.BufferDesc.Height = mWinHeight;		// Using the window's size avoids weird effects. If 0 the window's client height is used.
    scDesc.BufferDesc.RefreshRate.Numerator = 0;	// Screen refresh rate as RationalNumber. Zeroing it out makes DXGI calculate it.
    scDesc.BufferDesc.RefreshRate.Denominator = 0;	// Screen refresh rate as RationalNumber. Zeroing it out makes DXGI calculate it.
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						// The most common format. Variations include [...]UNORM_SRGB.
    scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// The order pixel rows are drawn to the back buffer doesn't matter.
    scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					// Since the back buffer and window sizes matches, scaling doesn't matter.
    scDesc.SampleDesc.Count = 1;												// Disable multisampling.
    scDesc.SampleDesc.Quality = 0;												// Disable multisampling.
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // The back buffer will be rendered to.
    scDesc.BufferCount = 1;							        // We only have one back buffer.
    scDesc.OutputWindow = glfwGetWin32Window(mGLFWwindow);	    // Must point to the handle for the window used for rendering.
    scDesc.Windowed = true;					                // Run in windowed mode.
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	        // This makes the display driver select the most efficient technique.
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // Alt-enter fullscreen.

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef BUILD_ENABLE_D3D11_DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    DxAssert(D3D11CreateDeviceAndSwapChain(
        nullptr,					// Use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,	// Use the graphics card for rendering. Other options include software emulation.
        NULL,						// NULL since we don't use software emulation.
        createDeviceFlags,	// Dbg creation flags.
        nullptr,					// Array of feature levels to try using. With null the following are used 11.0, 10.1, 10.0, 9.3, 9.2, 9.1.
        0,							// The array above has 0 elements.
        D3D11_SDK_VERSION,			// Always use this.
        &scDesc,					// Description of the swap chain.
        &mSwapChain,				// [out] The created swap chain.
        &mDevice,					// [out] The created device.
        nullptr,					// [out] The highest supported feature level (from array).
        &mDeviceContext				// [out] The created device context.
    ), S_OK);

    // Window frame buffer.
    ID3D11Texture2D* backBufferTex;
    DxAssert(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTex)), S_OK);
    mWinFrameBuffer = new FrameBuffer(mDevice, mDeviceContext, mWinWidth, mWinHeight, D3D11_BIND_RENDER_TARGET, 0, backBufferTex);

    // Sample state.
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        desc.MipLODBias = 0.f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        DxAssert(mDevice->CreateSamplerState(&desc, &mSamplerState), S_OK);
        mDeviceContext->PSSetSamplers(0, 1, &mSamplerState);
    }

    // Ras state.
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
        desc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = false;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0.f;
        desc.DepthBiasClamp = 0.f;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = false;
        desc.AntialiasedLineEnable = false;
        DxAssert(mDevice->CreateRasterizerState(&desc, &mRasterizerState), S_OK);
        mDeviceContext->RSSetState(mRasterizerState);
    }

    // Viewport.
    {
        D3D11_VIEWPORT vp;
        vp.Width = (float)mWinWidth;
        vp.Height = (float)mWinHeight;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        mDeviceContext->RSSetViewports(1, &vp);
    }

}

void D3D11Renderer::DeInitialiseD3D11()
{
    delete mWinFrameBuffer;

    mSamplerState->Release();
    mRasterizerState->Release();

    mDevice->Release();
    mDeviceContext->Release();
    mSwapChain->Release();
}
