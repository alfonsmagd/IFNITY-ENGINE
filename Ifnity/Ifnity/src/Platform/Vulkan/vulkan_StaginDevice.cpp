

#include "vulkan_StaginDevice.hpp"
#include "vulkan_Buffer.hpp"
#include "vulkan_VulkanImage.hpp"
#include "../Windows/DeviceVulkan.h"


IFNITY_NAMESPACE



namespace Vulkan
{

	uint32_t getAlignedSize(uint32_t value, uint32_t alignment)
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}

	VulkanStagingDevice::VulkanStagingDevice(DeviceVulkan& ctx): ctx_(ctx)
	{


		const auto& limits = ctx.GetPhysicalDeviceLimits();

		// use default value of 128Mb clamped to the max limits
		maxBufferSize_ = std::min(limits.maxStorageBufferRange, 128u * 1024u * 1024u);

		//IFNITY ASSERT
		IFNITY_ASSERT(minBufferSize_ <= maxBufferSize_, "");


	}

	void VulkanStagingDevice::bufferSubData(VulkanBuffer& buffer, size_t dstOffset, size_t size, const void* data)
	{
		//Now we only do to test the not uses staging buffer, host visible buffer TODO: Implement staginbuffer and copy data to buffer
		if(buffer.isMapped())
		{
			buffer.bufferSubData(ctx_, dstOffset, size, data);
			return;
		}

		//If the buffer is not mapped, we need to map it and copy the data in staginbuffer
	}

	void VulkanStagingDevice::imageData2D(VulkanImage& image,
										  const VkRect2D& imageRegion, 
										  uint32_t baseMipLevel,
										  uint32_t numMipLevels,
										  uint32_t layer,
										  uint32_t numLayers,
										  VkFormat format,
										  const void* data)
	{}

	void VulkanStagingDevice::imageData3D(VulkanImage& image,
										  const VkOffset3D& offset,
										  const VkExtent3D& extent,
										  VkFormat format,
										  const void* data)
	{}

	void VulkanStagingDevice::getImageData(VulkanImage& image,
										   const VkOffset3D& offset,
										   const VkExtent3D& extent,
										   VkImageSubresourceRange range,
										   VkFormat format,
										   void* outData)
	{}

	VulkanStagingDevice::MemoryRegionDesc VulkanStagingDevice::getNextFreeOffset(uint32_t size)
	{
		const uint32_t requestedAlignedSize = getAlignedSize(size, kStagingBufferAlignment);

		ensureStagingBufferSize(requestedAlignedSize);

		IFNITY_ASSERT(!regions_.empty());

		// if we can't find an available region that is big enough to store requestedAlignedSize, return whatever we could find, which will be
		// stored in bestNextIt
		auto bestNextIt = regions_.begin();

		for(auto it = regions_.begin(); it != regions_.end(); ++it)
		{
			if(ctx_.immediate_->isReady(it->handle_))
			{
				// This region is free, but is it big enough?
				if(it->size_ >= requestedAlignedSize)
				{
					// It is big enough!
					const uint32_t unusedSize = it->size_ - requestedAlignedSize;
					const uint32_t unusedOffset = it->offset_ + requestedAlignedSize;

					// Return this region and add the remaining unused size to the regions_ deque
					SCOPE_EXIT{
					  regions_.erase(it);
					  if(unusedSize > 0)
						{
						regions_.push_front({unusedOffset, unusedSize, SubmitHandle()});
						}
					};

					return { it->offset_, requestedAlignedSize, SubmitHandle() };
				}
				// cache the largest available region that isn't as big as the one we're looking for
				if(it->size_ > bestNextIt->size_)
				{
					bestNextIt = it;
				}
			}
		}

		// we found a region that is available that is smaller than the requested size. It's the best we can do
		if(bestNextIt != regions_.end() && ctx_.immediate_->isReady(bestNextIt->handle_))
		{
			SCOPE_EXIT{
			  regions_.erase(bestNextIt);
			};

			return { bestNextIt->offset_, bestNextIt->size_, SubmitHandle() };
		}

		// nothing was available. Let's wait for the entire staging buffer to become free
		waitAndReset();

		// waitAndReset() adds a region that spans the entire buffer. Since we'll be using part of it, we need to replace it with a used block and
		// an unused portion
		regions_.clear();

		// store the unused size in the deque first...
		const uint32_t unusedSize = stagingBufferSize_ > requestedAlignedSize ? stagingBufferSize_ - requestedAlignedSize : 0;

		if(unusedSize)
		{
			const uint32_t unusedOffset = stagingBufferSize_ - unusedSize;
			regions_.push_front({ unusedOffset, unusedSize, SubmitHandle() });
		}

		// ...and then return the smallest free region that can hold the requested size
		return {
			 .offset_ = 0,
			 .size_ = stagingBufferSize_ - unusedSize,
			 .handle_ = SubmitHandle(),
		};
	}

	void VulkanStagingDevice::ensureStagingBufferSize(uint32_t sizeNeeded)
	{
	
		const uint32_t alignedSize = std::max(getAlignedSize(sizeNeeded, kStagingBufferAlignment), minBufferSize_);

		sizeNeeded = alignedSize < maxBufferSize_ ? alignedSize : maxBufferSize_;

		if(!stagingBuffer_->empty())
		{
			const bool isEnoughSize = sizeNeeded <= stagingBufferSize_;
			const bool isMaxSize = stagingBufferSize_ == maxBufferSize_;

			if(isEnoughSize || isMaxSize)
			{
				return;
			}
		}

		waitAndReset();

		// deallocate the previous staging buffer
		stagingBuffer_ = nullptr;

		// if the combined size of the new staging buffer and the existing one is larger than the limit imposed by some architectures on buffers
		// that are device and host visible, we need to wait for the current buffer to be destroyed before we can allocate a new one
	/*	if((sizeNeeded + stagingBufferSize_) > maxBufferSize_)
		{
			ctx_.waitDeferredTasks();
		}*/

		stagingBufferSize_ = sizeNeeded;

		char debugName[ 256 ] = { 0 };
		snprintf(debugName, sizeof(debugName) - 1, "Buffer: staging buffer %u", stagingBufferCounter_++);

		//stagingBuffer_ = { &ctx_,
		//						ctx_.createBuffer(stagingBufferSize_,
		//												VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		//												VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		//												nullptr,
		//												debugName) };
		

		regions_.clear();
		regions_.push_front({ 0, stagingBufferSize_, SubmitHandle() });
	
	}

	void VulkanStagingDevice::waitAndReset()
	{
		for(const MemoryRegionDesc& r : regions_)
		{
			ctx_.immediate_->wait(r.handle_);
		};
		regions_.clear();
		regions_.push_front({ 0, stagingBufferSize_, SubmitHandle() });
		//


	}



}

IFNITY_END_NAMESPACE