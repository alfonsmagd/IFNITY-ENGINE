

#include "vulkan_VulkanImage.hpp"
#include "../Windows/UtilsVulkan.h"
#include "../Windows/DeviceVulkan.h"
#include "vulkan_classes.hpp"

IFNITY_NAMESPACE


namespace Vulkan
{

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

	void VulkanImage::transitionLayout(VkCommandBuffer commandBuffer,
		VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask,
		const VkImageSubresourceRange& subresourceRange) const
	{


		VkAccessFlags srcAccessMask = 0;
		VkAccessFlags dstAccessMask = 0;

		if(vkImageLayout_ == VK_IMAGE_LAYOUT_UNDEFINED)
		{
			// we do not need to wait for any previous operations in this case
			srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}

		if(vkImageLayout_ == VK_IMAGE_LAYOUT_UNDEFINED)
		{
			// we do not need to wait for any previous operations in this case
			srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}

		const VkPipelineStageFlags doNotRequireAccessMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
			VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		VkPipelineStageFlags srcRemainingMask = srcStageMask & ~doNotRequireAccessMask;
		VkPipelineStageFlags dstRemainingMask = dstStageMask & ~doNotRequireAccessMask;

		// This lambda updates the access mask and remaining stages for a given pipeline stage.
		// It checks if the stage bit is set in the stage mask, updates the access mask, and removes the stage bit from the remaining mask.

		auto updateAccessMask = [](VkPipelineStageFlags stageMask,
			VkAccessFlags& accessMask,
			VkPipelineStageFlags& remainingMask,
			VkPipelineStageFlags stageBit,
			VkAccessFlags accessBit)
			{
				if(stageMask & stageBit)
				{
					accessMask |= accessBit;
					remainingMask &= ~stageBit;
				}
			};

		updateAccessMask(srcStageMask, srcAccessMask, srcRemainingMask,
			VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
		updateAccessMask(srcStageMask, srcAccessMask, srcRemainingMask,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
		updateAccessMask(srcStageMask, srcAccessMask, srcRemainingMask,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT);
		updateAccessMask(srcStageMask, srcAccessMask, srcRemainingMask,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VK_ACCESS_SHADER_WRITE_BIT);
		updateAccessMask(srcStageMask, srcAccessMask, srcRemainingMask,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

		//IFNITY_ASSERT_MSG(srcRemainingMask == 0, "Automatic access mask deduction is not implemented (yet) for this srcStageMask");

		updateAccessMask(dstStageMask, dstAccessMask, dstRemainingMask, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);
		updateAccessMask(dstStageMask, dstAccessMask, dstRemainingMask, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
		updateAccessMask(dstStageMask, dstAccessMask, dstRemainingMask, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
		updateAccessMask(dstStageMask, dstAccessMask, dstRemainingMask, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT);
		updateAccessMask(dstStageMask, dstAccessMask, dstRemainingMask, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT);
		updateAccessMask(dstStageMask, dstAccessMask, dstRemainingMask, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);

		//IFNITY_ASSERT_MSG(dstRemainingMask == 0, "Automatic access mask deduction is not implemented (yet) for this dstStageMask");


		imageMemoryBarrier(
			commandBuffer,
			vkImage_,
			srcAccessMask,
			dstAccessMask,
			vkImageLayout_,
			newImageLayout,
			srcStageMask,
			dstStageMask,
			subresourceRange);

		vkImageLayout_ = newImageLayout;


	}

	VkImageAspectFlags VulkanImage::getImageAspectFlags() const
	{
		VkImageAspectFlags flags = 0;

		flags |= isDepthFormat_ ? VK_IMAGE_ASPECT_DEPTH_BIT : 0;
		flags |= isStencilFormat_ ? VK_IMAGE_ASPECT_STENCIL_BIT : 0;
		flags |= !(isDepthFormat_ || isStencilFormat_) ? VK_IMAGE_ASPECT_COLOR_BIT : 0;

		return flags;
	}

	VkImageView VulkanImage::getOrCreateVkImageViewForFramebuffer(DeviceVulkan& ctx, uint8_t level, uint16_t layer)
	{
		_ASSERT(level < MAX_MIP_LEVELS);
		_ASSERT(layer < ARRAY_NUM_ELEMENTS(imageViewForFramebuffer_[ 0 ]));

		if(level >= MAX_MIP_LEVELS || layer >= ARRAY_NUM_ELEMENTS(imageViewForFramebuffer_[ 0 ]))
		{
			return VK_NULL_HANDLE;
		}

		if(imageViewForFramebuffer_[ level ][ layer ] != VK_NULL_HANDLE)
		{
			return imageViewForFramebuffer_[ level ][ layer ];
		}

		imageViewForFramebuffer_[ level ][ layer ] =
			createImageView(ctx.device_, VK_IMAGE_VIEW_TYPE_2D, vkImageFormat_, getImageAspectFlags(), level, 1u, layer, 1u);

		return imageViewForFramebuffer_[ level ][ layer ];
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





}

IFNITY_END_NAMESPACE