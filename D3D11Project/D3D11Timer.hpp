#pragma once

#include <d3d11.h>

// D3D11 timer.
class D3D11Timer {
    public:
        // Constructor.
        D3D11Timer(float& dt, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
        {
            mDt = &dt;
            mpDevice = pDevice;
            mpDeviceContext = pDeviceContext;

            D3D11_QUERY_DESC desc;
            desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
            desc.MiscFlags = 0;
            mpDevice->CreateQuery(&desc, &mDisjoint);

            desc.Query = D3D11_QUERY_TIMESTAMP;
            mpDevice->CreateQuery(&desc, &mStart);
            mpDevice->CreateQuery(&desc, &mStop);

            // Start.
            mpDeviceContext->Begin(mDisjoint);
            mpDeviceContext->End(mStart);
        }

        // Destructor.
        ~D3D11Timer()
        {
            // Stop.
            mpDeviceContext->End(mStop);
            mpDeviceContext->End(mDisjoint);

            // Get time.
            UINT64 startTime = 0;
            while (mpDeviceContext->GetData(mStart, &startTime, sizeof(startTime), 0) != S_OK);

            UINT64 endTime = 0;
            while (mpDeviceContext->GetData(mStop, &endTime, sizeof(endTime), 0) != S_OK);

            D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
            while (mpDeviceContext->GetData(mDisjoint, &disjointData, sizeof(disjointData), 0) != S_OK);

            if (disjointData.Disjoint == FALSE)
            {
                UINT64 delta = endTime - startTime;
                double frequency = static_cast<double>(disjointData.Frequency);
                *mDt = static_cast<float>((delta / frequency));
            }

            mDisjoint->Release();
            mStart->Release();
            mStop->Release();
        }

    private:
        float* mDt;
        ID3D11Device* mpDevice;
        ID3D11DeviceContext* mpDeviceContext;
        ID3D11Query* mDisjoint;
        ID3D11Query* mStart;
        ID3D11Query* mStop;
};

#define D3D11TIMER(dt, pDevice, pDeviceContext) D3D11Timer d3d11Timer(dt, pDevice, pDeviceContext)
