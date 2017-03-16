#include <crtdbg.h>

#include "D3D11Renderer.hpp"
#include "CPUTimer.hpp"
#include "FrameBuffer.hpp"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // +++ INIT +++ //
    unsigned int width = 800;
    unsigned int height = 600;
    D3D11Renderer renderer(width, height);
    ID3D11Device* device = renderer.mDevice;
    ID3D11DeviceContext* deviceContext = renderer.mDeviceContext;
    FrameBuffer frameBuffer(device, deviceContext, width, height);
    // --- INIT --- //

    // +++ MAIN LOOP +++ //
    float dt = 0.f;
    while (renderer.Running())
    {
        CPUTIMER(dt);
        // +++ UPDATE +++ //
        // --- UPDATE --- //

        // +++ RENDER +++ //
        frameBuffer.ClearAll(0.2f, 0.2f, 0.2f);
        // --- RENDER --- //

        // +++ PRESENET +++ //
        renderer.Present(&frameBuffer);
        // --- PRESENET --- //
    }
    // --- MAIN LOOP --- //

    // +++ SHUTDOWN +++ //
    // --- SHUTDOWN --- //

    return 0;
}
