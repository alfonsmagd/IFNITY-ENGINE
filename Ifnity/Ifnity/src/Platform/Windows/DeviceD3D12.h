#pragma once 

#include "pch.h"
#include "Ifnity/GraphicsDeviceManager.hpp"
#include "Platform/ImguiRender/ImguiD3D12Render.h"
#include "D3D12MemAlloc.h"

IFNITY_NAMESPACE

using namespace Microsoft::WRL;

class DeviceD3D12 final : public GraphicsDeviceManager
{
	ImDrawData* m_DrawData = nullptr;
	
	ComPtr<IDXGIFactory4> m_DxgiFactory = nullptr;
	ComPtr<ID3D12Device>  m_Device = nullptr;
	ComPtr<IDXGIAdapter>  m_DxgiAdapter = nullptr;
	ComPtr<ID3D12Fence>   m_Fence = nullptr;
	ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc = nullptr;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;
	ComPtr<IDXGISwapChain> m_SwapChain = nullptr;
	HWND m_hWnd = nullptr;

	ComPtr<D3D12MA::Allocator> g_Allocator;

	//States 
	bool m_MsaaState = false;
	UINT  m_SwapChainBufferCount = 2;    // Double buffering SwapChain move to DeviceManager TODO:
	UINT m_CurrentBackBufferIndex = 0;
	UINT m_CurrentFence = 0;
	HANDLE m_FenceEvent = nullptr;

	//Descriptor Heaps
	ComPtr<ID3D12DescriptorHeap> m_RtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_DsvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_CbvSrvUavHeap = nullptr;

	//Resources 
	std::array<ComPtr<ID3D12Resource>, 2> m_SwapChainBuffer;
	ComPtr<ID3D12Resource>				  m_DepthStencilBuffer;
	D3D12MA::Allocation*				  m_DepthStencilAllocation;


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
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		bool enableDebugRuntime = true;
	} m_DeviceParams;

	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

public:
	~DeviceD3D12() override;

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_Props.Width; }
	inline unsigned int GetHeight() const override { return m_Props.Height; }
	//change to private
	void RenderDemo(int w, int h) const override;
	
	void* Wrapper_ptr_data() override;

protected:
	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;
	bool InitInternalInstance() override;
	bool ConfigureSpecificHintsGLFW() const  override;
	bool InitializeDeviceAndContext() override;
	void ResizeSwapChain() override;
	void InitializeGui() override;

private:
	void LogAdaptersD3D12();
	void CreateFenceAndDescriptorSizes();
	UINT CheckMSAAQualitySupport(UINT SampleCount, DXGI_FORMAT format);
	bool CreateSwapChain();
	void CreateCommandQueue();
	void CaptureD3D12DebugMessages() const ;
	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	void CreateRtvAndDsvDescriptorHeaps();
	void OnResize();                         //Todo: Move to Protected. 
	void FlushCommandQueue();
	


	




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




//--------	--------------	----------------------------------------D3D12 Utils.


IFNITY_END_NAMESPACE