#include "Scene.hpp"
#include "StorageBuffer.hpp"

Scene::Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    mpDevice = pDevice;
    mpDeviceContext = pDeviceContext;

    mStorageBuffer = new StorageBuffer(mpDevice, mpDeviceContext, sizeof(Particle) * mMaxParticleCount, sizeof(Particle));
}

Scene::~Scene()
{
    delete mStorageBuffer;
}

void Scene::AddParticles(std::vector<Particle>& particleList)
{
    unsigned int offset = mParticleCount * sizeof(Particle);
    unsigned int particleCount = (unsigned int)particleList.size();
    unsigned int bytes = particleCount * sizeof(Particle);

    mStorageBuffer->Write(particleList.data(), bytes, offset);

    mParticleCount += particleCount;
}
