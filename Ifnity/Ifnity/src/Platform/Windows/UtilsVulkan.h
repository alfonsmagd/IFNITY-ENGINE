#pragma once

#include  "pch.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <glslang\Include\glslang_c_interface.h>
#include "../Vulkan/vulkan_classes.hpp"



IFNITY_NAMESPACE


#pragma region DEFINES AND MACROS

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

#pragma endregion

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
void imageMemoryBarrier(VkCommandBuffer buffer,
    VkImage image,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
	VkImageSubresourceRange subresourceRange);

void saveSPIRVBinaryFile(const char* filename, const uint8_t * code, size_t size);
glslang_resource_t getGlslangResource(const VkPhysicalDeviceLimits & limits);


VkResult compileShaderVK(VkShaderStageFlagBits stage,
    const char* code,
    std::vector<uint8_t>*outSPIRV,
    const glslang_resource_t * glslLangResource);

VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(uint32_t binding,
    VkDescriptorType descriptorType,
    uint32_t descriptorCount,
    VkShaderStageFlags stageFlags,
    const VkSampler * immutableSamplers = nullptr);


VkSpecializationInfo getPipelineShaderStageSpecializationInfo( Vulkan::SpecializationConstantDesc desc,
    VkSpecializationMapEntry* outEntries);

VkSampleCountFlagBits getVulkanSampleCountFlags(uint32_t numSamples, VkSampleCountFlags maxSamplesMask);

VkPipelineShaderStageCreateInfo getPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage,
    VkShaderModule shaderModule,
    const char* entryPoint,
    const VkSpecializationInfo * specializationInfo);


VkSpecializationInfo getSpecializationInfo(uint32_t mapEntryCount,
                                           const VkSpecializationMapEntry * mapEntries,
                                           size_t dataSize,
                                           const void* data);

VkMemoryPropertyFlags storageTypeToVkMemoryPropertyFlags(StorageType storage);



IFNITY_API void testShaderCompilation(const char* sourceFilename, const char* destFilename);



IFNITY_END_NAMESPACE