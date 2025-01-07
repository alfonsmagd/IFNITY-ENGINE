
#pragma once

#include  "pch.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

IFNITY_NAMESPACE

#define VK_CHECK(result, errorMessage) \
    if ((result) != VK_SUCCESS) { \
        IFNITY_LOG(LogCore, ERROR, errorMessage); \
        \
    }


IFNITY_INLINE VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageType,
	const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
	void* pUserData)
{
	// Select prefix depending on flags passed to the callback
	std::string prefix("");

	if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		prefix = "VERBOSE: ";
	}
	else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		prefix = "INFO: ";
	}
	else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		prefix = "WARNING: ";
	}
	else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		prefix = "ERROR: ";
	}

	std::cerr << prefix << " validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

IFNITY_INLINE VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

IFNITY_INLINE void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}




//
bool setupDebugCallbacksVK123(VkInstance instance, VkDebugUtilsMessengerEXT * debugMessenger)
{
	#ifdef _DEBUG
	VkDebugUtilsMessengerCreateInfoEXT create_info;
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debugUtilsMessengerCallback;
	create_info.pNext = NULL;
	create_info.flags = 0;


	if(CreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, debugMessenger) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create debug utils messenger in Vulkan Device");
		return false;

	}


	#endif

	return true;
}

VkResult setDebugObjectName(VkInstance instance, VkDevice device, VkObjectType type, uint64_t handle, const char* name)
{
	if(!name || !*name)
	{
		return VK_SUCCESS;
	}
	const VkDebugUtilsObjectNameInfoEXT ni = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType = type,
		.objectHandle = handle,
		.pObjectName = name,
	};
	
	auto func = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
	if(func != nullptr)
	{
		return func(device,&ni);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


//Forward declaration

//struct VkRenderData
//{
//	GLFWwindow* rdWindow = nullptr;
//
//	int rdWidth = 0;
//	int rdHeight = 0;
//
//	unsigned int rdTriangleCount = 0;
//
//	VmaAllocator rdAllocator = nullptr;
//
//	vkb::Instance rdVkbInstance{};
//	vkb::PhysicalDevice rdVkbPhysicalDevice{};
//	vkb::Device rdVkbDevice{};
//	vkb::Swapchain rdVkbSwapchain{};
//
//	std::vector<VkImage> rdSwapchainImages;
//	std::vector<VkImageView> rdSwapchainImageViews;
//	std::vector<VkFramebuffer> rdFramebuffers;
//
//	VkQueue rdGraphicsQueue = VK_NULL_HANDLE;
//	VkQueue rdPresentQueue = VK_NULL_HANDLE;
//
//	VkImage rdDepthImage = VK_NULL_HANDLE;
//	VkImageView rdDepthImageView = VK_NULL_HANDLE;
//	VkFormat rdDepthFormat;
//	VmaAllocation rdDepthImageAlloc = VK_NULL_HANDLE;
//
//	VkRenderPass rdRenderpass;
//	VkPipelineLayout rdPipelineLayout = VK_NULL_HANDLE;
//	VkPipeline rdBasicPipeline = VK_NULL_HANDLE;
//	VkPipeline rdChangedPipeline = VK_NULL_HANDLE;
//
//	VkCommandPool rdCommandPool = VK_NULL_HANDLE;
//	VkCommandBuffer rdCommandBuffer = VK_NULL_HANDLE;
//
//	VkSemaphore rdPresentSemaphore = VK_NULL_HANDLE;
//	VkSemaphore rdRenderSemaphore = VK_NULL_HANDLE;
//	VkFence rdRenderFence = VK_NULL_HANDLE;
//
//	VkImage rdTextureImage = VK_NULL_HANDLE;
//	VkImageView rdTextureImageView = VK_NULL_HANDLE;
//	VkSampler rdTextureSampler = VK_NULL_HANDLE;
//	VmaAllocation rdTextureImageAlloc = nullptr;
//
//	VkDescriptorPool rdTextureDescriptorPool = VK_NULL_HANDLE;
//	VkDescriptorSetLayout rdTextureDescriptorLayout = VK_NULL_HANDLE;
//	VkDescriptorSet rdTextureDescriptorSet = VK_NULL_HANDLE;
//
//	VkBuffer rdUboBuffer = VK_NULL_HANDLE;
//	VmaAllocation rdUboBufferAlloc = nullptr;
//
//	VkDescriptorPool rdUBODescriptorPool = VK_NULL_HANDLE;
//	VkDescriptorSetLayout rdUBODescriptorLayout = VK_NULL_HANDLE;
//	VkDescriptorSet rdUBODescriptorSet = VK_NULL_HANDLE;
//
//	VkDescriptorPool rdImguiDescriptorPool = VK_NULL_HANDLE;
//};

IFNITY_END_NAMESPACE