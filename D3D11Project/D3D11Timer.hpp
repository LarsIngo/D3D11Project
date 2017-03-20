#pragma once

#include <d3d11.h>
#include <assert.h>

// D3D11 timer.
class D3D11Timer {
    public:
        // Constructor.
        D3D11Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
        {
            mpDevice = pDevice;
            mpDeviceContext = pDeviceContext;
            mActive = false;

            D3D11_QUERY_DESC desc;
            desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
            desc.MiscFlags = 0;
            mpDevice->CreateQuery(&desc, &mDisjoint);

            desc.Query = D3D11_QUERY_TIMESTAMP;
            mpDevice->CreateQuery(&desc, &mStart);
            mpDevice->CreateQuery(&desc, &mStop);
        }

        // Destructor.
        ~D3D11Timer()
        {
            mDisjoint->Release();
            mStart->Release();
            mStop->Release();
        }

        // Start timestamp.
        void Start()
        {
            assert(!mActive);
            mActive = true;

            mpDeviceContext->Begin(mDisjoint);
            mpDeviceContext->End(mStart);
        }

        // Stop timestamp.
        void Stop()
        {
            assert(mActive);
            mActive = false;

            mpDeviceContext->End(mStop);
            mpDeviceContext->End(mDisjoint);
        }

        // Get time from start to stop in seconds.
        float GetTime()
        {
            UINT64 startTime = 0;
            while (mpDeviceContext->GetData(mStart, &startTime, sizeof(startTime), 0) != S_OK);

            UINT64 endTime = 0;
            while (mpDeviceContext->GetData(mStop, &endTime, sizeof(endTime), 0) != S_OK);

            D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
            while (mpDeviceContext->GetData(mDisjoint, &disjointData, sizeof(disjointData), 0) != S_OK);

            assert(disjointData.Disjoint == FALSE);
            if (disjointData.Disjoint == FALSE)
            {
                UINT64 delta = endTime - startTime;
                double frequency = static_cast<double>(disjointData.Frequency);
                return static_cast<float>((delta / frequency));
            }

            return 0.f;
        }

        // Whether timer is active.
        bool IsActive()
        {
            return mActive;
        }

    private:
        ID3D11Device* mpDevice;
        ID3D11DeviceContext* mpDeviceContext;
        ID3D11Query* mDisjoint;
        ID3D11Query* mStart;
        ID3D11Query* mStop;
        bool mActive;
};
