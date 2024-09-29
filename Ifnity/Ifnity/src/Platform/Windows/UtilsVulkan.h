
#pragma once

#include  "pch.h"

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>


#define VK_CHECK(result, errorMessage) \
    if ((result) != VK_SUCCESS) { \
        IFNITY_LOG(LogCore, ERROR, errorMessage); \
        \
    }




//Forward declaration
struct GLFWwindow;

struct ImageBlock
{
	VkImage			image = VK_NULL_HANDLE;
	VkImageView     imageView = VK_NULL_HANDLE;
	VkFormat		format;
	VmaAllocation	imageAlloc = VK_NULL_HANDLE;
	VkDeviceMemory  imageMemory = VK_NULL_HANDLE;
};

struct VkRenderData
{
	GLFWwindow* rdWindow = nullptr;

	int rdWidth = 0;
	int rdHeight = 0;

	unsigned int rdTriangleCount = 0;

	VmaAllocator rdAllocator = nullptr;

	vkb::Instance rdVkbInstance{};
	vkb::PhysicalDevice rdVkbPhysicalDevice{};
	vkb::Device rdVkbDevice{};
	vkb::Swapchain rdVkbSwapchain{};

	std::vector<VkImage> rdSwapchainImages;
	std::vector<VkImageView> rdSwapchainImageViews;
	std::vector<VkFramebuffer> rdFramebuffers;

	VkQueue rdGraphicsQueue = VK_NULL_HANDLE;
	VkQueue rdPresentQueue = VK_NULL_HANDLE;

	VkImage rdDepthImage = VK_NULL_HANDLE;
	VkImageView rdDepthImageView = VK_NULL_HANDLE;
	VkFormat rdDepthFormat;
	VmaAllocation rdDepthImageAlloc = VK_NULL_HANDLE;

	VkRenderPass rdRenderpass;
	VkPipelineLayout rdPipelineLayout = VK_NULL_HANDLE;
	VkPipeline rdBasicPipeline = VK_NULL_HANDLE;
	VkPipeline rdChangedPipeline = VK_NULL_HANDLE;

	VkCommandPool rdCommandPool = VK_NULL_HANDLE;
	VkCommandBuffer rdCommandBuffer = VK_NULL_HANDLE;

	VkSemaphore rdPresentSemaphore = VK_NULL_HANDLE;
	VkSemaphore rdRenderSemaphore = VK_NULL_HANDLE;
	VkFence rdRenderFence = VK_NULL_HANDLE;

	VkImage rdTextureImage = VK_NULL_HANDLE;
	VkImageView rdTextureImageView = VK_NULL_HANDLE;
	VkSampler rdTextureSampler = VK_NULL_HANDLE;
	VmaAllocation rdTextureImageAlloc = nullptr;

	VkDescriptorPool rdTextureDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout rdTextureDescriptorLayout = VK_NULL_HANDLE;
	VkDescriptorSet rdTextureDescriptorSet = VK_NULL_HANDLE;

	VkBuffer rdUboBuffer = VK_NULL_HANDLE;
	VmaAllocation rdUboBufferAlloc = nullptr;

	VkDescriptorPool rdUBODescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout rdUBODescriptorLayout = VK_NULL_HANDLE;
	VkDescriptorSet rdUBODescriptorSet = VK_NULL_HANDLE;

	VkDescriptorPool rdImguiDescriptorPool = VK_NULL_HANDLE;
};