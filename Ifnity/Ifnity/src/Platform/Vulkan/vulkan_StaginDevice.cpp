

#include "vulkan_StaginDevice.hpp"
#include "vulkan_Buffer.hpp"
#include "vulkan_VulkanImage.hpp"



IFNITY_NAMESPACE



namespace Vulkan
{



	VulkanStagingDevice::VulkanStagingDevice(DeviceVulkan& ctx): ctx_(ctx)
	{
		// Constructor implementation
	}

	void VulkanStagingDevice::bufferSubData(VulkanBuffer& buffer, size_t dstOffset, size_t size, const void* data)
	{
		// Implementation of bufferSubData
	
	}

	void VulkanStagingDevice::imageData2D(VulkanImage& image, const VkRect2D& imageRegion, uint32_t baseMipLevel, uint32_t numMipLevels, uint32_t layer, uint32_t numLayers, VkFormat format, const void* data)
	{}

	void VulkanStagingDevice::imageData3D(VulkanImage & image, const VkOffset3D & offset, const VkExtent3D & extent, VkFormat format, const void* data)
	{}

	void VulkanStagingDevice::getImageData(VulkanImage & image, const VkOffset3D & offset, const VkExtent3D & extent, VkImageSubresourceRange range, VkFormat format, void* outData)
	{}

	VulkanStagingDevice::MemoryRegionDesc VulkanStagingDevice::getNextFreeOffset(uint32_t size)
	{
		return MemoryRegionDesc();
	}

	void VulkanStagingDevice::ensureStagingBufferSize(uint32_t sizeNeeded)
	{}

	void VulkanStagingDevice::waitAndReset()
	{}

	

}

IFNITY_END_NAMESPACE