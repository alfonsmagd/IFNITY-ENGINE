

#include "vulkan_Buffer.hpp"
#include "../Windows/UtilsVulkan.h"
#include "../Windows/DeviceVulkan.h"
#include "vulkan_classes.hpp"

IFNITY_NAMESPACE



namespace Vulkan
{

	
	void VulkanBuffer::bufferSubData(const DeviceVulkan& ctx, size_t offset, size_t size, const void* data)
	{
		// only host-visible buffers can be uploaded this way
		IFNITY_ASSERT(mappedPtr_);

		if(!mappedPtr_)
		{
			IFNITY_LOG(LogCore, ERROR, "Buffer is not mapped");
			return;
		}

		IFNITY_ASSERT(offset + size <= bufferSize_);

		if(data)
		{
			memcpy((uint8_t*)mappedPtr_ + offset, data, size);
		}
		else
		{
			memset((uint8_t*)mappedPtr_ + offset, 0, size);
		}

		if(!isCoherentMemory_)
		{
			flushMappedMemory(ctx, offset, size);
		}
	
	
	}

	void VulkanBuffer::getBufferSubData(const DeviceVulkan & ctx, size_t offset, size_t size, void* data)
	{
		
		// only host-visible buffers can be downloaded this way
		IFNITY_ASSERT(mappedPtr_);

		if(!mappedPtr_)
		{
			return;
		}

		IFNITY_ASSERT(offset + size <= bufferSize_);

		if(!isCoherentMemory_)
		{
			invalidateMappedMemory(ctx, offset, size);
		}

		const uint8_t* src = static_cast<uint8_t*>(mappedPtr_) + offset;
		memcpy(data, src, size);
	
	}

	void VulkanBuffer::flushMappedMemory(const DeviceVulkan & ctx, VkDeviceSize offset, VkDeviceSize size) const
	{
		if(!IFNITY_VERIFY(isMapped()))
		{
			IFNITY_LOG(LogCore, ERROR, "Buffer is not mapped");
			return;
		}
		if(VMA_ALLOCATOR_VK)
		{
			vmaFlushAllocation(static_cast<VmaAllocator>(ctx.getVmaAllocator()), vmaAllocation_, offset, size);
		}
		else
		{

			const VkMappedMemoryRange range = {
				 .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				 .memory = vkMemory_,
				 .offset = offset,
				 .size = size,
			};
			vkFlushMappedMemoryRanges(ctx.device_, 1, &range);
			IFNITY_LOG(LogCore, TRACE, STRMESSAGE("Memoryflush ranges"));
		}


	
	}

	void VulkanBuffer::invalidateMappedMemory(const DeviceVulkan & ctx, VkDeviceSize offset, VkDeviceSize size) const
	{
		if(!IFNITY_VERIFY(isMapped()))
		{
			return;
		}

		if(VMA_ALLOCATOR_VK)
		{
			vmaInvalidateAllocation(static_cast<VmaAllocator>(ctx.getVmaAllocator()), vmaAllocation_, offset, size);
		}
		else
		{
			const VkMappedMemoryRange range = {
				 .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				 .memory = vkMemory_,
				 .offset = offset,
				 .size = size,
			};
			vkInvalidateMappedMemoryRanges(ctx.device_, 1, &range);
		}

	
	}

}

IFNITY_END_NAMESPACE