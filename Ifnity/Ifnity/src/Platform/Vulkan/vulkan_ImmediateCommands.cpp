

#include "vulkan_ImmediateCommands.hpp"
#include "../Windows/UtilsVulkan.h"
#include "../Windows/DeviceVulkan.h"


IFNITY_NAMESPACE


namespace Vulkan
{


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
			//IFNITY_LOG(LogCore, INFO, "Waiting for command buffers...\n");
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



}

IFNITY_END_NAMESPACE