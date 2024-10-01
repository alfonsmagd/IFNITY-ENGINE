#include "DeviceD3D11.h"
#include "Platform/ImguiRender/ImguiD3D11Render.h"

IFNITY_NAMESPACE


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

using namespace DirectX;
DeviceD3D11::~DeviceD3D11()
{
	m_ImmediateContext->Flush();
	m_SwapChain.Reset();
	//Destroy all elements 
	DestroySwapChainResources();
	m_DxgiFactory.Reset();
	m_DxgiAdapter.Reset();
	m_Device.Reset();
	IFNITY_LOG(LogApp, WARNING, "DeviceD3D11 destroyed");
}



void DeviceD3D11::OnUpdate()
{
	
	m_SwapChain->Present(IsVSync() ? 1 : 0, 0);
	//Swap the buffers
	
	glfwPollEvents();

	// TODO: NOT DO THIS HERE, NOW ITS A TEST CODE SOLUTION, BUT ALL ITS NEED WHEN RESIZE EVENTS ITS TRIGGERED;
	ResizeSwapChain();


}

void DeviceD3D11::RenderDemo(int w, int h) const
{
	m_ImmediateContext->ClearRenderTargetView(m_RenderTarget.Get(), Colors::Azure); // Clear the back buffer to a 

	D3D11_VIEWPORT vp{};
	vp.Width = static_cast<float>(w);
	vp.Height = static_cast<float>(h);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;

	m_ImmediateContext->RSSetViewports(1, &vp); // Set the viewport 
	m_ImmediateContext->OMSetRenderTargets(1, m_RenderTarget.GetAddressOf(), nullptr); // depth stencil view not used nullptr.

	



}

void DeviceD3D11::SetVSync(bool enabled)
{}

bool DeviceD3D11::IsVSync() const
{
	return false;
}

bool DeviceD3D11::InitInternalInstance()
{
	if ( !m_DxgiFactory )
	{
		HRESULT hres = CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory));
		if ( hres != S_OK )
		{
			IFNITY_LOG(LogCore, ERROR, "ERROR in CreateDXGIFactory1.\n"
				"For more info, get log from debug D3D runtime: (1) Install DX SDK, and enable Debug D3D from DX Control Panel Utility. (2) Install and start DbgView. (3) Try running the program again.\n");
			return false;
		}
	}

	return true;

}

bool DeviceD3D11::ConfigureSpecificHintsGLFW() const
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	return true;
}

bool DeviceD3D11::InitializeDeviceAndContext()
{
	int adapterIndex = 0;

	if ( FAILED(m_DxgiFactory->EnumAdapters(adapterIndex, &m_DxgiAdapter)) )
	{
		if ( adapterIndex == 0 )
			IFNITY_LOG(LogCore, ERROR, "Cannot find any DXGI adapters in the system.");
		else
			IFNITY_LOG(LogCore, ERROR, "The specified DXGI adapter %d does not exist.", adapterIndex);

	}

	{
		DXGI_ADAPTER_DESC aDesc;
		m_DxgiAdapter->GetDesc(&aDesc);
		m_IsNvidia = rhi::IsNvDeviceID(aDesc.VendorId);
	}

	UINT createFlags = 0;
	if ( m_DeviceParams.enableDebugRuntime )
		createFlags |= D3D11_CREATE_DEVICE_DEBUG;

	const HRESULT hr = D3D11CreateDevice(
		m_DxgiAdapter.Get(), // pAdapter
		D3D_DRIVER_TYPE_UNKNOWN, // DriverType
		nullptr, // Software
		createFlags, // Flags
		&m_DeviceParams.featureLevel, // pFeatureLevels
		1, // FeatureLevels
		D3D11_SDK_VERSION, // SDKVersion
		&m_Device, // ppDevice
		nullptr, // pFeatureLevel
		&m_ImmediateContext // ppImmediateContext
	);

	if ( FAILED(hr) )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create D3D11 device.");
		return false;
	}

	// Call CreateSwapChain
	if ( !CreateSwapChain() )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create swap chain.");
		return false;
	}

	return true;

}

bool DeviceD3D11::CreateSwapChain()
{
	// Get the window handle
	m_hWnd = glfwGetWin32Window(m_Window);

	// Reset swap chain description struct, use DESC1 to facility the use of the new features.
	ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));

	// Set the swap chain description
	m_SwapChainDesc.Width = m_Props.Width;
	m_SwapChainDesc.Height = m_Props.Height;
	m_SwapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; // 32-bit RGBA color
	m_SwapChainDesc.Stereo = false;                // Stereo 3D disabled
	m_SwapChainDesc.SampleDesc.Count = 1;          // Don't use multi-sampling
	m_SwapChainDesc.SampleDesc.Quality = 0;        // Don't use multi-sampling
	m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use the swap chain as a render target
	m_SwapChainDesc.BufferCount = 2;               // Use double buffering to enable flip
	m_SwapChainDesc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH; // Use stretch scaling
	m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // Use flip discard to enable flip
	m_SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED; // Unspecified alpha mode
	m_SwapChainDesc.Flags = 0;                     // No flags


	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc{};
	fsSwapChainDesc.Windowed = TRUE; // Start windowed

	// Create the swap chain
	if ( FAILED(m_DxgiFactory->CreateSwapChainForHwnd(
		IN m_Device.Get(),     // Device
		IN m_hWnd,             // Window handle
		IN &m_SwapChainDesc,   // Swap chain description
		IN &fsSwapChainDesc,   // Fullscreen swap chain description
		nullptr,            // Restrict output to a single output
		OUT &m_SwapChain)) )	// Swap chain
	{
		IFNITY_LOG(LogCore, ERROR, " DXGI:Failed to create swap chain for HWND. D3D11");
		return false;
	}

	//Create SwapChain Resources
	if ( !CreateSwapChainResources() )
	{
		IFNITY_LOG(LogCore, ERROR, " DXGI:Failed to create swap chain resources. D3D11");
		return false;
	}


	IFNITY_LOG(LogCore, INFO, " DXGI:Create correctly SwapChain.");
	return true;
}

bool DeviceD3D11::CreateSwapChainResources()
{

	if ( FAILED(m_SwapChain->GetBuffer(0, OUT IID_PPV_ARGS(&m_backBuffer))) )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to get swap chain buffer D3D11.");
		return false;
	}

	// Create a render target view

	if ( !CreateRTV(m_backBuffer) )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create RTV  from back Buffer D3D11.");
		return false;
	}



	return true;
}

bool DeviceD3D11::CreateRTV(IN const ComPtr<ID3D11Texture2D>& buffer)
{

	if ( FAILED(m_Device->CreateRenderTargetView(
		IN buffer.Get(),
		nullptr,
		OUT &m_RenderTarget)) )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create RTV  from back Buffer D3D11.");
		return false;
	}
	return true;
}

void DeviceD3D11::DestroySwapChainResources()
{
	m_backBuffer.Reset();
	m_RenderTarget.Reset();

}

void DeviceD3D11::Shutdown()
{
	IFNITY_LOG(LogApp, WARNING, "Shutdown Process D3D11");
	GraphicsDeviceManager::Shutdown();
}

void DeviceD3D11::ResizeSwapChain()
{
	// Destroy the old resources
	DestroySwapChainResources();

	// Resize the swap chain
	if ( FAILED(m_SwapChain->ResizeBuffers(0,
		m_Props.Width,
		m_Props.Height,
		DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
		0)) )// NO flags
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to resize swap chain buffers D3D11.");
		return;
	}

	// Recreate the resources
	CreateSwapChainResources();



}

void DeviceD3D11::InitializeGui()
{
	ImGui_ImplDX11_Init(m_Device.Get(),m_ImmediateContext.Get());
	IFNITY_LOG(LogApp, TRACE, "Imgui API is set to D3D11");
}

void DeviceD3D11::InternalPreDestroy()
{
}



void DeviceD3D11::Init()
{}

void DeviceD3D11::InitializeD3D11()
{}

std::string DeviceD3D11::GetD3D11Info()
{
	return std::string();
}


IFNITY_END_NAMESPACE