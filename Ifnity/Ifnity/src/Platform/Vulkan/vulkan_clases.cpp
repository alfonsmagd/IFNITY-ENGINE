

#include "vulkan_classes.hpp"
#include "../Windows/UtilsVulkan.h"
#include "Platform/Windows/DeviceVulkan.h"

IFNITY_NAMESPACE


namespace Vulkan
{
	//----------------------------------------------------------------------------------------------------//
	// Static Variables
	//----------------------------------------------------------------------------------------------------//

	uint32_t VulkanPipelineBuilder::numPipelinesCreated_ = 0;




	//-----------------------------------------------------------------------------------------------//
	// VulkanSwapchain METHODS
	//----------------------------------------------- -----------------------------------------------//


	VulkanSwapchain::VulkanSwapchain(DeviceVulkan& ctx, uint32_t width, uint32_t height):
		ctx_(ctx), device_(ctx.device_), graphicsQueue_(ctx.deviceQueues_.graphicsQueue), width_(width), height_(height), swapchain_(ctx.swapchainBootStraap_.swapchain)
	{
		//1. Using vkb::Bootstrap to get it surface information and max numbers of images.
		surfaceFormat_ = { ctx.swapchainBootStraap_.image_format , ctx.swapchainBootStraap_.color_space };
		numSwapchainImages_ = ctx.swapchainBootStraap_.image_count;

		//2.Acquire the SwapChain semaphore. 
		acquireSemaphore_ = createSemaphore(device_, "Semaphore: swapchain-acquire");

		//3. Building VulkanImages Image are building first with vkb::SwapchainBuilder 
		char debugNameImage[ 256 ] = { 0 };
		char debugNameImageView[ 256 ] = { 0 };

		for(uint32_t i = 0; i < numSwapchainImages_; i++)
		{
			snprintf(debugNameImage, sizeof(debugNameImage) - 1, "Image: swapchain %u", i);
			snprintf(debugNameImageView, sizeof(debugNameImageView) - 1, "Image View: swapchain %u", i);
			VulkanImage image = {
				 .vkImage_ = ctx.swapchainBootStraap_.get_images().value()[ i ],
				 .vkUsageFlags_ = ctx.swapchainBootStraap_.image_usage_flags,
				 .vkExtent_ = VkExtent3D{.width = width_, .height = height_, .depth = 1},
				 .vkType_ = VK_IMAGE_TYPE_2D,
				 .vkImageFormat_ = surfaceFormat_.format,
				 .isSwapchainImage_ = true,
				 .isOwningVkImage_ = false,
				 .isDepthFormat_ = VulkanImage::isDepthFormat(surfaceFormat_.format),
				 .isStencilFormat_ = VulkanImage::isStencilFormat(surfaceFormat_.format),
			};

			VK_ASSERT(setDebugObjectName(device_, VK_OBJECT_TYPE_IMAGE, (uint64_t)image.vkImage_, debugNameImage));

			image.imageView_ = image.createImageView(device_,
				VK_IMAGE_VIEW_TYPE_2D,
				surfaceFormat_.format,
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				VK_REMAINING_MIP_LEVELS,
				0,
				1,
				{},
				nullptr,
				debugNameImageView);

			swapchainTextures_[ i ] = image;
		}//end for loop

	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		for(auto& image : swapchainTextures_)
		{
			vkDestroyImageView(device_, image.imageView_, nullptr);
		}
		vkDestroySemaphore(device_, acquireSemaphore_, nullptr);
		vkDestroyFence(device_, acquireFence_, nullptr);
	}

	VkResult VulkanSwapchain::present(VkSemaphore waitSemaphore)
	{

		const VkPresentInfoKHR pi = {
			 .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			 .waitSemaphoreCount = 1,
			 .pWaitSemaphores = &waitSemaphore,
			 .swapchainCount = 1u,
			 .pSwapchains = &swapchain_,
			 .pImageIndices = &currentImageIndex_,
		};
		VkResult r = vkQueuePresentKHR(graphicsQueue_, &pi);
		if(r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR && r != VK_ERROR_OUT_OF_DATE_KHR)
		{
			VK_ASSERT(r);
		}
		getNextImage_ = true;

		//Return error 
		return r;
	}

	VulkanImage VulkanSwapchain::getCurrentTexture()
	{
		if(getNextImage_)
		{
			// Our first submit handle can be still waiting on the previous `acquireSemaphore`.
			//   vkAcquireNextImageKHR():  Semaphore must not have any pending operations. The Vulkan spec states:
			//   If semaphore is not VK_NULL_HANDLE it must not have any uncompleted signal or wait operations pending
			//   (https://vulkan.lunarg.com/doc/view/1.3.275.0/windows/1.3-extensions/vkspec.html#VUID-vkAcquireNextImageKHR-semaphore-01779)
			if(acquireFence_ == VK_NULL_HANDLE)
			{
				acquireFence_ = createFence(device_, "Fence: swapchain-acquire");
			}
			else
			{
				vkWaitForFences(device_, 1, &acquireFence_, VK_TRUE, UINT64_MAX);
				vkResetFences(device_, 1, &acquireFence_);
			}
			// when timeout is set to UINT64_MAX, we wait until the next image has been acquired
			VkResult r = vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, acquireSemaphore_, acquireFence_, &currentImageIndex_);
			if(r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR && r != VK_ERROR_OUT_OF_DATE_KHR)
			{
				VK_ASSERT(r);
			}
			getNextImage_ = false;
			ctx_.immediate_->waitSemaphore(acquireSemaphore_);
		}

		if(currentImageIndex_ < numSwapchainImages_)
		{
			return swapchainTextures_[ currentImageIndex_ ];
		}
		return {};
	}

	uint32_t VulkanSwapchain::getCurrentImageIndex() const
	{
		return currentImageIndex_;
	}

	const VkSurfaceFormatKHR& VulkanSwapchain::getSurfaceFormat() const
	{
		//GetSurfaceFormat in VulkanSwapchain
		return surfaceFormat_;
	}







	//-----------------------------------------------//
	// CommandBuffer METHODS
	//-----------------------------------------------//
	Vulkan::CommandBuffer::CommandBuffer(DeviceVulkan* ctx): ctx_(ctx), wrapper_(&ctx->immediate_->acquire())
	{}

	CommandBuffer::~CommandBuffer()
	{


	}



	//-----------------------------------------------//
	// VulkanImage METHODS
	//-----------------------------------------------//

	VkImageView VulkanImage::createImageView(VkDevice device, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectMask, uint32_t baseLevel, uint32_t numLevels, uint32_t baseLayer, uint32_t numLayers, const VkComponentMapping mapping, const VkSamplerYcbcrConversionInfo* ycbcr, const char* debugName) const
	{

		const VkImageViewCreateInfo ci = {
			 .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			 .pNext = ycbcr,
			 .image = vkImage_,
			 .viewType = type,
			 .format = format,
			 .components = mapping,
			 .subresourceRange = {aspectMask, baseLevel, numLevels ? numLevels : numLevels_, baseLayer, numLayers},
		};
		VkImageView vkView = VK_NULL_HANDLE;
		VK_ASSERT(vkCreateImageView(device, &ci, nullptr, &vkView));
		VK_ASSERT(setDebugObjectName(device, VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)vkView, debugName));

		return vkView;
	}

	void VulkanImage::transitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, const VkImageSubresourceRange& subresourceRange) const
	{


		VkAccessFlags srcAccessMask = 0;
		VkAccessFlags dstAccessMask = 0;

		if(vkImageLayout_ == VK_IMAGE_LAYOUT_UNDEFINED)
		{
			// we do not need to wait for any previous operations in this case
			srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}

		const VkPipelineStageFlags doNotRequireAccessMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
			VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		VkPipelineStageFlags srcRemainingMask = srcStageMask & ~doNotRequireAccessMask;
		VkPipelineStageFlags dstRemainingMask = dstStageMask & ~doNotRequireAccessMask;

		if(srcStageMask & VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT)
		{
			srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			srcRemainingMask &= ~VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		}
		if(srcStageMask & VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
		{
			srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			srcRemainingMask &= ~VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		if(srcStageMask & VK_PIPELINE_STAGE_TRANSFER_BIT)
		{
			srcAccessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
			srcRemainingMask &= ~VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		if(srcStageMask & VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT)
		{
			srcAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
			srcRemainingMask &= ~VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		if(srcStageMask & VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
		{
			srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			srcRemainingMask &= ~VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}

		IFNITY_ASSERT_MSG(srcRemainingMask == 0, "Automatic access mask deduction is not implemented (yet) for this srcStageMask");

		if(dstStageMask & VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT)
		{
			dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
			dstAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
			dstRemainingMask &= ~VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		if(dstStageMask & VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT)
		{
			dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dstRemainingMask &= ~VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		}
		if(dstStageMask & VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
		{
			dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dstRemainingMask &= ~VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		if(dstStageMask & VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
		{
			dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
			dstAccessMask |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
			dstRemainingMask &= ~VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		if(dstStageMask & VK_PIPELINE_STAGE_TRANSFER_BIT)
		{
			dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
			dstRemainingMask &= ~VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		if(dstStageMask & VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR)
		{
			dstAccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
			dstRemainingMask &= ~VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
		}

		IFNITY_ASSERT_MSG(dstRemainingMask == 0, "Automatic access mask deduction is not implemented (yet) for this dstStageMask");

		imageMemoryBarrier(
			commandBuffer, vkImage_, srcAccessMask, dstAccessMask, vkImageLayout_, newImageLayout, srcStageMask, dstStageMask, subresourceRange);

		vkImageLayout_ = newImageLayout;


	}

	bool VulkanImage::isDepthFormat(VkFormat format)
	{
		return (format == VK_FORMAT_D16_UNORM) || (format == VK_FORMAT_X8_D24_UNORM_PACK32) || (format == VK_FORMAT_D32_SFLOAT) ||
			(format == VK_FORMAT_D16_UNORM_S8_UINT) || (format == VK_FORMAT_D24_UNORM_S8_UINT) || (format == VK_FORMAT_D32_SFLOAT_S8_UINT);
	}

	bool VulkanImage::isStencilFormat(VkFormat format)
	{
		return (format == VK_FORMAT_S8_UINT) || (format == VK_FORMAT_D16_UNORM_S8_UINT) || (format == VK_FORMAT_D24_UNORM_S8_UINT) ||
			(format == VK_FORMAT_D32_SFLOAT_S8_UINT);
	}




	//-----------------------------------------------//
	// VulkanImmediateCommands METHODS
	//-----------------------------------------------//


	VulkanImmediateCommands::VulkanImmediateCommands(VkDevice device, uint32_t queueFamilyIndex, const char* debugName):
		device_(device), queueFamilyIndex_(queueFamilyIndex), debugName_(debugName)
	{
		//1. Get the queue family index
		vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue_);

		//2. Create command pool info with reset command buffer and transient bit and queue family index
		const VkCommandPoolCreateInfo ci = {
			 .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			 .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			 .queueFamilyIndex = queueFamilyIndex,
		};


		VK_CHECK(vkCreateCommandPool(device, &ci, nullptr, &commandPool_), "Failed create command pool ");
		setDebugObjectName(device, VK_OBJECT_TYPE_COMMAND_POOL, (uint64_t)commandPool_, debugName);

		//3. Create command buffer allocate info with primary level and command buffer count 1
		const VkCommandBufferAllocateInfo ai = {
			 .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			 .commandPool = commandPool_,
			 .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			 .commandBufferCount = 1,
		};

		// 4. Allocate command buffer. Allocating one command buffer at a time to ensure safety and avoid potential issues with large allocations. 
		// Setting .commandBufferCount = kMaxCommandBuffers might lead to allocation failures or performance issues in some environments.
		for(uint32_t i = 0; i != kMaxCommandBuffers; i++)
		{
			auto& buf = buffers_[ i ];
			char fenceName[ 256 ] = { 0 };
			char semaphoreName[ 256 ] = { 0 };
			if(debugName)
			{
				snprintf(fenceName, sizeof(fenceName) - 1, "Fence: %s (cmdbuf %u)", debugName, i);
				snprintf(semaphoreName, sizeof(semaphoreName) - 1, "Semaphore: %s (cmdbuf %u)", debugName, i);
			}
			buf.semaphore_ = createSemaphore(device, semaphoreName);
			buf.fence_ = createFence(device, fenceName);
			VK_CHECK(vkAllocateCommandBuffers(device, &ai, &buf.cmdBufAllocated_), "Error allocate commandbuffer %u", i);
			buffers_[ i ].handle_.bufferIndex_ = i;
		}
	}

	VulkanImmediateCommands::~VulkanImmediateCommands()
	{
		// 1. Wait for all command buffers to finish executing TODO: DANGER
		waitAll();

		for(auto& buf : buffers_)
		{
			// lifetimes of all VkFence objects are managed explicitly we do not use deferredTask() for them
			vkDestroyFence(device_, buf.fence_, nullptr);
			vkDestroySemaphore(device_, buf.semaphore_, nullptr);
		}

		vkDestroyCommandPool(device_, commandPool_, nullptr);

	}

	void VulkanImmediateCommands::purge()
	{
		const uint32_t numBuffers = static_cast<uint32_t>(ARRAY_NUM_ELEMENTS(buffers_));

		for(uint32_t i = 0; i != numBuffers; i++)
		{
			// always start checking with the oldest submitted buffer, then wrap around and avoid out range access.
			CommandBufferWrapper& buf = buffers_[ (i + lastSubmitHandle_.bufferIndex_ + 1) % numBuffers ];

			if(buf.cmdBuf_ == VK_NULL_HANDLE || buf.isEncoding_)
			{
				continue;
			}

			const VkResult result = vkWaitForFences(device_, 1, &buf.fence_, VK_TRUE, 0);

			if(result == VK_SUCCESS)
			{
				VK_CHECK(vkResetCommandBuffer(buf.cmdBuf_, VkCommandBufferResetFlags{ 0 }), "Fail Reset CommandBuffer while purge cmdb function");
				VK_CHECK(vkResetFences(device_, 1, &buf.fence_), "Fail Reset Fences while purge its running");
				buf.cmdBuf_ = VK_NULL_HANDLE;
				numAvailableCommandBuffers_++;
			}
			else
			{
				if(result != VK_TIMEOUT)
				{
					VK_CHECK(result, "Error TIMEOUT while purge its running and WaitForFences ");
				}
			}
		}
	}


	const VulkanImmediateCommands::CommandBufferWrapper& VulkanImmediateCommands::acquire()
	{
		// 1. Check if there are any available command buffers	if not, wait until one becomes available with purge()
		if(!numAvailableCommandBuffers_)
		{
			purge();
		}

		while(!numAvailableCommandBuffers_)
		{
			IFNITY_LOG(LogCore, INFO, "Waiting for command buffers...\n");
			purge();
		}

		VulkanImmediateCommands::CommandBufferWrapper* current = nullptr;

		// we are ok with any available buffer, if cmdbuf is VK_NULL_HANDLE, it is available
		for(auto& buf : buffers_)
		{
			if(buf.cmdBuf_ == VK_NULL_HANDLE)
			{
				current = &buf;
				break;
			}
		}

		//Check Operations 
		assert(current);
		assert(numAvailableCommandBuffers_, "No available command buffers");
		assert(current, "No available command buffers");
		assert(current->cmdBufAllocated_ != VK_NULL_HANDLE);

		current->handle_.submitId_ = submitCounter_;
		numAvailableCommandBuffers_--;

		current->cmdBuf_ = current->cmdBufAllocated_;
		current->isEncoding_ = true;
		const VkCommandBufferBeginInfo bi = {
			 .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			 .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};
		VK_CHECK(vkBeginCommandBuffer(current->cmdBuf_, &bi), "Error begin command buffer");

		nextSubmitHandle_ = current->handle_;

		return *current;

	}

	void VulkanImmediateCommands::wait(const SubmitHandle handle)
	{


		if(handle.empty())
		{
			vkDeviceWaitIdle(device_);
			return;
		}

		if(isReady(handle))
		{
			return;
		}

		if(!buffers_[ handle.bufferIndex_ ].isEncoding_)
		{
			// we are waiting for a buffer which has not been submitted - this is probably a logic error somewhere in the calling code
			IFNITY_LOG(LogCore, ERROR, "Waiting for a buffer which has not been submitted , this is a probably logic error somewhere in the calling code");
			return;
		}

		VK_ASSERT(vkWaitForFences(device_, 1, &buffers_[ handle.bufferIndex_ ].fence_, VK_TRUE, UINT64_MAX));

		purge();
	}

	void VulkanImmediateCommands::waitAll()
	{


		VkFence fences[ kMaxCommandBuffers ];

		uint32_t numFences = 0;

		for(const auto& buf : buffers_)
		{
			if(buf.cmdBuf_ != VK_NULL_HANDLE && !buf.isEncoding_)
			{
				fences[ numFences++ ] = buf.fence_;
			}
		}

		if(numFences)
		{
			VK_ASSERT(vkWaitForFences(device_, numFences, fences, VK_TRUE, UINT64_MAX));
		}

		purge();
	}

	bool VulkanImmediateCommands::isReady(const SubmitHandle handle, bool fastCheckNoVulkan) const
	{
		assert(handle.bufferIndex_ < kMaxCommandBuffers);

		if(handle.empty())
		{
			// a null handle
			return true;
		}

		const CommandBufferWrapper& buf = buffers_[ handle.bufferIndex_ ];

		if(buf.cmdBuf_ == VK_NULL_HANDLE)
		{
			// already recycled and not yet reused
			return true;
		}

		if(buf.handle_.submitId_ != handle.submitId_)
		{
			// already recycled and reused by another command buffer
			return true;
		}

		if(fastCheckNoVulkan)
		{
			// do not ask the Vulkan API about it, just let it retire naturally (when submitId for this bufferIndex gets incremented)
			return false;
		}

		VkResult fenceStatus = vkGetFenceStatus(device_, buf.fence_);
		if(fenceStatus == VK_SUCCESS)
		{
			return true;
		}
		return false;

	}

	SubmitHandle VulkanImmediateCommands::submit(const CommandBufferWrapper& wrapper)
	{

		assert(wrapper.isEncoding_);
		VK_ASSERT(vkEndCommandBuffer(wrapper.cmdBuf_));

		const VkPipelineStageFlags waitStageMasks[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
		VkSemaphore waitSemaphores[] = { VK_NULL_HANDLE, VK_NULL_HANDLE };
		uint32_t numWaitSemaphores = 0;
		if(waitSemaphore_)
		{
			waitSemaphores[ numWaitSemaphores++ ] = waitSemaphore_;
		}
		if(lastSubmitSemaphore_)
		{
			waitSemaphores[ numWaitSemaphores++ ] = lastSubmitSemaphore_;
		}


		const VkSubmitInfo si = {
			 .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			 .waitSemaphoreCount = numWaitSemaphores,
			 .pWaitSemaphores = numWaitSemaphores ? waitSemaphores : nullptr,
			 .pWaitDstStageMask = waitStageMasks,
			 .commandBufferCount = 1u,
			 .pCommandBuffers = &wrapper.cmdBuf_,
			 .signalSemaphoreCount = 1u,
			 .pSignalSemaphores = &wrapper.semaphore_,
		};
		VK_ASSERT(vkQueueSubmit(queue_, 1u, &si, wrapper.fence_));


		lastSubmitSemaphore_ = wrapper.semaphore_;
		lastSubmitHandle_ = wrapper.handle_;
		waitSemaphore_ = VK_NULL_HANDLE;

		// reset
		const_cast<CommandBufferWrapper&>(wrapper).isEncoding_ = false;
		submitCounter_++;

		if(!submitCounter_)
		{
			// skip the 0 value - when uint32_t wraps around (null SubmitHandle)
			submitCounter_++;
		}

		return lastSubmitHandle_;
	}

	void VulkanImmediateCommands::waitSemaphore(VkSemaphore semaphore)
	{
		assert(waitSemaphore_ == VK_NULL_HANDLE);

		waitSemaphore_ = semaphore;
	}

	VkSemaphore VulkanImmediateCommands::acquireLastSubmitSemaphore()
	{
		return std::exchange(lastSubmitSemaphore_, VK_NULL_HANDLE);
	}

	VkFence VulkanImmediateCommands::getVkFence(SubmitHandle handle) const
	{
		if(handle.empty())
		{
			return VK_NULL_HANDLE;
		}

		return buffers_[ handle.bufferIndex_ ].fence_;
	}

	SubmitHandle VulkanImmediateCommands::getLastSubmitHandle() const
	{
		return lastSubmitHandle_;
	}

	SubmitHandle VulkanImmediateCommands::getNextSubmitHandle() const
	{
		return nextSubmitHandle_;
	}

	//------------------------------------------------------------------------------------------//
	//VULKAN BUILDER DEFINITION METHODS	
	//------------------------------------------------------------------------------------------//

	VulkanPipelineBuilder::VulkanPipelineBuilder()
	{
		// Configuración de vertexInputState_
		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = 0;
		vertexInputState.pVertexBindingDescriptions = nullptr;
		vertexInputState.vertexAttributeDescriptionCount = 0;
		vertexInputState.pVertexAttributeDescriptions = nullptr;
		vertexInputState_ = vertexInputState;

		// Configuración de inputAssembly_
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.flags = 0;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		inputAssembly_ = inputAssembly;

		// Configuración de tessellationState_
		VkPipelineTessellationStateCreateInfo tessellationState = {};
		tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		tessellationState.flags = 0;
		tessellationState.patchControlPoints = 0;
		tessellationState_ = tessellationState;

		// Configuración de rasterizationState_
		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.flags = 0;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.depthBiasConstantFactor = 0.0f;
		rasterizationState.depthBiasClamp = 0.0f;
		rasterizationState.depthBiasSlopeFactor = 0.0f;
		rasterizationState.lineWidth = 1.0f;
		rasterizationState_ = rasterizationState;

		// Configuración de multisampleState_
		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.minSampleShading = 0.0f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;
		multisampleState_ = multisampleState;

		// Configuración de depthStencilState_
		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.pNext = nullptr;
		depthStencilState.flags = 0;
		depthStencilState.depthTestEnable = VK_FALSE;
		depthStencilState.depthWriteEnable = VK_FALSE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front =
		{
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.depthFailOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_NEVER,
			.compareMask = 0,
			.writeMask = 0,
			.reference = 0,
		};
		depthStencilState.back =
		{
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.depthFailOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_NEVER,
			.compareMask = 0,
			.writeMask = 0,
			.reference = 0,
		};
		depthStencilState.minDepthBounds = 0.0f;
		depthStencilState.maxDepthBounds = 1.0f;
		depthStencilState_ = depthStencilState;
	}

	VulkanPipelineBuilder& VulkanPipelineBuilder::dynamicState(VkDynamicState state)
	{
		if(numDynamicStates_ < LVK_MAX_DYNAMIC_STATES)
		{
			dynamicStates_[ numDynamicStates_++ ] = state;
		}
		return *this;
	}

	VulkanPipelineBuilder& VulkanPipelineBuilder::primitiveTopology(VkPrimitiveTopology topology)
	{
		inputAssembly_.topology = topology;
		return *this;
	}

	VulkanPipelineBuilder& VulkanPipelineBuilder::rasterizationSamples(VkSampleCountFlagBits samples, float minSampleShading)
	{
		multisampleState_.rasterizationSamples = samples;
		multisampleState_.sampleShadingEnable = minSampleShading > 0.0f ? VK_TRUE : VK_FALSE;
		multisampleState_.minSampleShading = minSampleShading;
		return *this;
	}


	VulkanPipelineBuilder& VulkanPipelineBuilder::shaderStage(VkPipelineShaderStageCreateInfo stage)
	{
		if(stage.module != VK_NULL_HANDLE)
		{
			IFNITY_ASSERT_MSG(numShaderStages_ < ARRAY_NUM_ELEMENTS(shaderStages_), "Too many shader stages");
			shaderStages_[ numShaderStages_++ ] = stage;
		}
		return *this;
	}


	VulkanPipelineBuilder& VulkanPipelineBuilder::stencilStateOps(VkStencilFaceFlags faceMask,
		VkStencilOp failOp,
		VkStencilOp passOp,
		VkStencilOp depthFailOp,
		VkCompareOp compareOp)
	{
		depthStencilState_.stencilTestEnable = depthStencilState_.stencilTestEnable == VK_TRUE ||
			failOp != VK_STENCIL_OP_KEEP ||
			passOp != VK_STENCIL_OP_KEEP ||
			depthFailOp != VK_STENCIL_OP_KEEP ||
			compareOp != VK_COMPARE_OP_ALWAYS
			? VK_TRUE
			: VK_FALSE;

		if(faceMask & VK_STENCIL_FACE_FRONT_BIT)
		{
			VkStencilOpState& s = depthStencilState_.front;
			s.failOp = failOp;
			s.passOp = passOp;
			s.depthFailOp = depthFailOp;
			s.compareOp = compareOp;
		}

		if(faceMask & VK_STENCIL_FACE_BACK_BIT)
		{
			VkStencilOpState& s = depthStencilState_.back;
			s.failOp = failOp;
			s.passOp = passOp;
			s.depthFailOp = depthFailOp;
			s.compareOp = compareOp;
		}
		return *this;
	}

	VulkanPipelineBuilder& VulkanPipelineBuilder::stencilMasks(VkStencilFaceFlags faceMask,
		uint32_t compareMask,
		uint32_t writeMask,
		uint32_t reference)
	{
		if(faceMask & VK_STENCIL_FACE_FRONT_BIT)
		{
			VkStencilOpState& s = depthStencilState_.front;
			s.compareMask = compareMask;
			s.writeMask = writeMask;
			s.reference = reference;
		}

		if(faceMask & VK_STENCIL_FACE_BACK_BIT)
		{
			VkStencilOpState& s = depthStencilState_.back;
			s.compareMask = compareMask;
			s.writeMask = writeMask;
			s.reference = reference;
		}
		return *this;
	}

	//Cull mode 
	VulkanPipelineBuilder& VulkanPipelineBuilder::cullMode(VkCullModeFlags mode)
	{
		rasterizationState_.cullMode = mode;
		return *this;
	}


	//Front face
	VulkanPipelineBuilder& VulkanPipelineBuilder::frontFace(VkFrontFace mode)
	{
		rasterizationState_.frontFace = mode;
		return *this;
	}

	//Polygon mode
	VulkanPipelineBuilder& VulkanPipelineBuilder::polygonMode(VkPolygonMode mode)
	{
		rasterizationState_.polygonMode = mode;
		return *this;
	}


	// color attachments 
	VulkanPipelineBuilder& VulkanPipelineBuilder::colorAttachments(const VkPipelineColorBlendAttachmentState* states,
		const VkFormat* formats,
		uint32_t numColorAttachments)
	{
		assert(states);
		assert(formats);
		assert(numColorAttachments <= ARRAY_NUM_ELEMENTS(colorBlendAttachmentStates_));
		assert(numColorAttachments <= ARRAY_NUM_ELEMENTS(colorAttachmentFormats_));
		for(uint32_t i = 0; i != numColorAttachments; i++)
		{
			colorBlendAttachmentStates_[ i ] = states[ i ];
			colorAttachmentFormats_[ i ] = formats[ i ];
		}
		numColorAttachments_ = numColorAttachments;
		return *this;
	}


	// depth Attachment Format 
	VulkanPipelineBuilder& VulkanPipelineBuilder::depthAttachmentFormat(VkFormat format)
	{
		depthAttachmentFormat_ = format;
		return *this;
	}

	// stencil Attachment Format
	VulkanPipelineBuilder& VulkanPipelineBuilder::stencilAttachmentFormat(VkFormat format)
	{
		stencilAttachmentFormat_ = format;
		return *this;
	}

	//Vertex Input State
	VulkanPipelineBuilder& VulkanPipelineBuilder::vertexInputState(const VkPipelineVertexInputStateCreateInfo& state)
	{
		vertexInputState_ = state;
		return *this;
	}

	//Build the Graphics Pipeline

	VkResult VulkanPipelineBuilder::build(VkDevice device,
		VkPipelineCache pipelineCache,
		VkPipelineLayout pipelineLayout,
		VkPipeline* outPipeline, const char* debugName) noexcept
	{
		const VkPipelineDynamicStateCreateInfo dynamicState =
		{
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		 .dynamicStateCount = numDynamicStates_,
		 .pDynamicStates = dynamicStates_,
		};
		// viewport and scissor can be NULL if the viewport state is dynamic
		// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkPipelineViewportStateCreateInfo.html
		const VkPipelineViewportStateCreateInfo viewportState =
		{
			 .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			 .viewportCount = 1,
			 .pViewports = nullptr,
			 .scissorCount = 1,
			 .pScissors = nullptr,
		};
		const VkPipelineColorBlendStateCreateInfo colorBlendState = 
		{
			 .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			 .logicOpEnable = VK_FALSE,
			 .logicOp = VK_LOGIC_OP_COPY,
			 .attachmentCount = numColorAttachments_,
			 .pAttachments = colorBlendAttachmentStates_,
		};
		
		//Using the dynamic rendering , not uses renderpass . 
		const VkPipelineRenderingCreateInfo renderingInfo =
		{
			 .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
			 .pNext = nullptr,
			 .colorAttachmentCount = numColorAttachments_,
			 .pColorAttachmentFormats = colorAttachmentFormats_,
			 .depthAttachmentFormat = depthAttachmentFormat_,
			 .stencilAttachmentFormat = stencilAttachmentFormat_,
		};

		const VkGraphicsPipelineCreateInfo ci = {
			 .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			 .pNext = &renderingInfo,
			 .flags = 0,
			 .stageCount = numShaderStages_,
			 .pStages = shaderStages_,
			 .pVertexInputState = &vertexInputState_,
			 .pInputAssemblyState = &inputAssembly_,
			 .pTessellationState = &tessellationState_,
			 .pViewportState = &viewportState,
			 .pRasterizationState = &rasterizationState_,
			 .pMultisampleState = &multisampleState_,
			 .pDepthStencilState = &depthStencilState_,
			 .pColorBlendState = &colorBlendState,
			 .pDynamicState = &dynamicState,
			 .layout = pipelineLayout,
			 .renderPass = VK_NULL_HANDLE,
			 .subpass = 0,
			 .basePipelineHandle = VK_NULL_HANDLE,
			 .basePipelineIndex = -1,
		};

		const auto result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &ci, nullptr, outPipeline);

		if(!(result == VK_SUCCESS))
		{
			return result;
		}

		numPipelinesCreated_++;

		// set debug name
		// set debug name
		return setDebugObjectName(device, VK_OBJECT_TYPE_PIPELINE, (uint64_t)*outPipeline, debugName);
	}


	//------------------------------------------------------------------------------------------//

}
IFNITY_END_NAMESPACE