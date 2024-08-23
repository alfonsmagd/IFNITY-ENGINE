#pragma once 

#include "pch.h"
#include "Ifnity/GraphicsDeviceManager.hpp"



IFNITY_NAMESPACE

using namespace Microsoft::WRL;

class DeviceD3D11 final : public GraphicsDeviceManager
{
private:
	// D3D11 ComPtr Objects
	ComPtr<ID3D11Device> m_Device = nullptr;
	ComPtr<ID3D11DeviceContext> m_ImmediateContext = nullptr;
	ComPtr<IDXGIFactory2> m_DxgiFactory = nullptr;
	ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;
	ComPtr<ID3D11RenderTargetView> m_RenderTarget = nullptr;
	ComPtr<IDXGIAdapter> m_DxgiAdapter = nullptr;
	DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc{};
	HWND m_hWnd = nullptr;

	ComPtr<ID3D11Texture2D> m_backBuffer = nullptr;


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
	 //change to private
	void RenderDemo(int w, int h) const override;
	[[nodiscard]] ID3D11Device* GetDevice() const { return m_Device.Get(); }
	[[nodiscard]] ID3D11DeviceContext* GetDeviceContext() const { return m_ImmediateContext.Get(); }

protected:
	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;
	bool InitInternalInstance() override; 
	bool ConfigureSpecificHintsGLFW() const  override;
	bool InitializeDeviceAndContext() override;
	void ResizeSwapChain() override;


private:
	void Init();
	void InitializeD3D11();
	std::string GetD3D11Info();
	bool CreateSwapChain();
	bool CreateSwapChainResources();
	bool CreateRTV(IN const ComPtr<ID3D11Texture2D>& buffer);
	void DestroySwapChainResources();
	

	
};

IFNITY_END_NAMESPACE
