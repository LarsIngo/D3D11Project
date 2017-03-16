#pragma once

#include <d3d11.h>

// D3D11 timer.
class D3D11Timer {
    public:
        // Constructor.
        D3D11Timer(float& dt, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
        {
            mDt = &dt;
        }

        // Destructor.
        ~D3D11Timer()
        {
            float deltaTime = 0;
            *mDt = static_cast<float>(deltaTime) / 1000.f;
        }

    private:
        float* mDt;
};

#define D3D11TIMER(dt, device, deviceContext) D3D11Timer instance(dt, device, deviceContext)
