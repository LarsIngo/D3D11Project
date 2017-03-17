#pragma once

#define MAX_PARTICLE_COUNT 1048576U

#include <vector>
#include "Particle.hpp"
#include <d3d11.h>

class StorageSwapBuffer;
class ParticleSystem;

class Scene
{
    friend ParticleSystem;

    public:
        // Constructor.
        // pDevice Pointer to D3D11 device.
        // pDeviceContext Pointer to D3D11 device context.
        Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

        // Destructor.
        ~Scene();

        // Adds partilces to scene.
        // particleList Vector of particles to add.
        void AddParticles(std::vector<Particle>& particleList);

    private:
        unsigned int mParticleCount;
        StorageSwapBuffer* mParticleBuffer;

        ID3D11Device* mpDevice;
        ID3D11DeviceContext* mpDeviceContext;
};
