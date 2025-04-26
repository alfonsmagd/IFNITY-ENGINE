


#include "d3d12_SwapChain.hpp"
#include "../Windows/DeviceD3D12.h"
#include "d3d12_Image.hpp"

IFNITY_NAMESPACE

namespace D3D12
{



	D3D12Swapchain::D3D12Swapchain(DeviceD3D12& ctx, HWND hwnd, uint32_t width, uint32_t height):
		ctx_(ctx),
		width_(width),
		height_(height)
	{
        // 1. Describe the swapchain using modern flip model
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = width_;
        swapChainDesc.Height = height_;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Default color format
        swapChainDesc.BufferCount = MAX_SWAPCHAIN_IMAGES;  // Triple buffering recommended
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
        swapChainDesc.Stereo = FALSE;

        // 2. Create a temporary IDXGISwapChain1
        ComPtr<IDXGISwapChain1> tempSwapchain;
        HRESULT hr = ctx_.dxgiFactory4->CreateSwapChainForHwnd(
            ctx_.commandQueue.Get(),  // Command Queue
            hwnd,                    // Window handle
            &swapChainDesc,           // Swapchain description
            nullptr,                 // Fullscreen description (not used)
            nullptr,                 // Output restriction (not used)
            &tempSwapchain
        );
        assert(SUCCEEDED(hr));

        // 3. Cast to IDXGISwapChain3
        hr = tempSwapchain.As(&swapchain_);
        assert(SUCCEEDED(hr));

        // 4. Save swapchain properties
        surfaceFormat_ = swapChainDesc.Format;
        numSwapchainImages_ = swapChainDesc.BufferCount;
        currentBackBufferIndex_ = swapchain_->GetCurrentBackBufferIndex();

        // 5. Acquire backbuffers and wrap them into D3D12Image structures
        for (uint32_t i = 0; i < numSwapchainImages_; ++i)
        {
            ComPtr<ID3D12Resource> buffer;
            hr = swapchain_->GetBuffer(i, IID_PPV_ARGS(OUT &buffer));
            assert(SUCCEEDED(hr));

			// 5.1. Store the backbuffer in the array
            backBuffers_[i] = buffer;
            
            // 5.2. Allocate an RTV descriptor handle from the descriptor heap
            rtvHandles_[i] = ctx_.AllocateRTV(); 

            // 5.3. Create RTV for the backbuffer
            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
            rtvDesc.Format = surfaceFormat_;
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = 0;
            rtvDesc.Texture2D.PlaneSlice = 0;

            ctx_.m_Device->CreateRenderTargetView(
                buffer.Get(),      // Resource
                &rtvDesc,          // RTV description
                rtvHandles_[i]     // Descriptor handle
            );

            // 5.4. Create and store D3D12Image
            D3D12Image image = {};
            image.resource_ = buffer;
            image.usageFlags_ = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            image.currentState_ = D3D12_RESOURCE_STATE_PRESENT;
            image.format_ = surfaceFormat_;
            image.type_ = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            image.width_ = width_;
            image.height_ = height_;
            image.depthOrArraySize_ = 1;
            image.mipLevels_ = 1;
            image.isSwapchainImage_ = true;
            image.isDepthFormat_ = D3D12Image::isDepthFormat(surfaceFormat_);
            image.isStencilFormat_ = D3D12Image::isDepthStencilFormat(surfaceFormat_);
            image.rtvHandle_ = rtvHandles_[i]; //store the RTV in the D3D12Image itself

            backBufferHandles_[i] = ctx_.slotMapTextures_.create(std::move(image));
        }
	}


	D3D12Swapchain::~D3D12Swapchain()
	{
        // Free handles in slotmpa 
        for (uint32_t i = 0; i < numSwapchainImages_; ++i)
        {
            if (backBufferHandles_[i].valid())
            {
                ctx_.slotMapTextures_.destroy(backBufferHandles_[i]);
            }
        }
    
        // Los ComPtr automatic free;
        // - swapchain_
        // - backBuffers_[i]
    
    }

	void D3D12Swapchain::present()
	{
    
        // 1. Present the swapchain
        HRESULT hr = swapchain_->Present(checkVSyncEnabled() ? 1 : 0, 0); // 1 = VSync ON, 0 = no special flags

        if (FAILED(hr))
        {
            IFNITY_LOG(LogCore, ERROR, "Swapchain Present failed!");

            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {
                // Device lost, needs recreation
                IFNITY_LOG(LogCore, ERROR, "Device lost! Need to recreate device and swapchain.");
                // TODO: Señalar al motor que debe recrear device/swapchain.
            }
            else
            {
                assert(false && "Swapchain Present failed unexpectedly");
            }
        }

        // 2. Update the current back buffer index
        currentBackBufferIndex_ = swapchain_->GetCurrentBackBufferIndex();
    
    }

   

	TextureHandleSM D3D12Swapchain::getCurrentTexture()
	{
        currentBackBufferIndex_ = swapchain_->GetCurrentBackBufferIndex();
        if (currentBackBufferIndex_ < numSwapchainImages_)
        {
            return backBufferHandles_[currentBackBufferIndex_];
        }
        return {}; // return invalid handle if something wrong
    
	}

	uint32_t D3D12Swapchain::getCurrentBackBufferIndex() const
	{
        return currentBackBufferIndex_;
	}

	DXGI_FORMAT D3D12Swapchain::getSurfaceFormat() const
	{
		return surfaceFormat_;
	}

	void D3D12Swapchain::createSwapchain(HWND hwnd)
	{}

	void D3D12Swapchain::createBackbuffers()
	{}

    bool D3D12Swapchain::checkVSyncEnabled() const
    {
		return forcevsync_;
    }

}



IFNITY_END_NAMESPACE