#pragma once

#include <pch.h>
#include "d3d12_classes.hpp"



IFNITY_NAMESPACE

class DeviceD3D12;

namespace D3D12
{
	using namespace Microsoft::WRL;

    class D3D12Swapchain final
    {
        enum { MAX_SWAPCHAIN_IMAGES = 3 }; 
        // https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-multiple-swap-chains? triplebuffering in d3d12. 

    public:
        D3D12Swapchain(DeviceD3D12& ctx, HWND hwnd, uint32_t width, uint32_t height);
        ~D3D12Swapchain();

        void present();
        TextureHandleSM getCurrentTexture();

        uint32_t getCurrentBackBufferIndex() const;
        DXGI_FORMAT getSurfaceFormat() const;

        //For test purposes
		inline IDXGISwapChain3* getSwapchain() const { return swapchain_.Get(); }

    private:
        void createSwapchain(HWND hwnd);
        void createBackbuffers();
        bool checkVSyncEnabled() const;
    private:
        DeviceD3D12& ctx_;
        ComPtr<IDXGISwapChain3> swapchain_;
        ComPtr<ID3D12Resource> backBuffers_[ MAX_SWAPCHAIN_IMAGES ];
        TextureHandleSM backBufferHandles_[ MAX_SWAPCHAIN_IMAGES ];
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[ MAX_SWAPCHAIN_IMAGES ];

        uint32_t width_ = 0;
        uint32_t height_ = 0;
        uint32_t numSwapchainImages_ = 0;
        uint32_t currentBackBufferIndex_ = 0;
        DXGI_FORMAT surfaceFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM; // Formato de color

        bool forcevsync_ = false;
    };



}









IFNITY_END_NAMESPACE