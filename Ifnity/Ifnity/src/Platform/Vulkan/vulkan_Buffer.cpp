

#include "vulkan_Buffer.hpp"
#include "../Windows/UtilsVulkan.h"
#include "../Windows/DeviceVulkan.h"
#include "vulkan_classes.hpp"

IFNITY_NAMESPACE



namespace Vulkan
{

	
				

	void VulkanBuffer::bufferSubData(const DeviceVulkan& ctx, size_t offset, size_t size, const void* data)
	{}

	void VulkanBuffer::getBufferSubData(const DeviceVulkan & ctx, size_t offset, size_t size, void* data)
	{}

	void VulkanBuffer::flushMappedMemory(const DeviceVulkan & ctx, VkDeviceSize offset, VkDeviceSize size) const
	{}

	void VulkanBuffer::invalidateMappedMemory(const DeviceVulkan & ctx, VkDeviceSize offset, VkDeviceSize size) const
	{}

}

IFNITY_END_NAMESPACE