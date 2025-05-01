//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-01 by alfonsmagd


#pragma once 

#include "pch.h"
#include "Ifnity/GraphicsDeviceManager.hpp"
#include "Platform/ImguiRender/ImguiD3D12Render.h"
#include "D3D12MemAlloc.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "Platform/D3D12/d3d12_ImmediateCommands.hpp"
#include "Platform/D3D12/d3d12_Image.hpp"
#include "Platform/D3D12/d3d12_classes.hpp"
#include "Platform/D3D12/d3d12_SwapChain.hpp"
#include "Platform/D3D12/d3d12_CommandBuffer.hpp"

IFNITY_NAMESPACE

using namespace Microsoft::WRL;
using namespace DirectX;




class DeviceD3D12 final: public GraphicsDeviceManager
{

	enum
	{
		MAX_RTV_SWAPCHAIN_IMAGES = 3,
		MAX_RTV_DEFFERED_IMAGES = 12,
		MAX_UAV_IMAGES = 16,
		MAX_DSV_IMAGES = 1
	};




public:
	//Pipeline Objects
	ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
	ComPtr<IDXGIFactory4> dxgiFactory4 = nullptr;
	ComPtr<ID3D12Device>  m_Device = nullptr;
	ComPtr<IDXGIAdapter>  m_DxgiAdapter = nullptr;
	ComPtr<ID3D12Fence>   m_Fence = nullptr;
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc = nullptr;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;
	ComPtr<IDXGISwapChain3> m_SwapChain = nullptr;
	HWND m_hWnd = nullptr;

	ComPtr<D3D12MA::Allocator> g_Allocator;
	
	//UniquePtrs
	std::unique_ptr<D3D12::D3D12ImmediateCommands> m_ImmediateCommands = nullptr;
	std::unique_ptr<D3D12::D3D12Swapchain> swapchain_ = nullptr;
	
	//States 
	bool m_MsaaState = false;
	static constexpr size_t numSwapchainImages = 3;
	UINT  m_SwapChainBufferCount = numSwapchainImages;    // Double buffering SwapChain move to DeviceManager TODO:
	UINT m_CurrentBackBufferIndex = 0;
	UINT m_CurrentFence = 0;
	HANDLE m_FenceEvent = nullptr;
	D3D12::CommandBuffer currentCommandBuffer_;

	//Descriptor Heaps
	ComPtr<ID3D12DescriptorHeap> m_RtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_DsvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_CbvSrvUavHeap = nullptr;


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

		struct DSVSlots
		{
			size_t dsvDescriptorSize = 0; // Size of the DSV descriptor heap
			uint32_t nextSlot = 0;
			const uint32_t maxSlots = MAX_DSV_IMAGES; // depthbuffer 
		} dsv;
	};

	DescriptorAllocator descriptorAllocator_;
	
	//Resources 
	std::array<ID3D12Resource*,numSwapchainImages>        m_SwapChainBuffer;
	ComPtr<ID3D12Resource>				  m_DepthStencilBuffer;
	ComPtr<ID3D12Resource>				  m_VertexBuffer;
	D3D12MA::Allocation*				  m_DepthStencilAllocation;

	D3D12MA::Allocation* m_VertexBufferAllocation;

	//Shaders
	ComPtr<ID3DBlob> m_VsByteCode = nullptr;
	ComPtr<ID3DBlob> m_PsByteCode = nullptr;

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
		UINT CbvSrvUav= 0;
	} m_DescritporSizes;


	struct DeviceParams
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;
		bool enableDebugRuntime = true;
	} m_DeviceParams;

	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

	Color m_ClearColor = { 0.0f, 0.2f, 0.4f, 1.0f };


	SlotMap<D3D12::D3D12Image> slotMapTextures_;

public:
	~DeviceD3D12() override;

	void OnUpdate() override;
	D3D12_CPU_DESCRIPTOR_HANDLE  AllocateRTV();
	inline unsigned int GetWidth() const override { return m_Props.Width; }
	inline unsigned int GetHeight() const override { return m_Props.Height; }
	//change to private
	void RenderDemo(int w, int h) const override;
	
	void* Wrapper_ptr_data() override;
	float GetAspectRatio() { return static_cast<float>(GetWidth() / GetHeight()); }

	void ClearBackBuffer(float* color) override;
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
	

	void CaptureD3D12DebugMessages() const ;
	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	void CreateRtvAndDsvDescriptorHeaps();
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

	D3D12::CommandBuffer& acquireCommandBuffer();

	D3D12::SubmitHandle submit(D3D12::CommandBuffer& commandBuffer, D3D12::TextureHandleSM present);
	D3D12::TextureHandleSM getCurrentSwapChainTexture();


	friend D3D12::CommandBuffer;
};


struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

//--------	D3D12 Utils. ---------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------	//

inline void CaptureDXGIMessagesToConsole()
{
	ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
	{
		DXGI_INFO_QUEUE_MESSAGE* pMessage = nullptr;
		SIZE_T messageLength = 0;

		for (UINT i = 0; i < dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL); ++i)
		{
			dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
			pMessage = (DXGI_INFO_QUEUE_MESSAGE*)malloc(messageLength);
			dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
			//Pass to IFNITY_LOG and clasify by category.

			switch (pMessage->Severity)
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
	if (it != rhi::hrErrorMap.end())
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
	char s_str[64] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	std::string s(s_str);
	IFNITY_LOG(LogCore, ERROR, s);
	IFNITY_LOG(LogCore, ERROR, GetDXErrorMessage(hr));
	return std::string(s_str);
}


class HrException : public std::runtime_error
{
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
	HRESULT Error() const { return m_hr; }
private:
	const HRESULT m_hr;
};

//Microsoft Uses. 
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		CaptureDXGIMessagesToConsole();
		throw HrException(hr);
	}
}

inline ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	ThrowIfFailed(hr);

	return byteCode;
}


//--------	--------------	----------------------------------------D3D12 Utils.


IFNITY_END_NAMESPACE