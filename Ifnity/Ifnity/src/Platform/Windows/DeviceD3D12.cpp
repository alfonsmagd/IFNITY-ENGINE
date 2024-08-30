#include "DeviceD3D12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
IFNITY_NAMESPACE
DeviceD3D12::~DeviceD3D12()
{
}



void DeviceD3D12::OnUpdate()
{
}

void DeviceD3D12::RenderDemo(int w, int h) const
{
}

void DeviceD3D12::SetVSync(bool enabled)
{
}

bool DeviceD3D12::IsVSync() const
{
	return false;
}

bool DeviceD3D12::InitInternalInstance()
{
	UINT debugFlags = 0;

	// Enable the D3D12 debug layer.
	if (m_DeviceParams.enableDebugRuntime)
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(OUT &debugController)));
		debugController->EnableDebugLayer();
		debugFlags = DXGI_CREATE_FACTORY_DEBUG;
		IFNITY_LOG(LogCore, TRACE, "Enable Debug Layer D3D12");
	}
	//Build the DXGI Factory
	if (!m_DxgiFactory)
	{
		HRESULT hres = CreateDXGIFactory2(debugFlags, IID_PPV_ARGS(OUT &m_DxgiFactory));
		if (hres != S_OK)
		{
			IFNITY_LOG(LogCore, ERROR, "ERROR in CreateDXGIFactory2.\n"
				"For more info, get log from debug D3D runtime: (1) Install DX SDK, and enable Debug D3D from DX Control Panel Utility. (2) Install and start DbgView. (3) Try running the program again.\n");
			return false;
		}
	}

	return true;
}

bool DeviceD3D12::ConfigureSpecificHintsGLFW() const
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	return true;
	
}

bool DeviceD3D12::InitializeDeviceAndContext()
{
	
	int adapterIndex = 0;
	//Enumerate the adapters and select the first one, normally the primary adapter is Hardware Device.
	if (FAILED(m_DxgiFactory->EnumAdapters(adapterIndex, OUT &m_DxgiAdapter)))
	{
		if (adapterIndex == 0)
			IFNITY_LOG(LogCore, ERROR, "Cannot find any DXGI adapters in the system. D3D12");
		else
			IFNITY_LOG(LogCore, ERROR, "The specified DXGI adapter %d does not exist. D3D12", adapterIndex);

		return false;
	}

	{
		DXGI_ADAPTER_DESC aDesc;
		m_DxgiAdapter->GetDesc(&aDesc);
		m_IsNvidia = rhi::IsNvDeviceID(aDesc.VendorId);
	}

	//Create the D3D12 device
	HRESULT hr = D3D12CreateDevice(
		IN m_DxgiAdapter.Get(),
		m_DeviceParams.featureLevel,
		IID_PPV_ARGS(OUT &m_Device));

	if (FAILED(hr))
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create D3D12 device.");
		return false;
	}

	//Check if the debug layer when is enabled
	if (m_DeviceParams.enableDebugRuntime)
	{
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		m_Device->QueryInterface(IID_PPV_ARGS(OUT &pInfoQueue));

		if (pInfoQueue)
		{
#ifdef _DEBUG
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
#endif

			D3D12_MESSAGE_ID disableMessageIDs[] = {
				D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH, // descriptor validation doesn't understand acceleration structures
			};

			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.pIDList = disableMessageIDs;
			filter.DenyList.NumIDs = sizeof(disableMessageIDs) / sizeof(disableMessageIDs[0]);
			pInfoQueue->AddStorageFilterEntries(&filter);
		}
	}

#ifdef _DEBUG
	LogAdaptersD3D12();
#endif


	// Create Fence and obtain descriptor sizes.
	CreateFenceAndDescriptorSizes();
	// Create CommandQueue 
	CreateCommandQueue();
	//Create SwapChain
	CreateSwapChain();

	return true;
}

void DeviceD3D12::ResizeSwapChain()
{
}

void DeviceD3D12::InitializeGui()
{
}
void DeviceD3D12::LogAdaptersD3D12()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (m_DxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);
		std::wstring text = L"Adapter: ";
		text += desc.Description;
		text += L"\n";
		OutputDebugString(text.c_str());
		adapterList.push_back(adapter);
		++i;
	}
	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		DXGI_ADAPTER_DESC desc;
		adapterList[i]->GetDesc(&desc);
		std::wstring adapterDescription = desc.Description;
		//Transform wstrint to string. Fix non void pointer error.
		std::string adapterDescriptionStr(adapterDescription.begin(), adapterDescription.end());
		m_IsNvidia = rhi::IsNvDeviceID(desc.VendorId);

		IFNITY_LOG(LogCore, INFO, "Adapter Description  " + adapterDescriptionStr + "  N Adapter :  " + std::to_string(i));
		adapterList[i]->Release();
	}


}

void DeviceD3D12::CreateFenceAndDescriptorSizes()
{
	// Create Fence.
	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(OUT & m_Fence)));

	m_DescritporSizes.Rtv = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DescritporSizes.Dsv = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_DescritporSizes.CbvSrvUav = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

UINT DeviceD3D12::CheckMSAAQualitySupport(UINT SampleCount, DXGI_FORMAT format)
{
	// Check SampleCountX MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = format;
	msQualityLevels.SampleCount = SampleCount;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_Device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		OUT &msQualityLevels,
		sizeof(msQualityLevels)));

	assert(msQualityLevels.NumQualityLevels > 0 && "Unexpected MSAA quality level.");
	IFNITY_LOG(LogCore, INFO, "MSAA Quality Level: " + std::to_string(msQualityLevels.NumQualityLevels));
	 
	return msQualityLevels.NumQualityLevels;
}
bool DeviceD3D12::CreateSwapChain()
{
	// Obtener el handle de la ventana desde GLFW
	m_hWnd = glfwGetWin32Window(m_Window);


	// Describe and create the swap chain.
	DXGI_MODE_DESC bufferDesc = {};
	bufferDesc.Width  =                                  GetWidth();
	bufferDesc.Height =								     GetHeight();
	bufferDesc.RefreshRate.Numerator =					 60;
	bufferDesc.RefreshRate.Denominator =				 1;
	bufferDesc.Format =									 DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering =						 DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling =								 DXGI_MODE_SCALING_UNSPECIFIED;
														 
	DXGI_SAMPLE_DESC sampleDesc = {};					 
	sampleDesc.Count =									 m_MsaaState ? 4 : 1; // Activate MSSA 4X , by default is false. 												   
	sampleDesc.Quality =								CheckMSAAQualitySupport(sampleDesc.Count,
																				bufferDesc.Format) - 1; // Its importa													to substract 1 because the quality level is 0 based.

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc =                                     bufferDesc;
	sd.SampleDesc =                                     sampleDesc;
	sd.BufferUsage =                                    DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount =                                    2;
	sd.OutputWindow =                                   m_hWnd;
	sd.Windowed =                                       true;
	sd.SwapEffect =                                     DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags =                                          DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain> swapChain;
	ThrowIfFailed(m_DxgiFactory->CreateSwapChain(
		m_CommandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
		&sd,
		&swapChain
	));

	ThrowIfFailed(swapChain.As(&m_SwapChain));

	return true;




}
void DeviceD3D12::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;  // Direct command list can execute all grapchis commands.
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

	ThrowIfFailed(m_Device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_DirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(m_Device->CreateCommandList(
		0,							  // Predeterminate value Priority Level. 
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_DirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(m_CommandList.GetAddressOf())));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	m_CommandList->Close();






}
IFNITY_END_NAMESPACE