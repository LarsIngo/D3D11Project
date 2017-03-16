#pragma once

#include <d3d11.h>

class Scene;
class StorageBuffer;
class FrameBuffer;

class ParticleSystem
{
    public:
        // Constructor.
        // pDevice Pointer to D3D11 device.
        // pDeviceContext Pointer to D3D11 device context.
        ParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

        // Destructor.
        ~ParticleSystem();

        // Update particles.
        // scene Scene to update.
        void Update(Scene* scene);

        // Render particles.
        // scene Scene to render.
        // frameBuffer Frame buffer to render.
        void Render(Scene* scene, FrameBuffer* frameBuffer);

    private:
        ID3D11Device* mpDevice;
        ID3D11DeviceContext* mpDeviceContext;

        ID3D11ComputeShader* mComputeShader;

        ID3D11VertexShader* mVertexShader;
        ID3D11GeometryShader* mGeometryShader;
        ID3D11PixelShader* mPixelShader;
        ID3D11BlendState* mBlendState;
};
