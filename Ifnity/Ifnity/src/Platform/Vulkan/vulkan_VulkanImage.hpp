#pragma once

#include <pch.h>
#include <VkBootstrap.h>
#include "vk_mem_alloc.h"
#include "../Windows/vk_constans.hpp"


IFNITY_NAMESPACE



class DeviceVulkan;
namespace Vulkan
{

	//Forward declaration
	
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

		[[nodiscard]] VkImageAspectFlags getImageAspectFlags() const;
		// framebuffers can render only into one level/layer
		[[nodiscard]] VkImageView getOrCreateVkImageViewForFramebuffer(DeviceVulkan& ctx, uint8_t level, uint16_t layer);
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
		VkImageView imageViewForFramebuffer_[ MAX_MIP_LEVELS ][ 1 ] = {}; // max 6 faces for cubemap rendering

	};

	
	

} // namespace Vulkan

IFNITY_END_NAMESPACE