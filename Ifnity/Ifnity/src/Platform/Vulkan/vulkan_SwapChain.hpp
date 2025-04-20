#pragma once

#include <pch.h>
#include <VkBootstrap.h>
#include "vulkan_VulkanImage.hpp"
#include "Ifnity/Utils/SlotMap.hpp"
#include "vulkan_classes.hpp"


IFNITY_NAMESPACE

class DeviceVulkan;

namespace Vulkan
{

	//Forward declaratin
	class VulkanSwapchain final
	{
		enum { LVK_MAX_SWAPCHAIN_IMAGES = 16 };

	public:
		VulkanSwapchain(DeviceVulkan& ctx, uint32_t width, uint32_t height);
		~VulkanSwapchain();


		VkResult present(VkSemaphore waitSemaphore);
		TextureHandleSM getCurrentTexture();
		
		//VkImage getCurrentVkImage() const;
		VkImageView getCurrentVkImageView() const;

		const VkSurfaceFormatKHR& getSurfaceFormat() const;
		//uint32_t getNumSwapchainImages() const;
		uint32_t getCurrentImageIndex() const;
	
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
		VulkanImage swapchainTextures_[ LVK_MAX_SWAPCHAIN_IMAGES ];
		TextureHandleSM swapchainTextureHandles_[ LVK_MAX_SWAPCHAIN_IMAGES ];
		VkSemaphore acquireSemaphore_ = VK_NULL_HANDLE;
		VkFence acquireFence_ = VK_NULL_HANDLE;
	};
	

} // namespace Vulkan

IFNITY_END_NAMESPACE