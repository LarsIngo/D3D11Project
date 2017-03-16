#include "D3D11Renderer.hpp"
#include <assert.h>

D3D11Renderer::D3D11Renderer(unsigned int winWidth, unsigned int winHeight)
{
    mWinWidth = winWidth;
    mWinHeight = winHeight;
    mClose = false;

    // Window.
    InitialiseGLFW();

    // Vulkan.
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

    return !glfwWindowShouldClose(mWindow);
}

void D3D11Renderer::Close()
{
    mClose = true;
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
    mWindow = glfwCreateWindow(mWinWidth, mWinHeight, "Vulkan window", NULL, NULL);
}

void D3D11Renderer::DeInitialiseGLFW()
{
    glfwTerminate();
}

void D3D11Renderer::InitialiseD3D11()
{

}

void D3D11Renderer::DeInitialiseD3D11()
{

}
