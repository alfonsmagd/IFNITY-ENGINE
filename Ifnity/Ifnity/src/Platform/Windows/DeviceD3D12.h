#pragma once 

#include "pch.h"
#include "Ifnity/GraphicsDeviceManager.hpp"

IFNITY_NAMESPACE

using namespace Microsoft::WRL;

class DeviceD3D12 final : public GraphicsDeviceManager
{


	ComPtr<IDXGIFactory4> m_DxgiFactory = nullptr;
	ComPtr<ID3D12Device>  m_Device = nullptr;
	ComPtr<IDXGIAdapter>  m_DxgiAdapter = nullptr;
	ComPtr<ID3D12Fence>   m_Fence = nullptr;
	ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc = nullptr;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;
	ComPtr<IDXGISwapChain3> m_SwapChain = nullptr;
	HWND m_hWnd = nullptr;


	//States 
	bool m_MsaaState = false;
	bool m_SwapChainBufferCount = 2;    // Double buffering SwapChain move to DeviceManager TODO:

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



public:
	~DeviceD3D12() override;

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_Props.Width; }
	inline unsigned int GetHeight() const override { return m_Props.Height; }
	//change to private
	void RenderDemo(int w, int h) const override;
	

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


	




};

//--------	D3D12 Utils. ---------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------	//


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
		throw HrException(hr);
	}
}




//--------	--------------	----------------------------------------D3D12 Utils.


IFNITY_END_NAMESPACE