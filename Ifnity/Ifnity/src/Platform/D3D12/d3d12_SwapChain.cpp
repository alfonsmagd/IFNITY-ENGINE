


#include "d3d12_SwapChain.hpp"
#include "../Windows/DeviceD3D12.h"

IFNITY_NAMESPACE

namespace D3D12
{



	D3D12Swapchain::D3D12Swapchain(DeviceD3D12& ctx, HWND hwnd, uint32_t width, uint32_t height):
		ctx_(ctx),
		width_(width),
		height_(height)
	{}


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