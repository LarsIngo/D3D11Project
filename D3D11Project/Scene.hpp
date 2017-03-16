#pragma once

#include <vector>
#include "Particle.hpp"
#include <d3d11.h>

class StorageBuffer;
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
        const unsigned int mMaxParticleCount = 1024;
        unsigned int mParticleCount = 0;
        StorageBuffer* mStorageBuffer;

        ID3D11Device* mpDevice;
        ID3D11DeviceContext* mpDeviceContext;
};
