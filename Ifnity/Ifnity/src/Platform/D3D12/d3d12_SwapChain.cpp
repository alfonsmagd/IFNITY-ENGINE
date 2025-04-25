


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
            hr = swapchain_->GetBuffer(i, IID_PPV_ARGS(&buffer));
            assert(SUCCEEDED(hr));

            backBuffers_[i] = buffer;

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

            backBufferHandles_[i] = ctx_.slotMapTextures_.create(std::move(image));
        }

	
	
	
	
	
	
	
	
	
	
	
	
	
	}


	D3D12Swapchain::~D3D12Swapchain()
	{}

	void D3D12Swapchain::present()
	{}

	TextureHandleSM D3D12Swapchain::getCurrentTexture()
	{
		return TextureHandleSM();
	}

	uint32_t D3D12Swapchain::getCurrentBackBufferIndex() const
	{
		return 0;
	}

	DXGI_FORMAT D3D12Swapchain::getSurfaceFormat() const
	{
		return DXGI_FORMAT();
	}

	void D3D12Swapchain::createSwapchain(HWND hwnd)
	{}

	void D3D12Swapchain::createBackbuffers()
	{}

}



IFNITY_END_NAMESPACE