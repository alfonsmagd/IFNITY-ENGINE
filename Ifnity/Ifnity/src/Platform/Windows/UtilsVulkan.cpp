


#include "UtilsVulkan.h"



IFNITY_NAMESPACE

const char* getVulkanResultString(VkResult result)
{
	#define RESULT_CASE(res) \
  case res:              \
    return #res
	switch(result)
	{
		RESULT_CASE(VK_SUCCESS);
		RESULT_CASE(VK_NOT_READY);
		RESULT_CASE(VK_TIMEOUT);
		RESULT_CASE(VK_EVENT_SET);
		RESULT_CASE(VK_EVENT_RESET);
		RESULT_CASE(VK_INCOMPLETE);
		RESULT_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
		RESULT_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		RESULT_CASE(VK_ERROR_INITIALIZATION_FAILED);
		RESULT_CASE(VK_ERROR_DEVICE_LOST);
		RESULT_CASE(VK_ERROR_MEMORY_MAP_FAILED);
		RESULT_CASE(VK_ERROR_LAYER_NOT_PRESENT);
		RESULT_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
		RESULT_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
		RESULT_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
		RESULT_CASE(VK_ERROR_TOO_MANY_OBJECTS);
		RESULT_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
		RESULT_CASE(VK_ERROR_SURFACE_LOST_KHR);
		RESULT_CASE(VK_ERROR_OUT_OF_DATE_KHR);
		RESULT_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		RESULT_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		RESULT_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
		RESULT_CASE(VK_ERROR_FRAGMENTED_POOL);
		RESULT_CASE(VK_ERROR_UNKNOWN);
		// Provided by VK_VERSION_1_1
		RESULT_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
		// Provided by VK_VERSION_1_1
		RESULT_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
		// Provided by VK_VERSION_1_2
		RESULT_CASE(VK_ERROR_FRAGMENTATION);
		// Provided by VK_VERSION_1_2
		RESULT_CASE(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
		// Provided by VK_KHR_swapchain
		RESULT_CASE(VK_SUBOPTIMAL_KHR);
		// Provided by VK_NV_glsl_shader
		RESULT_CASE(VK_ERROR_INVALID_SHADER_NV);
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
		#endif
		// Provided by VK_EXT_image_drm_format_modifier
		RESULT_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
		// Provided by VK_KHR_global_priority
		RESULT_CASE(VK_ERROR_NOT_PERMITTED_KHR);
		// Provided by VK_EXT_full_screen_exclusive
		RESULT_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
		// Provided by VK_KHR_deferred_host_operations
		RESULT_CASE(VK_THREAD_IDLE_KHR);
		// Provided by VK_KHR_deferred_host_operations
		RESULT_CASE(VK_THREAD_DONE_KHR);
		// Provided by VK_KHR_deferred_host_operations
		RESULT_CASE(VK_OPERATION_DEFERRED_KHR);
		// Provided by VK_KHR_deferred_host_operations
		RESULT_CASE(VK_OPERATION_NOT_DEFERRED_KHR);
	default:
		return "Unknown VkResult Value";
	}
	#undef RESULT_CASE
}



PFN_vkSetDebugUtilsObjectNameEXT gvkSetDebugUtilsObjectNameEXT = nullptr;



IFNITY_INLINE VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
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

IFNITY_INLINE VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
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

IFNITY_INLINE void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}



//Set debug object name without instance and previously luaded vkSetDebugUtilsObjectNameEXT 
VkResult setDebugObjectName(VkDevice device, VkObjectType type, uint64_t handle, const char* name)
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

	if(gvkSetDebugUtilsObjectNameEXT != nullptr)
	{
		return gvkSetDebugUtilsObjectNameEXT(device, &ni);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


//
bool setupDebugCallbacksVK123(VkInstance instance, VkDebugUtilsMessengerEXT* debugMessenger)
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
		return func(device, &ni);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


VkSemaphore createSemaphore(VkDevice device, const char* debugName)
{
	const VkSemaphoreCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.flags = 0,
	};
	VkSemaphore semaphore = VK_NULL_HANDLE;
	VK_CHECK(vkCreateSemaphore(device, &ci, nullptr, &semaphore), "error create semaphore");
	VK_CHECK(setDebugObjectName(device, VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)semaphore, debugName), "error set debug Object Name type Semaphore");
	return semaphore;
}


VkFence createFence(VkDevice device, const char* debugName)
{
	const VkFenceCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = 0,
	};
	VkFence fence = VK_NULL_HANDLE;
	VK_CHECK(vkCreateFence(device, &ci, nullptr, &fence), "error create fence ");
	VK_CHECK(setDebugObjectName(device, VK_OBJECT_TYPE_FENCE, (uint64_t)fence, debugName), "error debug objet name type fence");
	return fence;
}

void imageMemoryBarrier(VkCommandBuffer buffer,
	VkImage image,
	VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask,
	VkImageSubresourceRange subresourceRange)
{
	const VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask = srcAccessMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = oldImageLayout,
		.newLayout = newImageLayout,
		.image = image,
		.subresourceRange = subresourceRange,
	};
	vkCmdPipelineBarrier(buffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
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