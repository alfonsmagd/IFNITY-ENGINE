//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-19 by alfonsmagd



// 
// 
// 
// 
// 
// Last modified: 2025-05-13 by alfonsmagd




#pragma once 

#include "pch.h"
#include "Ifnity/GraphicsDeviceManager.hpp"
#include "Platform/ImguiRender/ImguiD3D12Render.h"
#include "D3D12MemAlloc.h"
#include <d3dcompiler.h>
#include <dxcapi.h>
#include <DirectXMath.h>

#include "Platform/D3D12/d3d12_ImmediateCommands.hpp"
#include "Platform/D3D12/d3d12_Image.hpp"
#include "Platform/D3D12/d3d12_Buffer.hpp"
#include "Platform/D3D12/d3d12_classes.hpp"
#include "Platform/D3D12/d3d12_SwapChain.hpp"
#include "Platform/D3D12/d3d12_CommandBuffer.hpp"
#include "Platform/D3D12/d3d12_StaginDevice.hpp"

#include "Platform/D3D12/d3d12_classes.hpp"
#include "d3d12_backend.hpp"


IFNITY_NAMESPACE

using namespace Microsoft::WRL;
using namespace DirectX;



class DeviceD3D12 final: public GraphicsDeviceManager
{

	enum
	{
		MAX_NUMBER_SAMPLERS = 8,
		MAX_RTV_SWAPCHAIN_IMAGES = 3,
		MAX_RTV_DEFFERED_IMAGES = 12,
		MAX_SRV_IMAGES = 562,
		MAX_UAV_IMAGES = 16,
		MAX_DSV_IMAGES = 1,
		MAX_INDEX_SLOT_BUFFERS = 10,

		START_SLOT_TEXTURES = 10,
		START_SLOT_BUFFERS = 1,

		DEPTH_SRV_INDEX = 561
	};

	enum
	{
		kBinding_RootConstant = 0,
		kBinding_RootConstant_BaseInstance = 1,


		kMAX_ROOT_DESCRIPTORS 
	};




public:

	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;


	//Pipeline Objects
	ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
	ComPtr<IDXGIFactory4> dxgiFactory4			= nullptr;
	ComPtr<ID3D12Device>  m_Device				= nullptr;
	ComPtr<IDXGIAdapter>  m_DxgiAdapter			= nullptr;
	ComPtr<ID3D12Fence>   m_Fence				= nullptr;
	ComPtr<ID3D12CommandQueue> commandQueue     = nullptr;
	ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc = nullptr;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;
	ComPtr<IDXGISwapChain3> m_SwapChain = nullptr;
	ComPtr<ID3D12CommandSignature> m_CommandSignature = nullptr;


	HWND m_hWnd = nullptr;

	ComPtr<D3D12MA::Allocator> g_Allocator;


	//UniquePtrs
	std::unique_ptr<D3D12::D3D12ImmediateCommands> m_ImmediateCommands = nullptr;
	std::unique_ptr<D3D12::D3D12Swapchain> swapchain_                  = nullptr;
	std::unique_ptr<D3D12::D3D12StagingDevice> stagingDevice_          = nullptr;

	//SlotMaps 
	SlotMap<D3D12::D3D12Image> slotMapTextures_;
	SlotMap<D3D12::ShaderModuleState> slotMapShaderModules_;
	SlotMap<D3D12::GraphicsPipeline> slotMapRenderPipelines_;
	SlotMap<D3D12::D3D12Buffer> slotMapBuffers_;
	SlotMap<D3D12_STATIC_SAMPLER_DESC> slotMapSamplers_;

	//States 
	bool m_MsaaState = false;
	static constexpr size_t numSwapchainImages = 3;
	UINT  m_SwapChainBufferCount = numSwapchainImages;    // Double buffering SwapChain move to DeviceManager TODO:
	UINT m_CurrentBackBufferIndex = 0;
	UINT m_CurrentFence = 0;
	HANDLE m_FenceEvent = nullptr;
	 

	D3D12::CommandBuffer currentCommandBuffer_;
	D3D12::GraphicsPipeline* actualPipeline_ = nullptr;

	//Descriptor Heaps
	ComPtr<ID3D12DescriptorHeap> m_RtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_DsvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_BindlessHeap = nullptr; //Bindless heap for SRV and UAV
	ComPtr<ID3D12DescriptorHeap> m_ImguiHeap = nullptr;

	//D3D12MA
	D3D12MA::Allocation* m_DepthStencilAllocation;
	D3D12MA::Allocation* m_VertexBufferAllocation;


	//Vector Deferred Task
	std::deque<DeferredTask<D3D12::SubmitHandle>> deferredTasks_;


	struct DescriptorAllocator
	{
		struct RTVSlots
		{
			size_t rtvDescriptorSize = 0; // Size of the RTV descriptor heap by default is 
			uint32_t nextSlot = 0;
			const uint32_t maxSlots = MAX_RTV_SWAPCHAIN_IMAGES + MAX_RTV_DEFFERED_IMAGES; // Deferred + swapchain
		} rtv;

		struct UAVSlots
		{
			size_t uavDescriptorSize = 0; // Size of the UAV descriptor heap
			uint32_t nextSlot = 0;
			const uint32_t maxSlots = MAX_UAV_IMAGES; //How many UAVs we can have in the system.
		} uav;

		struct SRVSlots
		{
			size_t srvDescriptorSize = 0; // Size of the SRV descriptor heap
			uint32_t nextSlot = 0;
			const uint32_t maxSlots = MAX_SRV_IMAGES; //How many SRVs we can have in the system.
			std::set<uint32_t> usedIndex; //Used slots in the heap.
		} srv;

		struct DSVSlots
		{
			size_t dsvDescriptorSize = 0; // Size of the DSV descriptor heap
			uint32_t nextSlot = 0;
			const uint32_t maxSlots = MAX_DSV_IMAGES; // depthbuffer 
		} dsv;
	};

	DescriptorAllocator descriptorAllocator_;

	//Resources 
	std::array<ID3D12Resource*, numSwapchainImages>        m_SwapChainBuffer;
	ComPtr<ID3D12Resource>				  m_DepthStencilBuffer;
	ComPtr<ID3D12Resource>				  m_VertexBuffer;


	//Shaders
	ComPtr<IDxcBlob> m_VsByteCode = nullptr;
	ComPtr<IDxcBlob> m_PsByteCode = nullptr;

	IDxcBlob* m_vsBlob = nullptr;
	IDxcBlob* m_psBlob = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	//Pipeline State Objects
	ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
	D3D_DRIVER_TYPE m_D3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	struct DescritporSizes
	{
		UINT Rtv = 0;
		UINT Dsv = 0;
		UINT CbvSrvUav = 0;
		UINT Bindless = 0;
	} m_DescritporSizes;


	struct DeviceParams
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_2;
		bool enableDebugRuntime = true;
	} m_DeviceParams;



	Color m_ClearColor = { 0.0f, 0.2f, 0.4f, 1.0f };




public:
	~DeviceD3D12() override;
	IDevice* GetRenderDevice() const override { return m_RenderDevice.get(); }
	void OnUpdate() override;
	D3D12_CPU_DESCRIPTOR_HANDLE  AllocateRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE  AllocateFreeSRV();
	D3D12_CPU_DESCRIPTOR_HANDLE  AllocateSRV(uint32_t index = 0);
	void                         FreeSRV( uint32_t index = 0 );

	inline unsigned int GetWidth() const override { return m_Props.Width; }
	inline unsigned int GetHeight() const override { return m_Props.Height; }
	//change to private
	void RenderDemo(int w, int h) const override;

	void* Wrapper_ptr_data() override;
	float GetAspectRatio() { return static_cast<float>(GetWidth() / GetHeight()); }
	void ClearBackBuffer(float* color) override;


	D3D12::TextureHandleSM getCurrentSwapChainTexture() const;

	void CreateCommandSignature( ID3D12Device* device, 
								 ID3D12CommandSignature** outSignature,
								 ID3D12RootSignature* rootSig = nullptr) ;

	//DeferredTask
	void processDeferredTasks();
	void waitDeferredTasks();
	void addDeferredTask(std::packaged_task<void()>&& task, D3D12::SubmitHandle handle = D3D12::SubmitHandle());

	
	//Destroy Operations
	void destroy( D3D12::BufferHandleSM bufferHandle );
	void destroy( D3D12::TextureHandleSM textureHandle );

protected:
	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;
	bool InitInternalInstance() override;
	bool ConfigureSpecificHintsGLFW() const  override;
	bool InitializeDeviceAndContext() override;
	void ResizeSwapChain() override;
	void InitializeGui() override;
	void InternalPreDestroy() override;

private:
	void LogAdaptersD3D12();
	void CreateFenceAndDescriptorSizes();
	UINT CheckMSAAQualitySupport(UINT SampleCount, DXGI_FORMAT format);
	bool CreateSwapChain();
	void CreateCommandQueue();
	void CreateImmediateCommands();

	


	void CaptureD3D12DebugMessages() const;
	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDefaultSampler();
	void CreateDummyTexture();
	void OnResize();                         //Todo: Move to Protected. 
	void FlushCommandQueue();

	//Process load assets. 
	void LoadAssetDemo();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineStage();


	//void PopulateCommandList();
	void ReportLiveObjects() const;

	//Render PIX 


	//void DrawElements(); 
	void DrawElements(const ComPtr<ID3D12PipelineState>& pipelineState,
					  const ComPtr<ID3D12RootSignature>& rootSignature);

	void DrawElements(ID3D12GraphicsCommandList* commandList,
					  const ComPtr<ID3D12PipelineState>& pipelineState,
					  const ComPtr<ID3D12RootSignature>& rootSignature);

	void destroy( SlotMap<D3D12::GraphicsPipeline> sm );
	void destroy( SlotMap<D3D12::D3D12Buffer> sm );
private:
	DeviceHandle m_RenderDevice;

	D3D12::CommandBuffer& acquireCommandBuffer();

	D3D12::SubmitHandle submit(D3D12::CommandBuffer& commandBuffer, D3D12::TextureHandleSM present);
	D3D12::TextureHandleSM getCurrentSwapChainTexture();


	friend D3D12::CommandBuffer;
	friend D3D12::Device;
};


struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
};

//--------	D3D12 Utils. ---------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------	//

inline void CaptureDXGIMessagesToConsole()
{
	ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if( SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))) )
	{
		DXGI_INFO_QUEUE_MESSAGE* pMessage = nullptr;
		SIZE_T messageLength = 0;

		for( UINT i = 0; i < dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL); ++i )
		{
			dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
			pMessage = (DXGI_INFO_QUEUE_MESSAGE*)malloc(messageLength);
			dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
			//Pass to IFNITY_LOG and clasify by category.

			switch( pMessage->Severity )
			{
				// Help 
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
					IFNITY_LOG(LogCore, INFO, "INFO: " + std::string(pMessage->pDescription));
					break;
					// Warning
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
					IFNITY_LOG(LogCore, WARNING, "WARNING: " + std::string(pMessage->pDescription));
					break;
					// Error
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
					IFNITY_LOG(LogCore, ERROR, "ERROR: " + std::string(pMessage->pDescription));
					break;
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
					IFNITY_LOG(LogCore, ERROR, "CORRUPTION: " + std::string(pMessage->pDescription));
					break;
				default:
					IFNITY_LOG(LogCore, TRACE, "DXGI Message: " + std::string(pMessage->pDescription));
					break;
			}
			//

			free(pMessage);
		}
	}

}



inline std::string GetDXErrorMessage(HRESULT hr)
{
	auto it = rhi::hrErrorMap.find(hr);
	if( it != rhi::hrErrorMap.end() )
	{
		return std::string(it->second);
	}
	else
	{
		return "Unknown error";
	}
}



inline std::string HrToString(HRESULT hr)
{
	char s_str[ 64 ] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	std::string s(s_str);
	IFNITY_LOG(LogCore, ERROR, s);
	IFNITY_LOG(LogCore, ERROR, GetDXErrorMessage(hr));
	return std::string(s_str);
}


class HrException: public std::runtime_error
{
public:
	HrException(HRESULT hr): std::runtime_error(HrToString(hr)), m_hr(hr) {}
	HRESULT Error() const { return m_hr; }
private:
	const HRESULT m_hr;
};

//Microsoft Uses. 
inline void ThrowIfFailed(HRESULT hr)
{
	if( FAILED(hr) )
	{
		CaptureDXGIMessagesToConsole();
		throw HrException(hr);
	}
}




//--------	--------------	----------------------------------------D3D12 Utils.


IFNITY_END_NAMESPACE