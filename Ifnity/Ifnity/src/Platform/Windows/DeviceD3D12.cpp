//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-09 by alfonsmagd




#include "DeviceD3D12.hpp"
#include  "../D3D12/d3d12_classes.hpp"
#include "ShaderBuilding\ShaderBuilder.hpp"


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
IFNITY_NAMESPACE

using namespace DirectX;
void CaptureDXGIMessagesToConsole();

// depuration callbackn
// depuration callbackn
void CALLBACK DebugMessageCallback(D3D12_MESSAGE_CATEGORY Category, D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID ID, LPCSTR pDescription, void* pContext)
{
	switch( Severity )
	{
		case D3D12_MESSAGE_SEVERITY_INFO:
			IFNITY_LOG(LogCore, INFO, "D3D12 INFO: " + std::string(pDescription));
			break;
		case D3D12_MESSAGE_SEVERITY_WARNING:
			IFNITY_LOG(LogCore, WARNING, "D3D12 WARNING: " + std::string(pDescription));
			break;
		case D3D12_MESSAGE_SEVERITY_ERROR:
			IFNITY_LOG(LogCore, ERROR, "D3D12 ERROR: " + std::string(pDescription));
			break;
		case D3D12_MESSAGE_SEVERITY_CORRUPTION:
			IFNITY_LOG(LogCore, ERROR, "D3D12 CORRUPTION: " + std::string(pDescription));
			break;
		default:
			IFNITY_LOG(LogCore, TRACE, "D3D12 Message: " + std::string(pDescription));
			break;
	}
}

DeviceD3D12::~DeviceD3D12()
{


	// Check
	FlushCommandQueue();

	// Salir del modo de pantalla completa si es necesario
	BOOL fs = false;
	ThrowIfFailed(m_SwapChain->GetFullscreenState(&fs, NULL));
	if( fs )
		m_SwapChain->SetFullscreenState(false, NULL);

	// Liberar recursos en el orden correcto
	m_VsByteCode.Reset();
	m_PsByteCode.Reset();
	m_VertexBuffer.Reset();
	if( m_VertexBufferAllocation )
	{
		m_VertexBufferAllocation->Release();
		m_VertexBufferAllocation = nullptr;
	}
	m_PipelineState.Reset();
	m_RootSignature.Reset();
	CloseHandle(m_FenceEvent);
	m_CommandList.Reset();
	commandQueue.Reset();
	m_CbvSrvUavHeap.Reset();
	m_DsvHeap.Reset();
	m_DepthStencilBuffer.Reset();
	if( m_DepthStencilAllocation )
	{
		m_DepthStencilAllocation->Release();
		m_DepthStencilAllocation = nullptr;
	}
	m_RtvHeap.Reset();
	swapchain_.reset();
	for( int i = 0; i < m_SwapChainBufferCount; ++i )
	{
		if( m_SwapChainBuffer[ i ] )
		{
			m_SwapChainBuffer[ i ]->Release();
			m_SwapChainBuffer[ i ] = nullptr;
		}
	}
	m_DirectCmdListAlloc.Reset();
	m_Fence.Reset();
	g_Allocator.Reset();
	m_SwapChain.Reset();
	m_DxgiAdapter.Reset();
	dxgiFactory4.Reset();
	m_Device.Reset();



}



void DeviceD3D12::OnUpdate()
{
	using namespace D3D12;

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.

	//acquiere commandbufferlist()

	




	D3D12::CommandBuffer& cmdBuffer = acquireCommandBuffer();
	D3D12::TextureHandleSM currentTexture = swapchain_->getCurrentTexture();


	RenderPass renderPass = {
		.color = { {.loadOp = rhi::LoadOp_Clear, .clearColor = { 1.0f, 1.0f, 1.0f, 1.0f } } } };

	Framebuffer framebuffer = { .color = { {.texture = currentTexture } } };




	//auto* textback = slotMapTextures_.get(currentTexture);

	
	//cmdBuffer.cmdBeginRendering(textback);
	cmdBuffer.cmdBeginRendering(renderPass, framebuffer);
	//Demo 
	DrawElements(cmdBuffer.wrapper_->commandList.Get(), m_PipelineState, m_RootSignature);
	cmdBuffer.cmdRenderImgui(ImGui::GetDrawData(), m_CbvSrvUavHeap.Get());

	

	cmdBuffer.cmdEndRendering();
	submit(cmdBuffer, currentTexture);


}

D3D12_CPU_DESCRIPTOR_HANDLE DeviceD3D12::AllocateRTV()
{
	auto& rtvAlloc = descriptorAllocator_.rtv;
	IFNITY_ASSERT(rtvAlloc.nextSlot < rtvAlloc.maxSlots && "RTV Heap overflow: No more RTV descriptors available.");

	//Check if m_RtvHeap is null
	if( m_RtvHeap == nullptr )
	{
		IFNITY_LOG(LogCore, ERROR, "RTV Heap is null. Cannot allocate RTV descriptor.");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(rtvAlloc.nextSlot, rtvAlloc.rtvDescriptorSize);

	++rtvAlloc.nextSlot;

	return handle;
}

void DeviceD3D12::RenderDemo(int w, int h) const
{}

void* DeviceD3D12::Wrapper_ptr_data()
{
	return reinterpret_cast<void*>(m_CommandList.GetAddressOf());
}

void DeviceD3D12::ClearBackBuffer(float* color)
{
	m_ClearColor = color;
}

void DeviceD3D12::SetVSync(bool enabled)
{}

bool DeviceD3D12::IsVSync() const
{
	return false;
}

bool DeviceD3D12::InitInternalInstance()
{
	UINT debugFlags = 0;

	// Enable the D3D12 debug layer.
	if( m_DeviceParams.enableDebugRuntime )
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(OUT & debugController)));
		debugController->EnableDebugLayer();
		debugFlags = DXGI_CREATE_FACTORY_DEBUG;
		IFNITY_LOG(LogCore, TRACE, "Enable Debug Layer D3D12");

		ReportLiveObjects();
		#if defined(_DEBUG)
		ReportLiveObjects();
		#endif
	}

	//Build the DXGI Factory
	if( !dxgiFactory4 )
	{
		HRESULT hres = CreateDXGIFactory2(debugFlags, IID_PPV_ARGS(OUT & dxgiFactory4));
		if( hres != S_OK )
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
	if( FAILED(dxgiFactory4->EnumAdapters(adapterIndex, OUT & m_DxgiAdapter)) )
	{
		if( adapterIndex == 0 )
			IFNITY_LOG(LogCore, ERROR, "Cannot find any DXGI adapters in the system. D3D12");
		else
			//IFNITY_LOG(LogCore, ERROR, "The specified DXGI adapter %d does not exist. D3D12", adapterIndex);

			return false;
	}

	{
		DXGI_ADAPTER_DESC aDesc;
		m_DxgiAdapter->GetDesc(&aDesc);
		m_IsNvidia = rhi::IsNvDeviceID(aDesc.VendorId);
	}

	//Create the D3D12 device
	HRESULT result = D3D12CreateDevice(
		IN m_DxgiAdapter.Get(),
		IN m_DeviceParams.featureLevel,
		IID_PPV_ARGS(OUT & m_Device));



	// Fallback to WARP device.
	if( FAILED(result) )
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(dxgiFactory4->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			IN pWarpAdapter.Get(),
			IN D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(OUT & m_Device)));
	}

	{
		D3D12MA::ALLOCATOR_DESC desc = {};
		desc.Flags = D3D12MA::ALLOCATOR_FLAGS::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED | D3D12MA::ALLOCATOR_FLAGS::ALLOCATOR_FLAG_SINGLETHREADED;
		desc.pDevice = m_Device.Get();
		desc.pAdapter = m_DxgiAdapter.Get();

		/* if (ENABLE_CPU_ALLOCATION_CALLBACKS)
		 {
			 g_AllocationCallbacks.pAllocate = &CustomAllocate;
			 g_AllocationCallbacks.pFree = &CustomFree;
			 g_AllocationCallbacks.pPrivateData = CUSTOM_ALLOCATION_PRIVATE_DATA;
			 desc.pAllocationCallbacks = &g_AllocationCallbacks;
		 }*/

		ThrowIfFailed(D3D12MA::CreateAllocator(&desc, &g_Allocator));
	}

	//Check if the debug layer when is enabled
	if( m_DeviceParams.enableDebugRuntime )
	{
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		m_Device->QueryInterface(IID_PPV_ARGS(OUT & pInfoQueue));

		if( pInfoQueue )
		{
			#ifdef _DEBUG
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);

			#endif

			D3D12_MESSAGE_ID disableMessageIDs[] = {
				D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH, // descriptor validation doesnt understand acceleration structures
			};

			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.pIDList = disableMessageIDs;
			filter.DenyList.NumIDs = sizeof(disableMessageIDs) / sizeof(disableMessageIDs[ 0 ]);
			pInfoQueue->AddStorageFilterEntries(&filter);
		}


		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if( SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))) )
		{
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, TRUE);
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
	CreateImmediateCommands();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	//Crate RTV and DSV Descriptor Heaps

	LoadAssetDemo();

	OnResize();

	CaptureD3D12DebugMessages();

	//Ok try to create the DeviceHandle 
	m_RenderDevice = D3D12::CreateDevice(this);

	return true;
}

void DeviceD3D12::ResizeSwapChain()
{}

void DeviceD3D12::InitializeGui()
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.


	//
	//Initialize ImGui D3D12
	ImGui_ImplDX12_Init(m_Device.Get(),
						D3D12::D3D12ImmediateCommands::kMaxCommandBuffers,
						m_BackBufferFormat,
						m_CbvSrvUavHeap.Get(),
						m_CbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(),
						m_CbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());




	// Help me create the pipeline state object.


}
void DeviceD3D12::InternalPreDestroy()
{}
void DeviceD3D12::LogAdaptersD3D12()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while( dxgiFactory4->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND )
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
	for( size_t i = 0; i < adapterList.size(); ++i )
	{
		DXGI_ADAPTER_DESC desc;
		adapterList[ i ]->GetDesc(&desc);
		std::wstring adapterDescription = desc.Description;
		//Transform wstrint to string. Fix non void pointer error.
		std::string adapterDescriptionStr(adapterDescription.begin(), adapterDescription.end());
		m_IsNvidia = rhi::IsNvDeviceID(desc.VendorId);

		IFNITY_LOG(LogCore, INFO, "Adapter Description  " + adapterDescriptionStr + "  N Adapter :  " + std::to_string(i));
		adapterList[ i ]->Release();
	}


}

void DeviceD3D12::CreateFenceAndDescriptorSizes()
{
	// Create Fence.
	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(OUT & m_Fence)));

	m_DescritporSizes.Rtv = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DescritporSizes.Dsv = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_DescritporSizes.CbvSrvUav = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Fill descritpor allocator
	descriptorAllocator_.rtv.rtvDescriptorSize = m_DescritporSizes.Rtv;
	descriptorAllocator_.dsv.dsvDescriptorSize = m_DescritporSizes.Dsv;
	descriptorAllocator_.uav.uavDescriptorSize = m_DescritporSizes.CbvSrvUav;

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
		OUT & msQualityLevels,
		sizeof(msQualityLevels)));

	assert(msQualityLevels.NumQualityLevels > 0 && "Unexpected MSAA quality level.");
	IFNITY_LOG(LogCore, INFO, "MSAA Quality Level: " + std::to_string(msQualityLevels.NumQualityLevels));

	return msQualityLevels.NumQualityLevels;
}
bool DeviceD3D12::CreateSwapChain()
{
	// Obtener el handle de la ventana desde GLFW
	m_hWnd = glfwGetWin32Window(m_Window);

	if( !m_hWnd )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to get window handle from GLFW.");
		return false;
	}
	//Create the swap chain.
	swapchain_ = std::make_unique<D3D12::D3D12Swapchain>(*this, m_hWnd, GetWidth(), GetHeight());

	m_SwapChain = swapchain_->getSwapchain();


	//// Liberar los recursos anteriores que vamos a recrear.
	//for (int i = 0; i < m_SwapChainBufferCount; ++i)
	//{
	//	if (m_SwapChainBuffer[i] != nullptr)
	//	{
	//		m_SwapChainBuffer[i]->Release();
	//		m_SwapChainBuffer[i] = nullptr; // Asegrate de establecer el puntero a nullptr despus de liberar el recurso.
	//	}
	//}

	//// Libera la swap chain anterior.
	//m_SwapChain.Reset();

	//FlushCommandQueue();

	//// Describe and create the swap chain.
	//DXGI_MODE_DESC bufferDesc = {};
	//bufferDesc.Width = GetWidth();
	//bufferDesc.Height = GetHeight();
	//bufferDesc.RefreshRate.Numerator = 60;
	//bufferDesc.RefreshRate.Denominator = 1;
	//bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//DXGI_SAMPLE_DESC sampleDesc = {};
	//sampleDesc.Count = m_MsaaState ? 4 : 1; // Activate MSSA 4X , by default is false. 												   
	//sampleDesc.Quality = CheckMSAAQualitySupport(sampleDesc.Count,
	//	bufferDesc.Format) - 1; // Its importa													to substract 1 because the quality level is 0 based.

	//DXGI_SWAP_CHAIN_DESC sd = {};
	//sd.BufferDesc = bufferDesc;
	//sd.SampleDesc = sampleDesc;
	//sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//sd.BufferCount = 2;
	//sd.OutputWindow = m_hWnd;
	//sd.Windowed = true;
	//sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//ComPtr<IDXGISwapChain> swapChain;
	//ThrowIfFailed(dxgiFactory4->CreateSwapChain(
	//	commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
	//	&sd,
	//	m_SwapChain.GetAddressOf()
	//));



	OutputDebugString(L"SwapChain Created\n");
	return true;




}
void DeviceD3D12::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;  // Direct command list can execute all grapchis commands.
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

	ThrowIfFailed(m_Device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_DirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(m_Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_DirectCmdListAlloc.Get(),
		nullptr,
		IID_PPV_ARGS(m_CommandList.GetAddressOf())));

	m_CommandList->Close();

}

void DeviceD3D12::CreateImmediateCommands()
{
	if( commandQueue )
	{
		//m_ImmediateCommands = std::make_unique<D3D12::D3D12ImmediateCommands>();
		m_ImmediateCommands = std::make_unique<D3D12::D3D12ImmediateCommands>(m_Device.Get(), commandQueue.Get());
	}
	else
	{
		IFNITY_LOG(LogCore, ERROR, "Command Queue is not created yet.");
	}


}



void DeviceD3D12::CaptureD3D12DebugMessages() const
{
	if( m_DeviceParams.enableDebugRuntime )
	{
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if( SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))) )
		{
			UINT64 messageCount = pInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
			for( UINT64 i = 0; i < messageCount; ++i )
			{
				SIZE_T messageLength = 0;
				pInfoQueue->GetMessage(i, nullptr, &messageLength);
				std::vector<char> messageData(messageLength);
				D3D12_MESSAGE* pMessage = reinterpret_cast<D3D12_MESSAGE*>(messageData.data());
				pInfoQueue->GetMessage(i, pMessage, &messageLength);

				switch( pMessage->Severity )
				{
					case D3D12_MESSAGE_SEVERITY_INFO:
						IFNITY_LOG(LogCore, INFO, "D3D12 INFO: " + std::string(pMessage->pDescription));
						break;
					case D3D12_MESSAGE_SEVERITY_WARNING:
						IFNITY_LOG(LogCore, WARNING, "D3D12 WARNING: " + std::string(pMessage->pDescription));
						break;
					case D3D12_MESSAGE_SEVERITY_ERROR:
						IFNITY_LOG(LogCore, ERROR, "D3D12 ERROR: " + std::string(pMessage->pDescription));
						break;
					case D3D12_MESSAGE_SEVERITY_CORRUPTION:
						IFNITY_LOG(LogCore, ERROR, "D3D12 CORRUPTION: " + std::string(pMessage->pDescription));
						break;
					default:
						IFNITY_LOG(LogCore, TRACE, "D3D12 Message: " + std::string(pMessage->pDescription));
						break;
				}
			}
		}
	}

}



ID3D12Resource* DeviceD3D12::CurrentBackBuffer()const
{
	return m_SwapChainBuffer[ m_CurrentBackBufferIndex ];
}


//The CurrentBackBufferView function returns a D3D12_CPU_DESCRIPTOR_HANDLE that points to the current back buffers render target view (RTV) descriptor. This handle is essential for rendering operations, as it allows the graphics pipeline to know which back buffer to render to.
D3D12_CPU_DESCRIPTOR_HANDLE DeviceD3D12::CurrentBackBufferView() const
{
	// Declare a handle of type D3D12_CPU_DESCRIPTOR_HANDLE
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	// Get the base address of the RTV descriptor heap and calculate the offset
	// m_RtvHeap: Pointer to the RTV descriptor heap
	// GetCPUDescriptorHandleForHeapStart(): Returns the handle to the first descriptor in the heap
	// m_CurrentBackBufferIndex: Index indicating the current back buffer
	// m_DescritporSizes.Rtv: Size of each RTV descriptor in the heap
	handle.ptr = m_RtvHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_CurrentBackBufferIndex * m_DescritporSizes.Rtv;

	// Return the handle to the current back buffers RTV descriptor
	return handle;
}

//The DepthStencilView function returns a D3D12_CPU_DESCRIPTOR_HANDLE that points to the depth/stencil buffers descriptor. This handle is used to bind the depth/stencil buffer to the graphics pipeline.
D3D12_CPU_DESCRIPTOR_HANDLE DeviceD3D12::DepthStencilView() const
{
	return m_DsvHeap->GetCPUDescriptorHandleForHeapStart(); // This is unique for the depth stencil view and not need calculate the offset. 
}

void DeviceD3D12::CreateRtvAndDsvDescriptorHeaps()
{

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = MAX_RTV_SWAPCHAIN_IMAGES + MAX_RTV_DEFFERED_IMAGES;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(OUT m_RtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = MAX_DSV_IMAGES;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(OUT m_DsvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(OUT m_CbvSrvUavHeap.GetAddressOf())));


}

void DeviceD3D12::OnResize()
{
	//Asserts differents conditions about members in D3D12Device.
	assert(m_Device);
	assert(m_SwapChain);
	assert(m_CommandList);


	//Allocator desc creation. 

	auto& cmd = m_ImmediateCommands->acquire();

	//Reuse the memory of the command list.
	//Ensure the command list is in a clean state before recording new commands.
	//Properly synchronize with the GPU to avoid overwriting commands that have not yet been executed.
	//ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), m_PipelineState.Get()));
	cmd.commandList->SetPipelineState(m_PipelineState.Get());

	UINT64 width, height;
	//TODO : Check if the width and height are greater than 0.
	width = GetWidth() > 0 ? GetWidth() : 1;
	height = GetHeight() > 0 ? GetHeight() : 1;

	// Release the previous resources we will be recreating.
	// Liberar los recursos anteriores que vamos a recrear.
	//for (int i = 0; i < m_SwapChainBufferCount; ++i)
	//{
	//	if (m_SwapChainBuffer[i] != nullptr)
	//	{
	//		m_SwapChainBuffer[i]->Release();
	//		m_SwapChainBuffer[i] = nullptr; // Asegrate de establecer el puntero a nullptr despus de liberar el recurso.
	//	}
	//}
	if( m_DepthStencilBuffer )
	{
		m_DepthStencilBuffer->Release();
	}
	if( m_DepthStencilAllocation )
	{
		m_DepthStencilAllocation->Release();
	}


	// Resize the swap chain.
	//ThrowIfFailed(m_SwapChain->ResizeBuffers(
	//	m_SwapChainBufferCount,
	//	width, height,
	//	m_BackBufferFormat,
	//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	//m_CurrentBackBufferIndex = 0;


	////Create RTV Descriptor Heap
	//{
	//	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	//	for (UINT i = 0; i < m_SwapChainBufferCount; i++)
	//	{
	//		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
	//		m_Device->CreateRenderTargetView(m_SwapChainBuffer[i], nullptr, rtvHeapHandle);
	//		rtvHeapHandle.Offset(1, m_DescritporSizes.Rtv); //1 is the offset to the next descriptor in the heap.
	//	}
	//}
	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = m_DepthStencilFormat;
	//TODO CHANGE: 
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_DepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	// Allocate a 2-D surface as the depth/stencil buffer FORMAT D3D12MA::ALLOCTION_DESC

	D3D12MA::ALLOCATION_DESC depthStencilAllocDesc = {};
	depthStencilAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC depthStencilResourceDesc = {};
	depthStencilResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilResourceDesc.Alignment = 0;
	depthStencilResourceDesc.Width = width;
	depthStencilResourceDesc.Height = height;
	depthStencilResourceDesc.DepthOrArraySize = 1;
	depthStencilResourceDesc.MipLevels = 1;
	depthStencilResourceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilResourceDesc.SampleDesc.Count = 1;
	depthStencilResourceDesc.SampleDesc.Quality = 0;
	depthStencilResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;



	ThrowIfFailed(g_Allocator->CreateResource(
		&depthStencilAllocDesc,
		&depthStencilResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		&m_DepthStencilAllocation,
		IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())
	));



	//ThrowIfFailed(m_DepthStencilBuffer->SetName(L"Depth/Stencil Resource Heap"));
	//m_DepthStencilAllocation->SetName(L"Depth/Stencil Resource Heap");
	/*ThrowIfFailed(m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		IN & depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		IN & optClear,
		IID_PPV_ARGS(OUT m_DepthStencilBuffer.GetAddressOf())));*/

		/*Create descriptor to mip level 0 of entire resource using the format of the resource.*/
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = m_DepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());


	//// Transition the resource from its initial state to be used as a depth buffer.
	//m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
	//	D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	  // Transition the resource from its initial state to be used as a depth buffer.
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
														D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	cmd.commandList->ResourceBarrier(1, &barrier);
	// Execute the resize commands.
	/*ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);*/
	m_ImmediateCommands->submit(cmd);




	// Update the viewport transform to cover the client area.
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(width);
	m_ScreenViewport.Height = static_cast<float>(height);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;

	m_ScissorRect = { 0, 0, static_cast<int>(width), static_cast<int>(height) };

	//ThrowIfFailed(m_DirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	//m_CommandList->SetDescriptorHeaps(1, m_CbvSrvUavHeap.GetAddressOf());



}

void DeviceD3D12::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	m_CurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point wont be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(commandQueue->Signal(m_Fence.Get(), m_CurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if( m_Fence->GetCompletedValue() < m_CurrentFence )
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void DeviceD3D12::BuildRootSignature()
{
	// Shader programs typically require resources as input (constant buffers,
	// textures, samplers).  The root signature defines the resources the shader
	// programs expect.  If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be
	// thought of as defining the function signature. 
	// Create an empty root signature. that shader will use to access resources in D3D12.
	{
		//Describe and create a root signature. The root signature describes the data an HLSL shader expects. AND
		//D3D12_ROOT_SIGNATURE_DESC: can use input shader layout .
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0,
							   nullptr,
							   0,
							   nullptr,
							   D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		ThrowIfFailed(D3D12SerializeRootSignature(IN & rootSignatureDesc,
												  D3D_ROOT_SIGNATURE_VERSION_1,
												  OUT & signature, OUT & error));

		ThrowIfFailed(m_Device->CreateRootSignature(0,
													signature->GetBufferPointer(),
													signature->GetBufferSize(),
													IID_PPV_ARGS(OUT & m_RootSignature)));



		#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		#else
		UINT compileFlags = 0;
		#endif



	}
}

void DeviceD3D12::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;


	



	/*m_VsByteCode = CompileShader(L"Shaders\\shaders.hlsl", nullptr, "VSMain", "vs_5_0");
	m_PsByteCode = CompileShader(L"Shaders\\shaders.hlsl", nullptr, "PSMain", "ps_5_0");*/

	m_VsByteCode = ShaderCompiler::GetBlobFromFile("Shaders\\d3d12\\triangle.hlsl_vs.cso");
	m_PsByteCode = ShaderCompiler::GetBlobFromFile("Shaders\\d3d12\\triangle.hlsl_ps.cso");

	m_InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}



void DeviceD3D12::BuildPipelineStage()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS =
	{
		.pShaderBytecode = m_VsByteCode->GetBufferPointer(),
		.BytecodeLength = m_VsByteCode->GetBufferSize(),
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_PsByteCode->GetBufferPointer()),
		m_PsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[ 0 ] = m_BackBufferFormat;
	psoDesc.DSVFormat = m_DepthStencilFormat;
	psoDesc.SampleDesc.Count = m_MsaaState ? 4 : 1; // Activate MSSA 4X , by default is false. 												   
	psoDesc.SampleDesc.Quality = CheckMSAAQualitySupport(psoDesc.SampleDesc.Count,
														 m_BackBufferFormat) - 1; // Its importa													to substract 1 because the quality level is 0 based.
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));


	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { -0.5f, -0.5f , 0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.0f, 0.5f , 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { 0.5f, -0.5f , 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		/*ThrowIfFailed(m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)));*/
		D3D12MA::ALLOCATION_DESC vertexBufferAllocDesc = {};
		vertexBufferAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC vertexBufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

		ThrowIfFailed(g_Allocator->CreateResource(
			&vertexBufferAllocDesc,
			&vertexBufferResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&m_VertexBufferAllocation,
			IID_PPV_ARGS(m_VertexBuffer.GetAddressOf())
		));

		ThrowIfFailed(m_VertexBuffer->SetName(L"Vertex Buffer Resource Heap"));
		m_VertexBufferAllocation->SetName(L"Vertex Buffer Allocation");

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_VertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(Vertex);
		m_VertexBufferView.SizeInBytes = vertexBufferSize;

		//FlushCommandQueue();

	}
}

void DeviceD3D12::ReportLiveObjects() const
{
	ComPtr<IDXGIDebug> debugDXGI;
	ComPtr<IDXGIDebug1> debugDXGI1;
	// Debug DXGI
	ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(OUT & debugDXGI)));
	debugDXGI->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);

	IFNITY_LOG(LogCore, TRACE, "Enable Debug Layer DXGI");

	// Debug DXGI 1
	ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(OUT & debugDXGI1)));
	debugDXGI1->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
}

void DeviceD3D12::DrawElements(const ComPtr<ID3D12PipelineState>& pipelineState,
							   const ComPtr<ID3D12RootSignature>& rootSignature)
{
	m_CommandList->SetPipelineState(pipelineState.Get());
	m_CommandList->SetGraphicsRootSignature(rootSignature.Get());


	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->DrawInstanced(3, 1, 0, 0);

}
void DeviceD3D12::DrawElements(ID3D12GraphicsCommandList* commandList,
							   const ComPtr<ID3D12PipelineState>& pipelineState,
							   const ComPtr<ID3D12RootSignature>& rootSignature)
{
	commandList->SetPipelineState(pipelineState.Get());
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	commandList->DrawInstanced(3, 1, 0, 0);
	//commandList->Close();
	//ID3D12CommandList* cmdsLists[] = { commandList };
	//m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

}

D3D12::CommandBuffer& DeviceD3D12::acquireCommandBuffer()
{
	IFNITY_ASSERT_MSG(!currentCommandBuffer_.ctx_, "Cannot acquire more than 1 command buffer simultaneously");

	currentCommandBuffer_ = D3D12::CommandBuffer(this);

	return currentCommandBuffer_;
}


D3D12::SubmitHandle DeviceD3D12::submit(D3D12::CommandBuffer& commandBuffer, D3D12::TextureHandleSM present)
{
	//Using static_cast to convert and avoid the const in submit 
	auto d3d12CmdBuffer = static_cast<D3D12::CommandBuffer*>(&commandBuffer);

	IFNITY_ASSERT_MSG(d3d12CmdBuffer, "Not d3d12 command buffer");
	IFNITY_ASSERT_MSG(d3d12CmdBuffer->ctx_, "Commandbuffer has no D3D12 context assigned.");
	IFNITY_ASSERT_MSG(d3d12CmdBuffer->wrapper_, "Commandbuffer has no wrapper assigned.");

	if (present)
	{
		 D3D12::D3D12Image& tex = *slotMapTextures_.get(present);
		IFNITY_ASSERT_MSG(tex.isSwapchainImage_, "No SwapChainImage acquired for submission");

		// Prepare image for presentation
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			tex.resource_.Get(),
			tex.currentState_,
			D3D12_RESOURCE_STATE_PRESENT
		);
		//TrackState
		tex.currentState_ = D3D12_RESOURCE_STATE_PRESENT;
		d3d12CmdBuffer->wrapper_->commandList->ResourceBarrier(1, &barrier);
	}

	//// Submit command buffer to queue
	//ID3D12CommandList* cmdsLists[] = { d3d12CmdBuffer->wrapper_->commandList.Get() };
	//commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Present swapchain if needed
	const bool shouldPresent = true;
	d3d12CmdBuffer->lastSubmitHandle_ = m_ImmediateCommands->submit(*commandBuffer.wrapper_);
	//Is
	if (shouldPresent)
	{
		m_SwapChain->Present(IsVSync() ? 1 : 0,0);
	}

	// Optionally process deferred tasks (resource deletion, etc.)
	//processDeferredTasks();

	// Fence signal for tracking GPU work
	D3D12::SubmitHandle handle = d3d12CmdBuffer->lastSubmitHandle_; 
	// Reset current command buffer for next frame
	currentCommandBuffer_ = {};

	return handle;
}

D3D12::TextureHandleSM DeviceD3D12::getCurrentSwapChainTexture()
{
	if( !m_SwapChain )
	{
		return {};
	}
	// Get the current back buffer index
	D3D12::TextureHandleSM tex = swapchain_->getCurrentTexture();

	if( !tex.valid() )
	{
		IFNITY_LOG(LogCore, ERROR, "No swapchain image acquired");
		return {};
	}

	auto* texptr = slotMapTextures_.get(tex);
	IFNITY_ASSERT_MSG(texptr->format_ != DXGI_FORMAT_UNKNOWN , "Invalid swapchain d3d12 image format");

	return tex;
}











void DeviceD3D12::LoadAssetDemo()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineStage();
}





IFNITY_END_NAMESPACE