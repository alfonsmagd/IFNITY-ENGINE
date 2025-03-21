#pragma once

#include <pch.h>
#include <VkBootstrap.h>
#include <deque>
#include "vulkan_classes.hpp"


IFNITY_NAMESPACE

class DeviceVulkan;
class VulkanBuffer;
class VulkanImage;
struct SubmitHandle;


namespace Vulkan
{
    class Device;

    class VulkanStagingDevice final
    {
    public:
        explicit VulkanStagingDevice(DeviceVulkan& ctx);
        ~VulkanStagingDevice() = default;

        VulkanStagingDevice(const VulkanStagingDevice&) = delete;
        VulkanStagingDevice& operator=(const VulkanStagingDevice&) = delete;

        void bufferSubData(VulkanBuffer& buffer, size_t dstOffset, size_t size, const void* data);
        void imageData2D(VulkanImage& image,
            const VkRect2D& imageRegion,
            uint32_t baseMipLevel,
            uint32_t numMipLevels,
            uint32_t layer,
            uint32_t numLayers,
            VkFormat format,
            const void* data);
        void imageData3D(VulkanImage& image, const VkOffset3D& offset, const VkExtent3D& extent, VkFormat format, const void* data);
        void getImageData(VulkanImage& image,
            const VkOffset3D& offset,
            const VkExtent3D& extent,
            VkImageSubresourceRange range,
            VkFormat format,
            void* outData);

    private:
        enum { kStagingBufferAlignment = 16 }; // updated to support BC7 compressed image

        struct MemoryRegionDesc
        {
            uint32_t offset_ = 0;
            uint32_t size_ = 0;
            SubmitHandle handle_;
        };

        MemoryRegionDesc getNextFreeOffset(uint32_t size);
        void ensureStagingBufferSize(uint32_t sizeNeeded);
        void waitAndReset();

    private:
        Device* internalDevice_;
        DeviceVulkan& ctx_;
        HolderBufferSM stagingBuffer_;
        uint32_t stagingBufferSize_ = 0;
        uint32_t stagingBufferCounter_ = 0;
        uint32_t maxBufferSize_ = 0;
        const uint32_t minBufferSize_ = 4u * 2048u * 2048u;
        std::deque<MemoryRegionDesc> regions_;
    };


} // namespace Vulkan

IFNITY_END_NAMESPACE