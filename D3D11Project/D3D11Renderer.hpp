#pragma once

#include <GLFW/glfw3.h>
#include <d3d11.h>
class FrameBuffer;

class D3D11Renderer
{
    public:
        // Constructor.
        // winWidth Window width in pixels.
        // winHeight Window height in pixels.
        D3D11Renderer(unsigned int winWidth = 640, unsigned int winHeight = 640);

        // Destructor.
        ~D3D11Renderer();

        // Whether window is running of not.
        bool Running() const;

        // Close window.
        void Close();

        // Present.
        // frameBuffer Frame buffer to present to window.
        void Present(FrameBuffer* frameBuffer);

        // D3D11 device.
        ID3D11Device* mDevice;

        // D3D11 device context.
        ID3D11DeviceContext* mDeviceContext;

    private:
        void InitialiseGLFW();
        void DeInitialiseGLFW();

        void InitialiseD3D11();
        void DeInitialiseD3D11();

        unsigned int mWinWidth;
        unsigned int mWinHeight;
        bool mClose;
        GLFWwindow* mWindow;

        //D3D11
        IDXGISwapChain* mSwapChain;
        FrameBuffer* mWinFrameBuffer;
        ID3D11SamplerState* mSamplerState;
        ID3D11RasterizerState* mRasterizerState;
};
