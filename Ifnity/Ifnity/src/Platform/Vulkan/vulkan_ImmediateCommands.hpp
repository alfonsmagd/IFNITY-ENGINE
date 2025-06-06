#pragma once

#include <pch.h>
#include <VkBootstrap.h>
#include "vulkan_VulkanImage.hpp"
#include "vulkan_classes.hpp"



IFNITY_NAMESPACE

namespace Vulkan
{
	class VulkanImmediateCommands final
	{
	public:
		// the maximum number of command buffers which can similtaneously exist in the system; when we run out of buffers, we stall and wait until
		// an existing buffer becomes available
		static constexpr uint32_t kMaxCommandBuffers = 64;

		VulkanImmediateCommands(VkDevice device, uint32_t queueFamilyIndex, const char* debugName);
		~VulkanImmediateCommands();
		VulkanImmediateCommands(const VulkanImmediateCommands&) = delete;
		VulkanImmediateCommands& operator=(const VulkanImmediateCommands&) = delete;

		struct CommandBufferWrapper
		{
			VkCommandBuffer cmdBuf_ = VK_NULL_HANDLE;
			VkCommandBuffer cmdBufAllocated_ = VK_NULL_HANDLE;
			SubmitHandle handle_ = {};
			VkFence fence_ = VK_NULL_HANDLE;
			VkSemaphore semaphore_ = VK_NULL_HANDLE;
			bool isEncoding_ = false;
		};

		// returns the current command buffer (creates one if it does not exist)
		const CommandBufferWrapper& acquire();
		SubmitHandle submit(const CommandBufferWrapper& wrapper);
		void waitSemaphore(VkSemaphore semaphore);
		VkSemaphore acquireLastSubmitSemaphore();
		VkFence getVkFence(SubmitHandle handle) const;
		SubmitHandle getLastSubmitHandle() const;
		SubmitHandle getNextSubmitHandle() const;
		bool isReady(SubmitHandle handle, bool fastCheckNoVulkan = false) const;
		void wait(SubmitHandle handle);
		void waitAll();

	private:
		void purge();

	private:
		VkDevice device_ = VK_NULL_HANDLE;
		VkQueue queue_ = VK_NULL_HANDLE;
		VkCommandPool commandPool_ = VK_NULL_HANDLE;
		uint32_t queueFamilyIndex_ = 0;
		const char* debugName_ = "";
		CommandBufferWrapper buffers_[ kMaxCommandBuffers ];
		SubmitHandle lastSubmitHandle_ = SubmitHandle();
		SubmitHandle nextSubmitHandle_ = SubmitHandle();
		VkSemaphore lastSubmitSemaphore_ = VK_NULL_HANDLE;
		VkSemaphore waitSemaphore_ = VK_NULL_HANDLE;
		uint32_t numAvailableCommandBuffers_ = kMaxCommandBuffers;
		uint32_t submitCounter_ = 1;
	};


} // namespace Vulkan

IFNITY_END_NAMESPACE