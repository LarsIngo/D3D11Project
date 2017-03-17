#include <crtdbg.h>
#include <iostream>

#include "D3D11Renderer.hpp"
#include "CPUTimer.hpp"
#include "D3D11Timer.hpp"
#include "FrameBuffer.hpp"
#include "ParticleSystem.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "InputManager.hpp"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // +++ INIT +++ //
    unsigned int width = 800;
    unsigned int height = 600;
    D3D11Renderer renderer(width, height);
    ID3D11Device* device = renderer.mDevice;
    ID3D11DeviceContext* deviceContext = renderer.mDeviceContext;

    ParticleSystem particleSystem(device, deviceContext);
    
    InputManager inputManager(renderer.mGLFWwindow);

    FrameBuffer frameBuffer(device, deviceContext, width, height);
    Camera camera(60.f, &frameBuffer);
    camera.mPosition.z = -5.f;

    Scene scene(device, deviceContext);
    {
        std::vector<Particle> particleList;
        Particle particle;
        float spaceing = 1.f;
        float speed = 0.1f;
        particle.scale = glm::vec4(spaceing / 2.f, spaceing / 2.f, 0.f, 0.f);
        for (int y = 0; y < 1024; ++y)
        {
            for (int x = 0; x < 1024; ++x)
            {
                particle.position = glm::vec4(x * spaceing, y * spaceing, 0.f, 0.f);
                particle.velocity = -glm::normalize(particle.position + glm::vec4(speed, speed, 0.f, 0.f));
                particle.color = glm::vec4(y / 1024.f, 0.7f, 1.f - x / 1024.f, 1.f);
                particleList.push_back(particle);
            }
        }
        scene.AddParticles(particleList);
    }
    // --- INIT --- //

    // +++ MAIN LOOP +++ //
    float dt = 1.f;
    float GPUdt = 1.f;
    while (renderer.Running())
    {
        glm::clamp(dt, 1.f / 6000.f, 1.f / 60.f);
        std::cout << "CPU TIMER: " << 1000.f * dt << " ms | FPS: " << 1.f / dt << " | GPU: " <<  1000.f * GPUdt << " ms" << std::endl;
        CPUTIMER(dt);
        D3D11TIMER(GPUdt, device, deviceContext);
        // +++ UPDATE +++ //
        camera.Update(20.f, 2000.f, dt, &inputManager);
        particleSystem.Update(&scene, dt);
        // --- UPDATE --- //

        // +++ RENDER +++ //
        camera.mpFrameBuffer->ClearAll(0.2f, 0.2f, 0.2f);

        particleSystem.Render(&scene, &camera);
        // --- RENDER --- //

        // +++ PRESENET +++ //
        renderer.Present(camera.mpFrameBuffer);
        // --- PRESENET --- //
    }
    // --- MAIN LOOP --- //

    // +++ SHUTDOWN +++ //
    // --- SHUTDOWN --- //

    return 0;
}
