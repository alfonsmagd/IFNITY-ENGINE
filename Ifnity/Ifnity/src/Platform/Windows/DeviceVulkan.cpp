

#include "DeviceVulkan.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"


IFNITY_NAMESPACE 




void DeviceVulkan::OnUpdate()
{
}

unsigned int DeviceVulkan::GetWidth() const
{
	return 0;
}

unsigned int DeviceVulkan::GetHeight() const
{
	return 0;
}

bool DeviceVulkan::InitInternalInstance()
{
	// Create Vulkan instance

	vkb::InstanceBuilder Builder;

	auto inst_ret =
		Builder.set_app_name("Example Vulkan Application")
		.request_validation_layers()
		.use_default_debug_messenger()
		.build();
	
	if (!inst_ret)
	{
		//Report Error Log
		IFNITY_LOG(LogCore, ERROR, "Failed to create Vulkan instance");
		return false;
	}
	IFNITY_LOG(LogCore, INFO, "Vulkan instance created");

	// Store the instance.
	m_Instance = inst_ret.value();
	return true;
}

bool DeviceVulkan::InitializeDeviceAndContext()
{
	if (!CreateSurface()        || 
		!CreatePhysicalDevice() ||
		!CreateDevice()         ||
		!CreateVmaAllocator()	||
		!GetQueue()				||
		!CreateSwapChain()		||
		!CreateCommandPool())
	{
		IFNITY_LOG(LogCore, ERROR, "Failed Initialization Process ");
		return false;
	}


	return true;
}

bool DeviceVulkan::ConfigureSpecificHintsGLFW() const
{
	//Verify that Vulkan is supported
	if (!glfwVulkanSupported())
	{
		IFNITY_LOG(LogCore, ERROR, "Vulkan is not supported");
		return false;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	return true; 
}

void DeviceVulkan::SetVSync(bool enabled)
{
}

bool DeviceVulkan::IsVSync() const
{
	return false;
}

void DeviceVulkan::ResizeSwapChain()
{
}

void DeviceVulkan::InitializeGui()
{
}

bool DeviceVulkan::CreateSurface()
{
	VkResult result = VK_ERROR_UNKNOWN;
	result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);

	if (result != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create window surface in Vulkan");
		return false;
	}

	return true;
}

bool DeviceVulkan::CreatePhysicalDevice()
{
	
	vkb::PhysicalDeviceSelector physicalDevSel{ m_Instance};
	auto physicalDevSelRet = physicalDevSel.set_surface(m_Surface).select();
	if (!physicalDevSelRet)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to select physical device in Vulkan Device");
		return false;
	}
	m_PhysicalDevice = physicalDevSelRet.value();

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice.physical_device, &supportedFeatures);

	//To enable the features that we need available in the physical device.
	m_PhysicalDevice.features = supportedFeatures;

	IFNITY_LOG(LogCore, INFO, "Physical device selected " + std::string(m_PhysicalDevice.name.c_str()));



	return true;
}

bool DeviceVulkan::CreateDevice()
{
	vkb::DeviceBuilder deviceBuilder{ m_PhysicalDevice };

	auto deviceRet = deviceBuilder.build();
	if (!deviceRet)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create device in Vulkan Device");
		return false;
	}
	m_Device = deviceRet.value();

	return true;
}

bool DeviceVulkan::CreateVmaAllocator()
{
	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.physicalDevice = m_PhysicalDevice.physical_device;
	allocatorInfo.device = m_Device.device;
	allocatorInfo.instance = m_Instance.instance;
	if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create VMA allocator in Vulkan Device");
		return false;
	}

	return true;
}

bool DeviceVulkan::GetQueue()
{
	auto graphQueueRet = m_Device.get_queue(vkb::QueueType::graphics);
	if (!graphQueueRet.has_value())
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to get graphics queue in Vulkan Device");
		return false;
	}
	m_GraphicsQueue = graphQueueRet.value();

	auto presentQueueRet = m_Device.get_queue(vkb::QueueType::present);
	if (!presentQueueRet.has_value())
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to get present  queue in Vulkan Device");
		return false;
	}
	m_PresentQueue = presentQueueRet.value();
	


	return true;
}

bool DeviceVulkan::CreateSwapChain()
{
	vkb::SwapchainBuilder swapChainBuild{ m_Device};

	/* VK_PRESENT_MODE_FIFO_KHR enables vsync */
	auto swapChainBuildRet = swapChainBuild.set_old_swapchain(m_Swapchain).set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR).build();
	if (!swapChainBuildRet)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create swapchain in Vulkan Device");
		return false;
	}

	vkb::destroy_swapchain(m_Swapchain);
	m_Swapchain= swapChainBuildRet.value();

	return true;
}

bool DeviceVulkan::CreateCommandPool()
{
	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = m_Device.get_queue_index(vkb::QueueType::graphics).value();
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_Device, &pool_info, nullptr, &m_CommandPool) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create command pool in Vulkan Device");
		return false;
	}

	return TRUE;
}

bool DeviceVulkan::DestroyCommandPool()
{
	vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	return false;
}

bool DeviceVulkan::CreateDepthBuffer()
{
	/*VkExtent3D depthImageExtent = {
	   m_Swapchain.extent.width,
	   m_Swapchain.extent.height,
	   1
	};

	mRenderData.rdDepthFormat = VK_FORMAT_D32_SFLOAT;

	VkImageCreateInfo depthImageInfo{};
	depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
	depthImageInfo.format = mRenderData.rdDepthFormat;
	depthImageInfo.extent = depthImageExtent;
	depthImageInfo.mipLevels = 1;
	depthImageInfo.arrayLayers = 1;
	depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VmaAllocationCreateInfo depthAllocInfo{};
	depthAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	depthAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vmaCreateImage(mRenderData.rdAllocator, &depthImageInfo, &depthAllocInfo, &mRenderData.rdDepthImage, &mRenderData.rdDepthImageAlloc, nullptr) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not allocate depth buffer memory\n", __FUNCTION__);
		return false;
	}

	VkImageViewCreateInfo depthImageViewinfo{};
	depthImageViewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthImageViewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthImageViewinfo.image = mRenderData.rdDepthImage;
	depthImageViewinfo.format = mRenderData.rdDepthFormat;
	depthImageViewinfo.subresourceRange.baseMipLevel = 0;
	depthImageViewinfo.subresourceRange.levelCount = 1;
	depthImageViewinfo.subresourceRange.baseArrayLayer = 0;
	depthImageViewinfo.subresourceRange.layerCount = 1;
	depthImageViewinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	if (vkCreateImageView(mRenderData.rdVkbDevice.device, &depthImageViewinfo, nullptr, &mRenderData.rdDepthImageView) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not create depth buffer image view\n", __FUNCTION__);
		return false;
	}*/
	return true;
}


IFNITY_END_NAMESPACE