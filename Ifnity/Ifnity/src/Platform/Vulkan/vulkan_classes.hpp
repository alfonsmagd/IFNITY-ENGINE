#pragma once

//This Vulkan Classes are based on lightweight vulkan https://github.com/corporateshark/lightweightvk

#include <pch.h>
#include <VkBootstrap.h>
#include "vk_mem_alloc.h"
#include <Ifnity\Graphics\Interfaces\ITexture.hpp>



IFNITY_NAMESPACE

class DeviceVulkan;


namespace Vulkan
{
	struct DeviceQueues final
	{
		const static uint32_t INVALID = 0xFFFFFFFF;
		uint32_t graphicsQueueFamilyIndex = INVALID;
		uint32_t computeQueueFamilyIndex = INVALID;
		uint32_t presentQueueFamilyIndex = INVALID;

		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue computeQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
	};


	//Get struct 
	struct SubmitHandle
	{
		uint32_t bufferIndex_ = 0; // Index buffer
		uint32_t submitId_ = 0;	// Submit ID
		SubmitHandle() = default;
		explicit SubmitHandle(uint64_t handle): bufferIndex_(uint32_t(handle & 0xffffffff)),
			submitId_(uint32_t(handle >> 32))
		{
			assert(submitId_);
		}
		bool empty() const
		{
			return submitId_ == 0;
		}
		//Return handle in 64 bit with information about buffer index and submit ID
		uint64_t handle() const
		{
			return (uint64_t(submitId_) << 32) + bufferIndex_;
		}
	};

	//-----------------------------------------------//
	// VulkanImmediateCommands 
	//-----------------------------------------------//
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

	//-----------------------------------------------//
	// CommandBuffer
	//-----------------------------------------------//
	class CommandBuffer final
	{
	public:
		CommandBuffer() = default;
		explicit CommandBuffer(DeviceVulkan* ctx);
		~CommandBuffer() ;

		CommandBuffer& operator=(CommandBuffer&& other) = default;

		

	private:
		friend class DeviceVulkan;

		DeviceVulkan* ctx_;
		const VulkanImmediateCommands::CommandBufferWrapper* wrapper_ = nullptr;

	};


	//-----------------------------------------------//
	// VulkanImage class.
	//-----------------------------------------------//
	struct VulkanImage final
	{
		// clang-format off
		[[nodiscard]] inline bool isSampledImage() const { return (vkUsageFlags_ & VK_IMAGE_USAGE_SAMPLED_BIT) > 0; }
		[[nodiscard]] inline bool isStorageImage() const { return (vkUsageFlags_ & VK_IMAGE_USAGE_STORAGE_BIT) > 0; }
		[[nodiscard]] inline bool isColorAttachment() const { return (vkUsageFlags_ & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) > 0; }
		[[nodiscard]] inline bool isDepthAttachment() const { return (vkUsageFlags_ & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) > 0; }
		[[nodiscard]] inline bool isAttachment() const { return (vkUsageFlags_ & (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) > 0; }
		// clang-format on

		/*
		 * Setting `numLevels` to a non-zero value will override `mipLevels_` value from the original Vulkan image, and can be used to create
		 * image views with different number of levels.
		 */
		 [[nodiscard]] VkImageView createImageView(VkDevice device,
		 	VkImageViewType type,
		 	VkFormat format,
		 	VkImageAspectFlags aspectMask,
		 	uint32_t baseLevel,
		 	uint32_t numLevels = VK_REMAINING_MIP_LEVELS,
		 	uint32_t baseLayer = 0,
		 	uint32_t numLayers = 1,
		 	const VkComponentMapping mapping = { .r = VK_COMPONENT_SWIZZLE_IDENTITY,
		 													.g = VK_COMPONENT_SWIZZLE_IDENTITY,
		 													.b = VK_COMPONENT_SWIZZLE_IDENTITY,
		 													.a = VK_COMPONENT_SWIZZLE_IDENTITY },
		 	const VkSamplerYcbcrConversionInfo* ycbcr = nullptr,
		 	const char* debugName = nullptr) const;

		 //void generateMipmap(VkCommandBuffer commandBuffer) const;
		 void transitionLayout(VkCommandBuffer commandBuffer,
			 VkImageLayout newImageLayout,
			 VkPipelineStageFlags srcStageMask,
			 VkPipelineStageFlags dstStageMask,
			 const VkImageSubresourceRange& subresourceRange) const;

		 //[[nodiscard]] VkImageAspectFlags getImageAspectFlags() const;

		 // framebuffers can render only into one level/layer

		[[nodiscard]] static bool isDepthFormat(VkFormat format);
		[[nodiscard]] static bool isStencilFormat(VkFormat format);

	public:
		VkImage vkImage_ = VK_NULL_HANDLE;
		VkImageUsageFlags vkUsageFlags_ = 0;
		VkDeviceMemory vkMemory_[ 3 ] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };
		VmaAllocation vmaAllocation_ = VK_NULL_HANDLE;
		VkFormatProperties vkFormatProperties_ = {};
		VkExtent3D vkExtent_ = { 0, 0, 0 };
		VkImageType vkType_ = VK_IMAGE_TYPE_MAX_ENUM;
		VkFormat vkImageFormat_ = VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits vkSamples_ = VK_SAMPLE_COUNT_1_BIT;
		void* mappedPtr_ = nullptr;
		bool isSwapchainImage_ = false;
		bool isOwningVkImage_ = true;
		uint32_t numLevels_ = 1u;
		uint32_t numLayers_ = 1u;
		bool isDepthFormat_ = false;
		bool isStencilFormat_ = false;
		// current image layout
		mutable VkImageLayout vkImageLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
		// precached image views - owned by this VulkanImage
		VkImageView imageView_ = VK_NULL_HANDLE; // default view with all mip-levels
		VkImageView imageViewStorage_ = VK_NULL_HANDLE; // default view with identity swizzle (all mip-levels)

	};

	//-----------------------------------------------//
	// SwapChain class.  
	//-----------------------------------------------//

	class VulkanSwapchain final
	{
		enum { LVK_MAX_SWAPCHAIN_IMAGES = 16 };

	public:
		VulkanSwapchain(DeviceVulkan& ctx, uint32_t width, uint32_t height);
		~VulkanSwapchain();

		VulkanImage getCurrentTexture();
		/*VkResult present(VkSemaphore waitSemaphore);
		VkImage getCurrentVkImage() const;
		VkImageView getCurrentVkImageView() const;
		
		const VkSurfaceFormatKHR& getSurfaceFormat() const;
		uint32_t getNumSwapchainImages() const;*/

	private:
		DeviceVulkan& ctx_;
		VkDevice device_ = VK_NULL_HANDLE;
		VkQueue graphicsQueue_ = VK_NULL_HANDLE;
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		uint32_t numSwapchainImages_ = 0;
		uint32_t currentImageIndex_ = 0;
		bool getNextImage_ = true;
		VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
		VkSurfaceFormatKHR surfaceFormat_ = { .format = VK_FORMAT_UNDEFINED };
		VulkanImage swapchainTextures_[ LVK_MAX_SWAPCHAIN_IMAGES ] = {};
		VkSemaphore acquireSemaphore_ = VK_NULL_HANDLE;
		VkFence acquireFence_ = VK_NULL_HANDLE;
	};


}

IFNITY_END_NAMESPACE