#pragma once

#include <GLFW/glfw3.h>

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

    private:
        void InitialiseGLFW();
        void DeInitialiseGLFW();

        void InitialiseD3D11();
        void DeInitialiseD3D11();

        unsigned int mWinWidth;
        unsigned int mWinHeight;
        bool mClose;
        GLFWwindow* mWindow;
};
