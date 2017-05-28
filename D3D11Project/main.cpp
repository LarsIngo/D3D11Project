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

#define SKIP_TIME_NANO 5000000000

#define PROFILE_FRAME_COUNT 1000

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

    int lenX = 1;
    int lenY = 1;
    Scene scene(device, deviceContext, lenX * lenY);
    {
        std::vector<Particle> particleList;
        Particle particle;
        float spacing = 1.f;
        float speed = 0.1f;
        particle.scale = glm::vec4(spacing * 0.75f, spacing * 0.75f, 0.f, 0.f);
        for (int y = 0; y < lenY; ++y)
        {
            for (int x = 0; x < lenX; ++x)
            {
                particle.position = glm::vec4(x * spacing, y * spacing, 0.f, 0.f);
                //particle.velocity = -glm::normalize(particle.position + glm::vec4(speed, speed, 0.f, 0.f));
                particle.velocity = glm::vec4(0.f, 0.f, 0.f, 0.f);
                particle.color = glm::vec4((float)y / lenY, 0.7f, 1.f - (float)x / lenX, 1.f);
                particleList.push_back(particle);
            }
        }
        scene.AddParticles(particleList);

        camera.mPosition.x = (lenX - 1) / 2.f * spacing;
        camera.mPosition.y = (lenY - 1) / 2.f * spacing;
        camera.mPosition.z = -50.f;
    }
    // --- INIT --- //

    // +++ MAIN LOOP +++ //
    {
        double startTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double currentTime = 0.0;
        double totalTime = 0.0;
        double totalMeasureTime = 0.0;
        float mt = 0.f;
        float dt = 0.f;
        double profileFrames[PROFILE_FRAME_COUNT];
        for (int i = 0; i < PROFILE_FRAME_COUNT; ++i)
            profileFrames[i] = 0.0;
        double averageTime = 0.0;

        std::cout << "+++ Skip time: " << SKIP_TIME_NANO << " nanoseconds. (Wait for program to stabilize) +++" << std::endl;
        std::cout << "Hold F1 to sync compute/graphics. " << std::endl;
        std::cout << "Hold F2 to profile. " << std::endl;
        std::cout << "Hold F3 to show average frame time. " << std::endl;
        unsigned int frameCount = 0;
        D3D11Timer gpuComputeTimer(device, deviceContext);
        D3D11Timer gpuGraphicsTimer(device, deviceContext);
        Profiler profiler(1600, 200);
        while (renderer.Running())
        {
            //glm::clamp(dt, 1.f / 6000.f, 1.f / 60.f);
            bool syncComputeGraphics = inputManager.KeyPressed(GLFW_KEY_F1);
            //bool gpuProfile = inputManager.KeyPressed(GLFW_KEY_F2);
            {
                double lastTime = currentTime;
                currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                dt = (currentTime - lastTime) / 1000000000;
                totalTime = currentTime - startTime;

                CPUTIMER(mt);
                // +++ UPDATE +++ //
                if (totalTime > SKIP_TIME_NANO) gpuComputeTimer.Start();
                camera.Update(20.f, 2.f, dt, &inputManager);
                particleSystem.Update(&scene, dt);
                if (totalTime > SKIP_TIME_NANO)
                {
                    gpuComputeTimer.Stop();
                    // SYNC_COMPUTE_GRAPHICS
                    if (syncComputeGraphics) gpuComputeTimer.CalculateTime();
                }
                // --- UPDATE --- //

                // +++ RENDER +++ //
                if (totalTime > SKIP_TIME_NANO) gpuGraphicsTimer.Start();
                camera.mpFrameBuffer->Clear(0.2f, 0.2f, 0.2f);
                particleSystem.Render(&scene, &camera);
                if (totalTime > SKIP_TIME_NANO) gpuGraphicsTimer.Stop();
                // --- RENDER --- //

                
                if (totalTime > SKIP_TIME_NANO)
                {
                    // Wait on CPU for compute and graphics to complete.
                    gpuComputeTimer.CalculateTime();
                    gpuGraphicsTimer.CalculateTime();
                }
            }

            // +++ PRESENET +++ //
            renderer.Present(camera.mpFrameBuffer);
            // --- PRESENET --- //

            // +++ PROFILING +++ //
            if (totalTime > SKIP_TIME_NANO)
            {
                if (frameCount == 0)
                    std::cout << "--- Skip time over --- " << std::endl << std::endl;

                totalMeasureTime += mt;
                ++frameCount;

                if (inputManager.KeyPressed(GLFW_KEY_F2))
                {
                    float computeTime = 1.f / 1000000.f * gpuComputeTimer.GetDeltaTime();
                    float graphicsTime = 1.f / 1000000.f * gpuGraphicsTimer.GetDeltaTime();
                    std::cout << "GPU(Total) : " << computeTime + graphicsTime << " ms | GPU(Compute): " << computeTime << " ms | GPU(Graphics) : " << graphicsTime << " ms" << std::endl;
                    profiler.Rectangle(gpuComputeTimer.GetBeginTime(), 1, gpuComputeTimer.GetDeltaTime(), 1, 0.f, 0.f, 1.f);
                    profiler.Rectangle(gpuGraphicsTimer.GetBeginTime(), 0, gpuGraphicsTimer.GetDeltaTime(), 1, 0.f, 1.f, 0.f);
                    profiler.Point(gpuGraphicsTimer.GetBeginTime(), totalMeasureTime / frameCount, syncComputeGraphics ? "'-ro'" : "'-bo'");
                }

                // CALCULATE AVERAGE FRAME TIME OF LAST NUMBER OF FRAMES
                averageTime -= profileFrames[frameCount % PROFILE_FRAME_COUNT];
                profileFrames[frameCount % PROFILE_FRAME_COUNT] = mt;
                averageTime += mt;

                if (inputManager.KeyPressed(GLFW_KEY_F3))
                {
                    std::cout << "CPU(Average delta time of last " << PROFILE_FRAME_COUNT << " frames) : " << averageTime / PROFILE_FRAME_COUNT / 1000000 << " ms : FrameCount: " << frameCount << std::endl;
                }
            }
            // --- PROFILING --- //
        }
    }
    // --- MAIN LOOP --- //

    // +++ SHUTDOWN +++ //
    // --- SHUTDOWN --- //

    return 0;
}
