

#include "vulkan_StaginDevice.hpp"
#include "vulkan_Buffer.hpp"
#include "vulkan_VulkanImage.hpp"
#include "../Windows/vk_backend.hpp"
#include "../Windows/DeviceVulkan.h"
#include "Ifnity/Graphics/Utils.hpp"

#include "../Windows/UtilsVulkan.h"


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
		if( buffer.isMapped() )
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
	{

		IFNITY_ASSERT(numMipLevels <= rhi::MAX_MIPMAPS_LEVEL);

		//Simplify the code.
		// divide the width and height by 2 until we get to the size of level 'baseMipLevel'
		uint32_t width = image.vkExtent_.width >> baseMipLevel;
		uint32_t height = image.vkExtent_.height >> baseMipLevel;

		const rhi::Format texFormat(getRHIFormat(format));

		// find the storage size for all mip-levels being uploaded
		uint32_t layerStorageSize = 0;
		for( uint32_t i = 0; i < numMipLevels; ++i )
		{
			const uint32_t mipSize = Utils::getTextureBytesPerLayer(image.vkExtent_.width, image.vkExtent_.height, texFormat, i);
			layerStorageSize += mipSize;
			width = width <= 1 ? 1 : width >> 1;
			height = height <= 1 ? 1 : height >> 1;
		}

		const uint32_t storageSize = layerStorageSize * numLayers;

		//ensureStagingBufferSize(layerStorageSize);

		//MemoryRegionDesc desc = getNextFreeOffset(storageSize);
		//// No support for copying image in multiple smaller chunk sizes. If we get smaller buffer size than storageSize, we will wait for GPU idle
		//// and get bigger chunk.
		//if( desc.size_ < storageSize )
		//{
		//	waitAndReset();
		//	desc = getNextFreeOffset(storageSize);
		//}


		//Process with staggin buffer 
		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
		VkMemoryAllocateInfo memAllocInfo{ .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		VkMemoryRequirements memReqs = {};

		VkBufferCreateInfo bufferCreateInfo = VulkanBuffer::bufferCreateInfo();
		bufferCreateInfo.size = storageSize;
		// This buffer is used as a transfer source for the buffer copy
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		IFNITY_ASSERT(!vkCreateBuffer(ctx_.device_, &bufferCreateInfo, nullptr, &stagingBuffer));

		// Get memory requirements for the staging buffer (alignment, memory type bits)
		vkGetBufferMemoryRequirements(ctx_.device_, stagingBuffer, &memReqs);

		VkFlags memFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		IFNITY_ASSERT(!allocateMemory(ctx_.getPhysicalDevice(), ctx_.device_, &memReqs, memFlags, &stagingMemory));
		IFNITY_ASSERT(!vkBindBufferMemory(ctx_.device_, stagingBuffer, stagingMemory, 0));

		//Copy the data to the staging buffer
		void* mappedData;
		IFNITY_ASSERT(!vkMapMemory(ctx_.device_, stagingMemory, 0, storageSize, 0, &mappedData));
		memcpy(mappedData, data, storageSize);
		vkUnmapMemory(ctx_.device_, stagingMemory);

		//Get COMMAND BUFFER TO UPLOAD 
		const VulkanImmediateCommands::CommandBufferWrapper& wrapper = ctx_.immediate_->acquire();


		// 1. Transition initial image layout into TRANSFER_DST_OPTIMAL
		VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
		uint32_t currentMipLevel = baseMipLevel;
		imageMemoryBarrier2(wrapper.cmdBuf_,
							image.vkImage_,
							StageAccess{ .stage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, .access = VK_ACCESS_2_NONE },
							StageAccess{ .stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT, .access = VK_ACCESS_2_TRANSFER_WRITE_BIT },
							VK_IMAGE_LAYOUT_UNDEFINED,
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							VkImageSubresourceRange{ imageAspect, currentMipLevel, 1, layer, 1 });

		// 2. Copy the pixel data from the staging buffer into the image
		uint32_t planeOffset = 0;

		const VkExtent2D extent =
		{
			.width = std::max(1u, imageRegion.extent.width), //for now not mip levels
			.height = std::max(1u, imageRegion.extent.height),//for now not mip levels 
		};
		const VkRect2D region = {
			.offset = {.x = imageRegion.offset.x , .y = imageRegion.offset.y},
			.extent = extent,
		};
		const VkBufferImageCopy copy = {
			// the offset for this level is at the start of all mip-levels plus the size of all previous mip-levels being uploaded
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource =
			VkImageSubresourceLayers{imageAspect, currentMipLevel, layer, 1},
			.imageOffset = {.x = region.offset.x, .y = region.offset.y, .z = 0},
			.imageExtent = {.width = region.extent.width, .height = region.extent.height, .depth = 1u},
		};
		vkCmdCopyBufferToImage(wrapper.cmdBuf_,
							   stagingBuffer,
							   image.vkImage_,
							   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							   1,
							   &copy);

		// 3. Transition TRANSFER_DST_OPTIMAL into SHADER_READ_ONLY_OPTIMAL
		imageMemoryBarrier2(
			wrapper.cmdBuf_,
			image.vkImage_,
			StageAccess{ .stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT,     .access = VK_ACCESS_2_TRANSFER_WRITE_BIT },
			StageAccess{ .stage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, .access = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT },
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkImageSubresourceRange{ imageAspect, currentMipLevel, 1, layer, 1 });



		image.vkImageLayout_ = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		//Lambdas capture 
		auto handle = ctx_.immediate_->submit(wrapper);
		auto device = ctx_.device_;
		ctx_.addDeferredTask(DESTROY_VK_STAGING_BUFFER_DEFFERED(device, stagingBuffer, stagingMemory), handle);



	}

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

		for( auto it = regions_.begin(); it != regions_.end(); ++it )
		{
			if( ctx_.immediate_->isReady(it->handle_) )
			{
				// This region is free, but is it big enough?
				if( it->size_ >= requestedAlignedSize )
				{
					// It is big enough!
					const uint32_t unusedSize = it->size_ - requestedAlignedSize;
					const uint32_t unusedOffset = it->offset_ + requestedAlignedSize;

					// Return this region and add the remaining unused size to the regions_ deque
					SCOPE_EXIT{
					  regions_.erase(it);
					  if( unusedSize > 0 )
						{
						regions_.push_front({unusedOffset, unusedSize, SubmitHandle()});
						}
					};

					return { it->offset_, requestedAlignedSize, SubmitHandle() };
				}
				// cache the largest available region that isn't as big as the one we're looking for
				if( it->size_ > bestNextIt->size_ )
				{
					bestNextIt = it;
				}
			}
		}

		// we found a region that is available that is smaller than the requested size. It's the best we can do
		if( bestNextIt != regions_.end() && ctx_.immediate_->isReady(bestNextIt->handle_) )
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

		if( unusedSize )
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

		if( !stagingBuffer_->empty() )
		{
			const bool isEnoughSize = sizeNeeded <= stagingBufferSize_;
			const bool isMaxSize = stagingBufferSize_ == maxBufferSize_;

			if( isEnoughSize || isMaxSize )
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
		for( const MemoryRegionDesc& r : regions_ )
		{
			ctx_.immediate_->wait(r.handle_);
		};
		regions_.clear();
		regions_.push_front({ 0, stagingBufferSize_, SubmitHandle() });
		//


	}



}

IFNITY_END_NAMESPACE