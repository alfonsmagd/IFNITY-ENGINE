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

#define VK_ASSERT(func)                                            \
  {                                                                \
    const VkResult vk_assert_result = func;                        \
    if (vk_assert_result != VK_SUCCESS) {                          \
      IFNITY_LOG(LogCore,ERROR,"Vulkan API call failed: %s:%i\n  %s\n  %s\n", \
                    getVulkanResultString(vk_assert_result)); \
      assert(false);                                               \
    }                                                              \
  }

extern PFN_vkSetDebugUtilsObjectNameEXT gvkSetDebugUtilsObjectNameEXT;


#define VK_CHECK(result, errorMessage) \
    if ((result) != VK_SUCCESS) { \
        IFNITY_LOG(LogCore, ERROR, errorMessage); \
        \
    }

IFNITY_INLINE VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageType,
	const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
	void* pUserData);

IFNITY_INLINE VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

 extern IFNITY_INLINE void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * pAllocator);


//Set debug object name without instance and previously luaded vkSetDebugUtilsObjectNameEXT 
VkResult setDebugObjectName(VkDevice device, VkObjectType type, uint64_t handle, const char* name);
bool setupDebugCallbacksVK123(VkInstance instance, VkDebugUtilsMessengerEXT* debugMessenger);
VkResult setDebugObjectName(VkInstance instance, VkDevice device, VkObjectType type, uint64_t handle, const char* name);
VkSemaphore createSemaphore(VkDevice device, const char* debugName);
VkFence createFence(VkDevice device, const char* debugName);
const char* getVulkanResultString(VkResult result);


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