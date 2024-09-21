

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
		!CreateVmaAllocator())
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


IFNITY_END_NAMESPACE