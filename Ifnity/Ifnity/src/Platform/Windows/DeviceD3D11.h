#pragma once 

#include "pch.h"
#include "Ifnity/GraphicsDeviceManager.h"



IFNITY_NAMESPACE

using namespace Microsoft::WRL;

class DeviceD3D11 final : public GraphicsDeviceManager
{
private:
	// D3D11 ComPtr Objects
	ComPtr<ID3D11Device> m_Device = nullptr;
	ComPtr<ID3D11DeviceContext> m_ImmediateContext = nullptr;
	ComPtr<IDXGIFactory1> m_DxgiFactory = nullptr;
	ComPtr<IDXGISwapChain> m_SwapChain = nullptr;
	ComPtr<ID3D11RenderTargetView> m_RenderTarget = nullptr;
	ComPtr<IDXGIAdapter> m_DxgiAdapter = nullptr;
	DXGI_SWAP_CHAIN_DESC m_SwapChainDesc{};
	HWND m_hWnd = nullptr;


	std::string m_RendererString;


	struct DeviceParams
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		bool enableDebugRuntime = false;
	} m_DeviceParams;







public:
		virtual ~DeviceD3D11();

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_Props.Width; }
	inline unsigned int GetHeight() const override { return m_Props.Height; }

protected:
	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;
	bool InitInternalInstance() override; //TODO: Implement this function in .cpp file.
	bool ConfigureSpecificHintsGLFW() const  override;
	bool CreateAPISurface() override;


private:
	void Init();
	void InitializeD3D11();
	std::string GetD3D11Info();

};

IFNITY_END_NAMESPACE
