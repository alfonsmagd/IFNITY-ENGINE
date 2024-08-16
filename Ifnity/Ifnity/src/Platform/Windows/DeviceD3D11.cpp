#include "DeviceD3D11.h"

IFNITY_NAMESPACE


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")


DeviceD3D11::~DeviceD3D11()
{}

void DeviceD3D11::OnUpdate()
{}

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

bool DeviceD3D11::CreateAPISurface()
{
    int adapterIndex = 0;

    if ( FAILED(m_DxgiFactory->EnumAdapters(adapterIndex, &m_DxgiAdapter)) )
    {
        if ( adapterIndex == 0 )
            IFNITY_LOG(LogCore,ERROR,"Cannot find any DXGI adapters in the system.");
        else
            IFNITY_LOG(LogCore, ERROR, "The specified DXGI adapter %d does not exist.", adapterIndex);
        return false;
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

    return true;

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