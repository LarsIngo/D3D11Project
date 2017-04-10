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
#include "Profiler.hpp"

//#define SYNC_COMPUTE_GRAPHICS

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // +++ INIT +++ //
    unsigned int width = 1920 / 2;
    unsigned int height = 1080 / 2;
    D3D11Renderer renderer(width, height);
    ID3D11Device* device = renderer.mDevice;
    ID3D11DeviceContext* deviceContext = renderer.mDeviceContext;

    ParticleSystem particleSystem(device, deviceContext);
    
    InputManager inputManager(renderer.mGLFWwindow);

    FrameBuffer frameBuffer(device, deviceContext, width, height);
    Camera camera(60.f, &frameBuffer);
    camera.mPosition.z = -5.f;

    int lenX = 256;
    int lenY = 256;
    Scene scene(device, deviceContext, lenX * lenY);
    {
        std::vector<Particle> particleList;
        Particle particle;
        float spaceing = 1.f;
        float speed = 0.1f;
        particle.scale = glm::vec4(spaceing / 2.f, spaceing / 2.f, 0.f, 0.f);
        for (int y = 0; y < lenY; ++y)
        {
            for (int x = 0; x < lenX; ++x)
            {
                particle.position = glm::vec4(x * spaceing, y * spaceing, 0.f, 0.f);
                particle.velocity = -glm::normalize(particle.position + glm::vec4(speed, speed, 0.f, 0.f));
                particle.color = glm::vec4((float)y / lenY, 0.7f, 1.f - (float)x / lenX, 1.f);
                particleList.push_back(particle);
            }
        }
        scene.AddParticles(particleList);
    }
    // --- INIT --- //

    // +++ MAIN LOOP +++ //
    {
        float dt = 0.f;
        float totalTime = 0.f;
        unsigned int frameCount = 0;
        D3D11Timer gpuComputeTimer(device, deviceContext);
        D3D11Timer gpuGraphicsTimer(device, deviceContext);
        Profiler profiler(1600, 200);
        while (renderer.Running())
        {
            //glm::clamp(dt, 1.f / 6000.f, 1.f / 60.f);
            bool cpuProfile = inputManager.KeyPressed(GLFW_KEY_F1);
            bool gpuProfile = inputManager.KeyPressed(GLFW_KEY_F2);
            {
                CPUTIMER(dt);
                // +++ UPDATE +++ //
                if (gpuProfile) gpuComputeTimer.Start();
                camera.Update(20.f, 2.f, dt, &inputManager);
                particleSystem.Update(&scene, dt);
                if (gpuProfile)
                {
                    gpuComputeTimer.Stop();
#ifdef SYNC_COMPUTE_GRAPHICS
                    gpuComputeTimer.CalculateTime();
#endif
                }
                // --- UPDATE --- //

                // +++ RENDER +++ //
                if (gpuProfile) gpuGraphicsTimer.Start();
                camera.mpFrameBuffer->Clear(0.2f, 0.2f, 0.2f);
                particleSystem.Render(&scene, &camera);
                if (gpuProfile)
                {
                    gpuGraphicsTimer.Stop();
#ifdef SYNC_COMPUTE_GRAPHICS
                    gpuGraphicsTimer.CalculateTime();
#endif
                }
                // --- RENDER --- //

                // +++ PRESENET +++ //
                renderer.Present(camera.mpFrameBuffer);
                // --- PRESENET --- //
            }
            // +++ PROFILING +++ //
            ++frameCount;
            totalTime += dt;
            if (cpuProfile)
            {
                std::cout << "CPU(Delta time): " << 1000.f * dt << " ms | FPS: " << 1.f / dt << std::endl;
            }
            if (gpuProfile)
            {
                float computeTime = 1.f / 1000000.f * gpuComputeTimer.GetDeltaTime();
                float graphicsTime = 1.f / 1000000.f * gpuGraphicsTimer.GetDeltaTime();
                std::cout << "GPU(Total) : " << computeTime + graphicsTime << " ms | GPU(Compute): " << computeTime << " ms | GPU(Graphics) : " << graphicsTime << " ms" << std::endl;
                profiler.Rectangle(gpuComputeTimer.GetBeginTime(), 1, gpuComputeTimer.GetDeltaTime(), 1, 0.f, 0.f, 1.f);
                profiler.Rectangle(gpuGraphicsTimer.GetBeginTime(), 0, gpuGraphicsTimer.GetDeltaTime(), 1, 0.f, 1.f, 0.f);
            }
            if (inputManager.KeyPressed(GLFW_KEY_F3))
            {
                std::cout << "CPU(Average delta time) : " << totalTime / frameCount * 1000.f << " ms" << std::endl;
            }
            // --- PROFILING --- //
        }
    }
    // --- MAIN LOOP --- //

    // +++ SHUTDOWN +++ //
    // --- SHUTDOWN --- //

    return 0;
}
