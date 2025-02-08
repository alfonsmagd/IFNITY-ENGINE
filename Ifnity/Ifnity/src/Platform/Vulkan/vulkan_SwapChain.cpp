

#include "vulkan_SwapChain.hpp"
#include "../Windows/UtilsVulkan.h"
#include "../Windows/DeviceVulkan.h"


IFNITY_NAMESPACE


namespace Vulkan
{

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
			swapchainTextureHandles_[ i ] = ctx.slootMapTextures_.create(std::move(image));
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

	VulkanImage* VulkanSwapchain::getCurrentTexture()
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
			return &swapchainTextures_[ currentImageIndex_ ];
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




}

IFNITY_END_NAMESPACE