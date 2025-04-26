#pragma once 

#include <pch.h>


#include <d3d12.h>
#include <wrl.h>
#include "Ifnity/Utils/SlotMap.hpp"
#include "d3d12_classes.hpp"

IFNITY_NAMESPACE

namespace D3D12
{
	using namespace Microsoft::WRL;

    struct D3D12Image final
    {
        [[nodiscard]] inline bool isSampledImage() const { return (usageFlags_ & D3D12_RESOURCE_FLAG_NONE) == D3D12_RESOURCE_FLAG_NONE; }
        [[nodiscard]] inline bool isStorageImage() const { return (usageFlags_ & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0; }
        [[nodiscard]] inline bool isColorAttachment() const { return (usageFlags_ & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0; }
        [[nodiscard]] inline bool isDepthAttachment() const { return (usageFlags_ & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0; }
        [[nodiscard]] inline bool isAttachment() const { return isColorAttachment() || isDepthAttachment(); }

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getRTV() const;
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getDSV() const;
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getSRV() const;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getUAV() const;

        static bool isDepthFormat(DXGI_FORMAT format);
	    static bool isDepthStencilFormat(DXGI_FORMAT format);

    public:
        ComPtr<ID3D12Resource> resource_;
        D3D12_RESOURCE_FLAGS usageFlags_ = D3D12_RESOURCE_FLAG_NONE;
        D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_COMMON;
        DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
        D3D12_RESOURCE_DIMENSION type_ = D3D12_RESOURCE_DIMENSION_UNKNOWN;
        D3D12_RESOURCE_DESC desc_ = {};

        // Descriptors
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_ = D3D12InvalidHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_ = D3D12InvalidHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_ = D3D12InvalidHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE uavHandle_ = D3D12InvalidHandle;

        bool isDepthFormat_ = false;
        bool isStencilFormat_ = false;
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        uint16_t depthOrArraySize_ = 1;
        uint16_t mipLevels_ = 1;
        bool isSwapchainImage_ = false;
    };

} // namespace D3D12

IFNITY_END_NAMESPACE