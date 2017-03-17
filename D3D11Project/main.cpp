#include <crtdbg.h>
#include <iostream>

#include "D3D11Renderer.hpp"
#include "CPUTimer.hpp"
#include "FrameBuffer.hpp"
#include "ParticleSystem.hpp"
#include "Scene.hpp"

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

    ParticleSystem particleSystem(device, deviceContext);

    Scene scene(device, deviceContext);
    {
        Particle particle;
        particle.position = glm::vec4(-0.5f, -0.5f, 0.f, 0.f);
        particle.color = glm::vec4(0.f, 0.7f, 0.f, 1.f);
        std::vector<Particle> particleList;
        particleList.push_back(particle);
        scene.AddParticles(particleList);
    }
    // --- INIT --- //

    // +++ MAIN LOOP +++ //
    float dt = 0.f;
    while (renderer.Running())
    {
        std::cout << "CPU TIMER: " << dt << std::endl;
        CPUTIMER(dt);
        // +++ UPDATE +++ //
        particleSystem.Update(&scene);
        // --- UPDATE --- //

        // +++ RENDER +++ //
        frameBuffer.ClearAll(0.2f, 0.2f, 0.2f);

        particleSystem.Render(&scene, &frameBuffer);
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
