

#include "DeviceVulkan.h"
#include "UtilsVulkan.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#include <spirv_cross\spirv_reflect.hpp>
#include  "../Vulkan/vulkan_classes.hpp"
//#define IMGUI_DEMO 

IFNITY_NAMESPACE

static void check_vk_result(VkResult err)
{
	if( err == 0 )
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if( err < 0 )
		abort();
}

static void PrintEnabledFeature(VkPhysicalDevice vkpd)
{

	VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	VkPhysicalDeviceVulkan11Features vulkan11Features = {};
	vulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	deviceFeatures2.pNext = &vulkan11Features;

	VkPhysicalDeviceVulkan12Features vulkan12Features = {};
	vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	vulkan11Features.pNext = &vulkan12Features;

	VkPhysicalDeviceVulkan13Features vulkan13Features = {};
	vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	vulkan12Features.pNext = &vulkan13Features;

	VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};
	indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	vulkan13Features.pNext = &indexingFeatures;

	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
	bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	indexingFeatures.pNext = &bufferDeviceAddressFeatures;

	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	bufferDeviceAddressFeatures.pNext = &dynamicRenderingFeatures;

	vkGetPhysicalDeviceFeatures2(vkpd, &deviceFeatures2);

	// Imprimir características de VkPhysicalDeviceFeatures
	IFNITY_LOG(LogCore, INFO, "RobustBufferAccess: {}", deviceFeatures2.features.robustBufferAccess ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "FullDrawIndexUint32: {}", deviceFeatures2.features.fullDrawIndexUint32 ? "Enabled" : "Disabled");
	// Agrega más características según sea necesario...

	// Imprimir características de VkPhysicalDeviceVulkan11Features
	IFNITY_LOG(LogCore, INFO, "StorageBuffer16BitAccess: {}", vulkan11Features.storageBuffer16BitAccess ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "UniformAndStorageBuffer16BitAccess: {}", vulkan11Features.uniformAndStorageBuffer16BitAccess ? "Enabled" : "Disabled");
	// Agrega más características según sea necesario...

	// Imprimir características de VkPhysicalDeviceVulkan12Features
	IFNITY_LOG(LogCore, INFO, "BufferDeviceAddress: {}", vulkan12Features.bufferDeviceAddress ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "BufferDeviceAddressCaptureReplay: {}", vulkan12Features.bufferDeviceAddressCaptureReplay ? "Enabled" : "Disabled");
	// Agrega más características según sea necesario...

	// Imprimir características de VkPhysicalDeviceVulkan13Features
	IFNITY_LOG(LogCore, INFO, "DynamicRendering: {}", vulkan13Features.dynamicRendering ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "Synchronization2: {}", vulkan13Features.synchronization2 ? "Enabled" : "Disabled");
	// Agrega más características según sea necesario...

	// Imprimir características de VkPhysicalDeviceDescriptorIndexingFeatures
	IFNITY_LOG(LogCore, INFO, "DescriptorBindingSampledImageUpdateAfterBind: {}", indexingFeatures.descriptorBindingSampledImageUpdateAfterBind ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "DescriptorBindingStorageImageUpdateAfterBind: {}", indexingFeatures.descriptorBindingStorageImageUpdateAfterBind ? "Enabled" : "Disabled");
	// Agrega más características según sea necesario...

	// Imprimir características de VkPhysicalDeviceBufferDeviceAddressFeatures
	IFNITY_LOG(LogCore, INFO, "BufferDeviceAddress: {}", bufferDeviceAddressFeatures.bufferDeviceAddress ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "BufferDeviceAddressCaptureReplay: {}", bufferDeviceAddressFeatures.bufferDeviceAddressCaptureReplay ? "Enabled" : "Disabled");

	IFNITY_LOG(LogCore, INFO, "DynamicRendering: {}", vulkan13Features.dynamicRendering ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "Synchronization2: {}", vulkan13Features.synchronization2 ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "Maintenance4: {}", vulkan13Features.maintenance4 ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ShaderIntegerDotProduct: {}", vulkan13Features.shaderIntegerDotProduct ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ShaderDemoteToHelperInvocation: {}", vulkan13Features.shaderDemoteToHelperInvocation ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ShaderTerminateInvocation: {}", vulkan13Features.shaderTerminateInvocation ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "SubgroupSizeControl: {}", vulkan13Features.subgroupSizeControl ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ComputeFullSubgroups: {}", vulkan13Features.computeFullSubgroups ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "InlineUniformBlock: {}", vulkan13Features.inlineUniformBlock ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "DescriptorBindingInlineUniformBlockUpdateAfterBind: {}", vulkan13Features.descriptorBindingInlineUniformBlockUpdateAfterBind ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "PipelineCreationCacheControl: {}", vulkan13Features.pipelineCreationCacheControl ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "PrivateData: {}", vulkan13Features.privateData ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ShaderDemoteToHelperInvocation: {}", vulkan13Features.shaderDemoteToHelperInvocation ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ShaderTerminateInvocation: {}", vulkan13Features.shaderTerminateInvocation ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "SubgroupSizeControl: {}", vulkan13Features.subgroupSizeControl ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ComputeFullSubgroups: {}", vulkan13Features.computeFullSubgroups ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "InlineUniformBlock: {}", vulkan13Features.inlineUniformBlock ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "DescriptorBindingInlineUniformBlockUpdateAfterBind: {}", vulkan13Features.descriptorBindingInlineUniformBlockUpdateAfterBind ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "PipelineCreationCacheControl: {}", vulkan13Features.pipelineCreationCacheControl ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "PrivateData: {}", vulkan13Features.privateData ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ShaderDemoteToHelperInvocation: {}", vulkan13Features.shaderDemoteToHelperInvocation ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ShaderTerminateInvocation: {}", vulkan13Features.shaderTerminateInvocation ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "SubgroupSizeControl: {}", vulkan13Features.subgroupSizeControl ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "ComputeFullSubgroups: {}", vulkan13Features.computeFullSubgroups ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "InlineUniformBlock: {}", vulkan13Features.inlineUniformBlock ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "DescriptorBindingInlineUniformBlockUpdateAfterBind: {}", vulkan13Features.descriptorBindingInlineUniformBlockUpdateAfterBind ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "PipelineCreationCacheControl: {}", vulkan13Features.pipelineCreationCacheControl ? "Enabled" : "Disabled");
	IFNITY_LOG(LogCore, INFO, "PrivateData: {}", vulkan13Features.privateData ? "Enabled" : "Disabled");
	// Imprimir características de VkPhysicalDeviceDynamicRenderingFeaturesKHR
	IFNITY_LOG(LogCore, INFO, "DynamicRendering: {}", dynamicRenderingFeatures.dynamicRendering ? "Enabled" : "Disabled");



}



VkFormat DeviceVulkan::GetSwapChainFormat() const
{
	if( !hasSwapchain() )
	{
		return VK_FORMAT_UNDEFINED;
	}

	return swapchain_->getSurfaceFormat().format;
}



void DeviceVulkan::OnUpdate()
{



	#ifdef SANDBOX_TOOL

	//using vec3 = glm::vec3;
	//static bool hasexecuted = false;
	////First get acquire the command buffer 
	Vulkan::CommandBuffer& cmdBuffer = acquireCommandBuffer();
	float color[ 4 ] = { 1.0f, 0.0f, 0.0f, 1.0f }; // Rojo

	Vulkan::TextureHandleSM currentTexture = getCurrentSwapChainTexture();
	Vulkan::RenderPass renderPass = {
	.color = { {.loadOp = Vulkan::LoadOp_Clear, .clearColor = { 1.0f, 1.0f, 1.0f, 1.0f } } } };

	Vulkan::Framebuffer framebuffer = { .color = { {.texture = currentTexture } } };

	cmdBuffer.cmdBeginRendering(renderPass, framebuffer);
	BeginRenderDocTrace(cmdBuffer.wrapper_->cmdBuf_, "Render Pass Begin 11111", color);

	//cmdBuffer.cmdDraw(3);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer.wrapper_->cmdBuf_);
	EndRenderDocTrace(cmdBuffer.wrapper_->cmdBuf_);
	cmdBuffer.cmdEndRendering();
	submit(cmdBuffer, currentTexture);

	#endif

}









unsigned int DeviceVulkan::GetWidth() const
{
	return m_Props.Width;
}

unsigned int DeviceVulkan::GetHeight() const
{
	return m_Props.Height;
}

bool DeviceVulkan::InitInternalInstance()
{
	// Create Vulkan instance

	vkb::InstanceBuilder Builder;

	auto inst_ret =
		Builder.set_app_name("Example Vulkan Application")
		.request_validation_layers()
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.build();

	if( !inst_ret )
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



	if( !CreateSurface() ||
	   !CreatePhysicalDevice() ||
	   !CreateDevice() ||
	   !CreateVmaAllocator() ||
	   !GetQueue() ||
	   !createVulkanImmediateCommands() ||
	   !CreateSwapChain() ||
	   !CreateCommandPool() ||
	   !CreateRenderPass() ||
	   !CreateFrameBuffer() ||
	   !CreateCommandBuffers() ||
	   !CreatePipelineCache() ||
	   !CreateSyncObjects()

	   )//endif
	{
		IFNITY_LOG(LogCore, ERROR, "Failed Initialization Process ");
		return false;
	}

	if( !GetDepthAvailableFormat() )
	{
		IFNITY_LOG(LogCore, WARNING, "Failed to get depth available format");

	}

	IFNITY_LOG(LogCore, INFO, "Vulkan Device and Context Initialized");
	//Get properties2 
	IFNITY_ASSERT_MSG(m_PhysicalDevice.physical_device != VK_NULL_HANDLE, "No physical device create");
	getPhysicalDeviceProperties2(m_PhysicalDevice.physical_device);


	//Force create default sampler and dummyTexture
	createSampler(VkSamplerCreateInfo{}, "Default Sampler");

	growDescriptorPool(16, 16);

	m_RenderDevice = Vulkan::CreateDevice(device_, this);
	

	PrintEnabledFeature(m_PhysicalDevice.physical_device);


	return true;
}

bool DeviceVulkan::ConfigureSpecificHintsGLFW() const
{
	//Verify that Vulkan is supported
	if( !glfwVulkanSupported() )
	{
		IFNITY_LOG(LogCore, ERROR, "Vulkan is not supported");
		return false;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	return true;
}

void DeviceVulkan::SetVSync(bool enabled)
{}

bool DeviceVulkan::IsVSync() const
{
	return true;
}

void DeviceVulkan::ResizeSwapChain()
{}

void DeviceVulkan::InitializeGui()
{
	InitGui();
}

void DeviceVulkan::InternalPreDestroy()
{
	//Wait devide to finish and get idle state.
	vkDeviceWaitIdle(device_.device);
}

void DeviceVulkan::ClearBackBuffer(float* color)
{
	std::copy(color, color + 4, m_Color);
}


DeviceVulkan::~DeviceVulkan()
{


	//Destroy Descriptor Pool 
	vkDestroyDescriptorPool(device_.device, m_ImGuiDescriptorPool, nullptr);

	//Destroy Sync Objects
	DestroySyncObjects();
	DestroyCommandBuffers();
	DestroyCommandPool();
	DestroyImmediateCommands();
	CleanFrameBuffers();
	DestroyRenderPass();

	//Destroy pipelines 
	DestroyPipelines();
	DestroyPipelineCache();
	DestroyShaderStages();

	//Destroy Vma Allocator
	vmaDestroyAllocator(m_Allocator);

	// Destroy Debug Utils Messenger
	if( debugUtilsMessenger != VK_NULL_HANDLE )
	{
		DestroyDebugUtilsMessengerEXT(m_Instance, debugUtilsMessenger, nullptr);
	}

	//Destroy 
	vkDestroyDescriptorSetLayout(device_, vkDSL_, nullptr);
	vkDestroyDescriptorPool(device_, vkDPool_, nullptr);
	// Destroy VkBootStrap
	swapchainBootStraap_.destroy_image_views(m_SwapchainImageViews);

	vkb::destroy_swapchain(swapchainBootStraap_);
	swapchain_.reset();
	vkb::destroy_device(device_);
	vkb::destroy_surface(m_Instance, m_Surface);
	vkb::destroy_instance(m_Instance);



	IFNITY_LOG(LogCore, INFO, "Delete Device Vulkan Render.");

}

uint32_t DeviceVulkan::getFramebufferMSAABitMask() const
{
	//Get properties2 limits 
	//framebufferColorSampleCounts: Indicates the multisample anti-aliasing (MSAA) capabilities that the physical device supports for color attachments in a framebuffer.
	//framebufferDepthSampleCounts : Indicates the multisample anti - aliasing(MSAA) capabilities that the physical device supports for depth attachments in a framebuffer.

	const VkPhysicalDeviceLimits& limits = properties2.properties.limits;
	return limits.framebufferColorSampleCounts & limits.framebufferDepthSampleCounts;
}

bool DeviceVulkan::CreateSurface()
{
	VkResult result = VK_ERROR_UNKNOWN;
	result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);

	if( result != VK_SUCCESS )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create window surface in Vulkan");
		return false;
	}

	return true;
}

bool DeviceVulkan::CreatePhysicalDevice()
{

	vkb::PhysicalDeviceSelector physicalDevSel{ m_Instance };



	//Try to enable dynamic rendering 
	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	//Try to enable INDEXING FEATURES   
	VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
	indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
	indexingFeatures.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
	indexingFeatures.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
	indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
	indexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
	indexingFeatures.runtimeDescriptorArray = VK_TRUE;

	// Declarar y configurar VkPhysicalDeviceBufferDeviceAddressFeatures
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
	bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;
	bufferDeviceAddressFeatures.bufferDeviceAddressCaptureReplay = VK_TRUE;

	// Estructura de características
	VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT dynamicRenderingUnusedAttachmentsFeatures = {};
	dynamicRenderingUnusedAttachmentsFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT;
	dynamicRenderingUnusedAttachmentsFeatures.dynamicRenderingUnusedAttachments = VK_TRUE;

	//Features  Vulkan 1.3
	VkPhysicalDeviceVulkan13Features vulkan13Features = {};
	vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	vulkan13Features.synchronization2 = VK_TRUE;
	vulkan13Features.maintenance4 = VK_TRUE;
	vulkan13Features.dynamicRendering = VK_TRUE;


	auto physicalDevSelRet = physicalDevSel.set_surface(m_Surface)
		.add_required_extension_features(indexingFeatures)
		.add_required_extension_features(bufferDeviceAddressFeatures)
		//.add_required_extension_features(dynamicRenderingFeatures)
		.add_required_extension_features(dynamicRenderingUnusedAttachmentsFeatures)
		.add_required_extension_features(vulkan13Features)
		.add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
		.add_required_extension(VK_KHR_MAINTENANCE2_EXTENSION_NAME)
		//.add_required_extension(VK_KHR_MULTIVIEW_EXTENSION_NAME)
		//.add_required_extension(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME)
		.add_required_extension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)
		.add_required_extension(VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME)
		.select();

	if( !physicalDevSelRet )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to select physical device in Vulkan Device");
		return false;
	}

	m_PhysicalDevice = physicalDevSelRet.value();

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice.physical_device, &supportedFeatures);

	// Enumerar extensiones soportadas por el dispositivo físico
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, extensions.data());

	for( const auto& ext : extensions )
	{
		IFNITY_LOG(LogCore, INFO, "Supported extension: {}", ext.extensionName);
	}

	// Verificar si VK_EXT_debug_marker está en la lista de extensiones soportadas
	bool debugMarkerSupported = false;
	for( const auto& ext : extensions )
	{
		if( strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0 )
		{
			debugMarkerSupported = true;
			break;
		}
	}

	if( debugMarkerSupported )
	{
		IFNITY_LOG(LogCore, INFO, "VK_EXT_debug_marker is supported");
	}
	else
	{
		IFNITY_LOG(LogCore, WARNING, "VK_EXT_debug_marker is NOT supported");
	}

	//To enable the features that we need available in the physical device.
	m_PhysicalDevice.features = supportedFeatures;



	return true;
}

bool DeviceVulkan::CreateDevice()
{
	vkb::DeviceBuilder deviceBuilder{ m_PhysicalDevice };




	auto deviceRet = deviceBuilder.build();
	if( !deviceRet )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create device in Vulkan Device");
		return false;
	}
	device_ = deviceRet.value();

	#ifdef _DEBUG
	// Create debug utils messenger
	//setupDebugCallbacksVK(m_Instance.instance, &debugUtilsMessenger);
	setupCallbacks(device_.device);
	setDebugObjectName(m_Instance, device_.device, VK_OBJECT_TYPE_DEVICE, (uint64_t)device_.device, "Device Context:");

	#endif

	PrintEnabledFeature(m_PhysicalDevice.physical_device);

	return true;
}

bool DeviceVulkan::CreateVmaAllocator()
{


	m_Allocator = createVmaAllocator(m_PhysicalDevice, device_, m_Instance, getApiVersion());


	if( m_Allocator == VK_NULL_HANDLE )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create Vma Allocator in Vulkan Device");
		return false;
	}
	return true;
}

bool DeviceVulkan::GetQueue()
{
	auto graphQueueRet = device_.get_queue(vkb::QueueType::graphics);
	if( !graphQueueRet.has_value() )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to get graphics queue in Vulkan Device");
		return false;
	}
	deviceQueues_.graphicsQueue = graphQueueRet.value();
	deviceQueues_.graphicsQueueFamilyIndex = device_.get_queue_index(vkb::QueueType::graphics).value();

	auto presentQueueRet = device_.get_queue(vkb::QueueType::present);
	if( !presentQueueRet.has_value() )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to get present  queue in Vulkan Device");
		return false;
	}
	deviceQueues_.presentQueue = presentQueueRet.value();
	deviceQueues_.presentQueueFamilyIndex = device_.get_queue_index(vkb::QueueType::present).value();



	return true;
}

bool DeviceVulkan::CreateSwapChain()
{
	vkb::SwapchainBuilder swapChainBuild{ device_ };

	/* VK_PRESENT_MODE_FIFO_KHR enables vsync */
	auto swapChainBuildRet = swapChainBuild.set_old_swapchain(swapchainBootStraap_).
		set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR).
		set_desired_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }).
		add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT).
		build();
	if( !swapChainBuildRet )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create swapchain in Vulkan Device");
		return false;
	}



	vkb::destroy_swapchain(swapchainBootStraap_);
	swapchainBootStraap_ = swapChainBuildRet.value();

	//Get image_count 
	m_commandBufferCount = swapchainBootStraap_.image_count;

	swapchain_ = std::make_unique<Vulkan::VulkanSwapchain>(*this,
														   swapchainBootStraap_.extent.width,
														   swapchainBootStraap_.extent.height);

	return true;
}

bool DeviceVulkan::CreateCommandPool()
{
	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = deviceQueues_.graphicsQueueFamilyIndex;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if( vkCreateCommandPool(device_, &pool_info, nullptr, &m_CommandPool) != VK_SUCCESS )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create command pool in Vulkan Device");
		return false;
	}

	return TRUE;
}

bool DeviceVulkan::DestroyCommandPool()
{
	vkDestroyCommandPool(device_, m_CommandPool, nullptr);
	return false;
}

bool DeviceVulkan::DestroyShaderStages()
{
	Vulkan::Device* vkDevice = dynamic_cast<Vulkan::Device*>(GetRenderDevice());
	if( !vkDevice )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to get Vulkan Device");
		return false;
	}
	vkDevice->destroyShaderModule();
}

void DeviceVulkan::DestroySyncObjects()
{
	for( size_t i = 0; i < 3; i++ )
	{
		vkDestroySemaphore(device_.device, m_ImageAvailableSemaphores[ i ], nullptr);
		vkDestroySemaphore(device_.device, m_RenderFinishedSemaphores[ i ], nullptr);
		vkDestroyFence(device_.device, m_InFlightFences[ i ], nullptr);
	}

}

void DeviceVulkan::DestroyCommandBuffers()
{
	vkFreeCommandBuffers(device_.device,
						 m_CommandPool,
						 static_cast<uint32_t>(m_CommandBuffers.size()),
						 m_CommandBuffers.data());

}

void DeviceVulkan::CleanFrameBuffers()
{
	for( auto& framebuffer : m_Framebuffers )
	{
		vkDestroyFramebuffer(device_.device, framebuffer, nullptr);
	}
}

void DeviceVulkan::DestroyRenderPass()
{
	vkDestroyRenderPass(device_.device, m_RenderPass, nullptr);
}

void DeviceVulkan::DestroyImmediateCommands()
{
	immediate_.reset();
}

void DeviceVulkan::DestroyPipelines()
{
	//Iterate about map destroy all pipelines

	for( auto& [key, pipeline] : map_renderPipelines )
	{
		if( pipeline )
		{
			pipeline->DestroyPipeline(device_);

		}
		continue;
	}
	map_renderPipelines.clear();
}

void DeviceVulkan::DestroyPipelineCache()
{
	if( pipelineCache_ != VK_NULL_HANDLE )
	{
		vkDestroyPipelineCache(device_.device, pipelineCache_, nullptr);
	}

}

bool DeviceVulkan::CreateDepthBuffer()
{
	/*VkExtent3D depthImageExtent = {
	   m_Swapchain.extent.width,
	   swapchainBootStraap_.extent.height,
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

	if (vkCreateImageView(m_Device.device, &depthImageViewinfo, nullptr, &mRenderData.rdDepthImageView) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not create depth buffer image view\n", __FUNCTION__);
		return false;
	}*/
	return true;
}

bool DeviceVulkan::CreateRenderPass()
{
	//Single RenderPass with one subpass 

	std::array<VkAttachmentDescription, 1> attachments = {};

	// Color attachment
	attachments[ 0 ].format = swapchainBootStraap_.image_format;								// Use the color format selected by the swapchain
	attachments[ 0 ].samples = VK_SAMPLE_COUNT_1_BIT;                                 // We don't use multi sampling in this example
	attachments[ 0 ].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear this attachment at the start of the render pass
	attachments[ 0 ].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                          // Keep its contents after the render pass is finished (for displaying it)
	attachments[ 0 ].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // Similar to loadOp, but for stenciling (we don't use stencil here)
	attachments[ 0 ].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // Similar to storeOp, but for stenciling (we don't use stencil here)
	attachments[ 0 ].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
	attachments[ 0 ].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;                   // Layout to which the attachment is transitioned when the render pass is finished
	// As we want to present the color attachment, we transition to PRESENT_KHR
// Setup attachment references
	VkAttachmentReference colorReference = {};
	colorReference.attachment = 0;                                                  // Attachment 0 is color
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;               // Attachment layout used as color during the subpass

	std::array<VkAttachmentReference, 1> attachmentref = { colorReference };

	// Setup a single subpass reference
	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;         // Pipeline type subpass is to be bound to
	subpassDescription.colorAttachmentCount = attachmentref.size();                 // Reference to the color attachment in this subpass
	subpassDescription.pColorAttachments = attachmentref.data();                    // Reference to the color attachment in this subpass
	subpassDescription.pDepthStencilAttachment = nullptr;							// (Depth attachments not used by this sample)
	subpassDescription.inputAttachmentCount = 0;									// Input attachments can be used to sample from contents of a previous subpass
	subpassDescription.pInputAttachments = nullptr;									// (Input attachments not used by this example)
	subpassDescription.preserveAttachmentCount = 0;									// Preserved attachments can be used to loop (and preserve) attachments through subpasses
	subpassDescription.pPreserveAttachments = nullptr;								// (Preserve attachments not used by this example)
	subpassDescription.pResolveAttachments = nullptr;								// Resolve attachments are resolved at the end of a sub pass and can be used for e.g. multi sampling

	// Setup subpass dependencies
	std::array<VkSubpassDependency, 1> dependencies = {};

	// Setup dependency and add implicit layout transition from final to initial layout for the color attachment.
	// (The actual usage layout is preserved through the layout specified in the attachment reference).
	dependencies[ 0 ].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[ 0 ].dstSubpass = 0;
	dependencies[ 0 ].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[ 0 ].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[ 0 ].srcAccessMask = VK_ACCESS_NONE;
	dependencies[ 0 ].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;


	//Create the render pass

	// Create the render pass object
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());  // Number of attachments used by this render pass
	renderPassInfo.pAttachments = attachments.data();                            // Descriptions of the attachments used by the render pass
	renderPassInfo.subpassCount = 1;                                             // We only use one subpass in this example
	renderPassInfo.pSubpasses = &subpassDescription;                             // Description of that subpass
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size()); // Number of subpass dependencies
	renderPassInfo.pDependencies = dependencies.data();                          // Subpass dependencies used by the render pass

	VK_CHECK(vkCreateRenderPass(device_.device, &renderPassInfo, nullptr, &m_RenderPass), "Failed to create render pass");


	return true;
}

bool DeviceVulkan::CreateFrameBuffer()
{
	m_SwapchainImages = swapchainBootStraap_.get_images().value();
	m_SwapchainImageViews = swapchainBootStraap_.get_image_views().value();


	VkImageView attachments[ 1 ] = {};


	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = NULL;
	frameBufferCreateInfo.renderPass = m_RenderPass;
	frameBufferCreateInfo.attachmentCount = 1;
	frameBufferCreateInfo.pAttachments = attachments;
	frameBufferCreateInfo.width = swapchainBootStraap_.extent.width;
	frameBufferCreateInfo.height = swapchainBootStraap_.extent.height;
	frameBufferCreateInfo.layers = 1;

	//Create Framebuffers for each swapchain image view
	m_Framebuffers.resize(m_SwapchainImageViews.size());

	for( size_t i = 0; i < m_Framebuffers.size(); i++ )
	{
		attachments[ 0 ] = m_SwapchainImageViews.at(i);
		VK_CHECK(vkCreateFramebuffer(device_.device, &frameBufferCreateInfo, nullptr, &m_Framebuffers[ i ]), "Failed to create framebuffer");

	}

	return true;
}

bool DeviceVulkan::CreateCommandBuffers()
{
	m_CommandBuffers.resize(m_Framebuffers.size());
	//vkGetDeviceQueue(m_Device.device, (int)m_GraphicsQueue, 0, &m_GraphicsQueue);
	VkCommandBufferAllocateInfo bufferAllocInfo{};
	bufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferAllocInfo.commandPool = m_CommandPool;
	bufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	bufferAllocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if( vkAllocateCommandBuffers(device_.device, &bufferAllocInfo, m_CommandBuffers.data()) != VK_SUCCESS )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to allocate command buffers");
		return false;
	}

	return true;
}

bool DeviceVulkan::CreateSyncObjects()
{
	m_ImageAvailableSemaphores.resize(3);
	m_RenderFinishedSemaphores.resize(3);
	m_InFlightFences.resize(3);

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for( size_t i = 0; i < 3; i++ )
	{
		if( vkCreateSemaphore(device_.device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[ i ]) != VK_SUCCESS ||
		   vkCreateSemaphore(device_.device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[ i ]) != VK_SUCCESS ||
		   vkCreateFence(device_.device, &fenceInfo, nullptr, &m_InFlightFences[ i ]) != VK_SUCCESS )
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to create synchronization objects for a frame");
			return false;
		}
	}


	return true;
}

bool DeviceVulkan::CreatePipelineCache()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	if( vkCreatePipelineCache(device_.device, &pipelineCacheCreateInfo, nullptr, &pipelineCache_) != VK_SUCCESS )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create pipeline cache");
		return false;
	}

	return true;
}

bool DeviceVulkan::GetDepthAvailableFormat()
{
	const VkFormat depthFormats[] = {
		VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM
	};

	for( const auto& depthFormat : depthFormats )
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice.physical_device, depthFormat, &formatProps);

		if( formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
		{
			depthFormats_.push_back(depthFormat);
		}
	}

	return !depthFormats_.empty();
}




VkResult DeviceVulkan::growDescriptorPool(uint32_t maxTextures, uint32_t maxSamplers)
{

	currentMaxTextures_ = maxTextures;
	currentMaxSamplers_ = maxSamplers;



	if( !(maxTextures <= vkPhysicalDeviceVulkan12Properties_.maxDescriptorSetUpdateAfterBindSampledImages) )
	{
		IFNITY_LOG(LogCore, ERROR, "Max Textures exceeded %u (max %u)", maxTextures, vkPhysicalDeviceVulkan12Properties_.maxDescriptorSetUpdateAfterBindSampledImages);
	}

	if( !(maxSamplers <= vkPhysicalDeviceVulkan12Properties_.maxDescriptorSetUpdateAfterBindSamplers) )
	{
		IFNITY_LOG(LogCore, ERROR, "Max Sampler exceeded %u (max %u)", maxSamplers, vkPhysicalDeviceVulkan12Properties_.maxDescriptorSetUpdateAfterBindSampledImages);
	}

	// Deallocate Descriptor Pool , Descriptor set layout



	/////////////////////////////////////////////////////////
	const uint32_t kBinding_NumBindings = 3;

	// create default descriptor set layout which is going to be shared by graphics pipelines
	VkShaderStageFlags stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
		VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |
		VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT |
		VK_SHADER_STAGE_FRAGMENT_BIT |
		VK_SHADER_STAGE_COMPUTE_BIT;
	//Create Descriptor set layout binding. 
	const VkDescriptorSetLayoutBinding bindings[ kBinding_NumBindings ] =
	{
		 descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxTextures, stageFlags),
		 descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER, maxSamplers, stageFlags),
		 descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxTextures, stageFlags)
	};

	const uint32_t flags = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
		VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT |
		VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
	//Fill bindingflags for each binding
	VkDescriptorBindingFlags bindingFlags[ kBinding_NumBindings ];
	for( int i = 0; i < kBinding_NumBindings; ++i )
	{
		bindingFlags[ i ] = flags;
	}
	const VkDescriptorSetLayoutBindingFlagsCreateInfo setLayoutBindingFlagsCI =
	{
		 .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,
		 .bindingCount = uint32_t(true ? kBinding_NumBindings : kBinding_NumBindings - 1), //exclude the last if we use acceleration structure
		 .pBindingFlags = bindingFlags,
	};
	const VkDescriptorSetLayoutCreateInfo dslci =
	{
		 .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		 .pNext = &setLayoutBindingFlagsCI,
		 .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT,
		 .bindingCount = uint32_t(true ? kBinding_NumBindings : kBinding_NumBindings - 1),//exclude the last if we use acceleration structure
		 .pBindings = bindings,
	};

	VK_ASSERT(vkCreateDescriptorSetLayout(device_, &dslci, nullptr, &vkDSL_));
	VK_ASSERT(setDebugObjectName(
		device_, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t)vkDSL_, "Descriptor Set Layout: DeviceVulkan::vkDSL_"));


	{
		// create default descriptor pool and allocate 1 descriptor set
		const VkDescriptorPoolSize poolSizes[ kBinding_NumBindings ]
		{
			 VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxTextures},
			 VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, maxSamplers},
			 VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxTextures}
		};
		const VkDescriptorPoolCreateInfo ci =
		{
			 .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			 .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
			 .maxSets = 1,
			 .poolSizeCount = uint32_t(TRUE ? kBinding_NumBindings : kBinding_NumBindings - 1),
			 .pPoolSizes = poolSizes,
		};
		VK_ASSERT(vkCreateDescriptorPool(device_, &ci, nullptr, &vkDPool_));
		const VkDescriptorSetAllocateInfo ai = {
			 .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			 .descriptorPool = vkDPool_,
			 .descriptorSetCount = 1,
			 .pSetLayouts = &vkDSL_,
		};
		VK_ASSERT(vkAllocateDescriptorSets(device_, &ai, &vkDSet_));
	}

	return VkResult();
}

void DeviceVulkan::getPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice)
{

	vkGetPhysicalDeviceProperties2(physicalDevice, &vkPhysicalDeviceProperties2_);
	properties2 = vkPhysicalDeviceProperties2_;

}

void DeviceVulkan::createWeaknessDeviceReference()
{


}

bool DeviceVulkan::AcquireNextImage()
{
	//Get the index of the next available swapchain image
	VkResult result = vkAcquireNextImageKHR(device_.device, swapchainBootStraap_.swapchain, UINT64_MAX, m_PresentSemaphore, (VkFence)nullptr, &m_imageIndex);

	if( result == VK_ERROR_OUT_OF_DATE_KHR )
	{
		//Swapchain is out of date (e.g. the window was resized) and
		//must be recreated:
		ResizeSwapChain();
		return false;
	}
	else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to acquire next swapchain image");
		return false;
	}

	return true;
}

bool DeviceVulkan::PopulateCommandBuffer()
{
	return false;
}

bool DeviceVulkan::SubmitCommandBuffer()
{
	return false;
}

bool DeviceVulkan::PresentImage()
{
	return false;
}

bool DeviceVulkan::InitGui()
{
	// Create descriptor pool
	if( !CreateImGuiDescriptorPool() )
	{
		return false;
	}


	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_Instance;
	init_info.PhysicalDevice = m_PhysicalDevice;
	init_info.Device = device_;
	init_info.Queue = deviceQueues_.graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = m_ImGuiDescriptorPool;
	init_info.RenderPass = VK_NULL_HANDLE;
	init_info.Subpass = 0;
	init_info.MinImageCount = 3;
	init_info.ImageCount = Vulkan::VulkanImmediateCommands::kMaxCommandBuffers;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.CheckVkResultFn = check_vk_result;
	init_info.UseDynamicRendering = true;

	// Asegúrate de que PipelineRenderingCreateInfo esté configurado correctamente
	VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {};
	pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	pipelineRenderingCreateInfo.colorAttachmentCount = 1;
	pipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchainBootStraap_.image_format;

	init_info.PipelineRenderingCreateInfo = pipelineRenderingCreateInfo;
	ImGui_ImplVulkan_Init(&init_info);

	//Upload Fonts if this was needed.


	return true;
}

void DeviceVulkan::CheckSpirvVersion(VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	uint32_t apiVersion = deviceProperties.apiVersion;
	uint32_t major = VK_VERSION_MAJOR(apiVersion);
	uint32_t minor = VK_VERSION_MINOR(apiVersion);
	uint32_t patch = VK_VERSION_PATCH(apiVersion);

	std::cout << "Vulkan API Version: " << major << "." << minor << "." << patch << std::endl;

	if( major == 1 && minor == 0 )
	{
		std::cout << "Supported SPIR-V Version: 1.0" << std::endl;
	}
	else if( major == 1 && minor == 1 )
	{
		std::cout << "Supported SPIR-V Version: 1.3" << std::endl;
	}
	else if( major == 1 && minor == 2 )
	{
		std::cout << "Supported SPIR-V Version: 1.5" << std::endl;
	}
	else if( major == 1 && minor == 3 )
	{
		std::cout << "Supported SPIR-V Version: 1.6" << std::endl;
	}
	else
	{
		std::cout << "Unknown Vulkan API Version" << std::endl;
	}
}

bool DeviceVulkan::CreateImGuiDescriptorPool()
{
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1;
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	if( vkCreateDescriptorPool(device_, &pool_info, nullptr, &m_ImGuiDescriptorPool) != VK_SUCCESS )
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create descriptor pool for ImGui");
		return false;
	}

	return true;
}



void DeviceVulkan::setupCallbacks(VkDevice& i_device)
{

	// Check if the debug utils extension is present (which is the case if run from a graphics debugger)
	bool extensionPresent = false;
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	for( auto& extension : extensions )
	{
		if( strcmp(extension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0 )
		{
			extensionPresent = true;
			break;
		}
	}

	if( extensionPresent )
	{
		// As with an other extension, function pointers need to be manually loaded
		vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
		vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
		vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(m_Instance, "vkCmdBeginDebugUtilsLabelEXT"));
		vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(m_Instance, "vkCmdInsertDebugUtilsLabelEXT"));
		vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(m_Instance, "vkCmdEndDebugUtilsLabelEXT"));
		vkQueueBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(m_Instance, "vkQueueBeginDebugUtilsLabelEXT"));
		vkQueueInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(m_Instance, "vkQueueInsertDebugUtilsLabelEXT"));
		vkQueueEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(m_Instance, "vkQueueEndDebugUtilsLabelEXT"));
		vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(m_Instance, "vkSetDebugUtilsObjectNameEXT"));

		// Set flag if at least one function pointer is present
		m_DebugUtilsSupported = (vkCreateDebugUtilsMessengerEXT != VK_NULL_HANDLE);

		//Select global DebugLevel 
		gvkSetDebugUtilsObjectNameEXT = vkSetDebugUtilsObjectNameEXT;
	}
	else
	{
		IFNITY_LOG(LogCore, WARNING, "{} not present, debug utils are disabled.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		IFNITY_LOG(LogCore, WARNING, "Try running the sample from inside a Vulkan graphics debugger (e.g. RenderDoc)");

	}


}

void DeviceVulkan::BeginRenderDocTrace(VkCommandBuffer commandBuffer, const char* markerName, float color[ 4 ])
{
	if( vkCmdBeginDebugUtilsLabelEXT )
	{
		VkDebugUtilsLabelEXT labelInfo{};
		labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		labelInfo.pLabelName = markerName;
		labelInfo.color[ 0 ] = color[ 0 ];
		labelInfo.color[ 1 ] = color[ 1 ];
		labelInfo.color[ 2 ] = color[ 2 ];
		labelInfo.color[ 3 ] = color[ 3 ];
		vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &labelInfo);
	}
	else
	{
		IFNITY_LOG(LogCore, WARNING, "{} not present, debug utils are disabled.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		IFNITY_LOG(LogCore, WARNING, "Try running the sample from inside a Vulkan graphics debugger (e.g. RenderDoc)");
	}
}

bool DeviceVulkan::createVulkanImmediateCommands()
{
	IFNITY_ASSERT_MSG(deviceQueues_.graphicsQueueFamilyIndex != deviceQueues_.INVALID, "call CreateQueue First to get graphicsQueueFamilyIndex valid  ");
	immediate_ = std::make_unique<Vulkan::VulkanImmediateCommands>(device_.device, deviceQueues_.graphicsQueueFamilyIndex,
																   "Creating Immediate Commands");

	return true;
}

Vulkan::CommandBuffer& DeviceVulkan::acquireCommandBuffer()
{
	IFNITY_ASSERT_MSG(!currentCommandBuffer_.ctx_, "Cannot acquire more than 1 command buffer simultaneously");

	currentCommandBuffer_ = Vulkan::CommandBuffer(this);

	return currentCommandBuffer_;
}

Vulkan::TextureHandleSM DeviceVulkan::getCurrentSwapChainTexture()
{

	//Verify that have swapchain
	if( !hasSwapchain() )
	{
		return {};
	}

	Vulkan::TextureHandleSM tex = swapchain_->getCurrentTexture();

	if( !tex.valid() )
	{
		IFNITY_LOG(LogCore, ERROR, "No swapchain image acquired");
		return {};
	}

	auto* texptr = slootMapTextures_.get(tex);
	IFNITY_ASSERT_MSG(texptr->vkImageFormat_ != VK_FORMAT_UNDEFINED, "Invalid image format");

	return tex;


}

bool DeviceVulkan::hasSwapchain() const noexcept
{
	return swapchain_ != nullptr;
}

Vulkan::SubmitHandle DeviceVulkan::submit(Vulkan::CommandBuffer& commandBuffer, Vulkan::TextureHandleSM present)
{
	auto vkCmdBuffer = static_cast<Vulkan::CommandBuffer*>(&commandBuffer);


	IFNITY_ASSERT_MSG(vkCmdBuffer, "Not vkcmdbuffer ");
	IFNITY_ASSERT_MSG(vkCmdBuffer->ctx_, "Commandbuffer has not Vulkancontext assign.");
	IFNITY_ASSERT_MSG(vkCmdBuffer->wrapper_, "Commandbuffer has not command buffer wrapper ");

	//Prepare image to be presented
	if( present )
	{
		const Vulkan::VulkanImage& tex = *slootMapTextures_.get(present);
		IFNITY_ASSERT_MSG(tex.isSwapchainImage_, "No SwapChainImage acquire to submit");

		// prepare image for presentation the image might be coming from a compute shader
		const VkPipelineStageFlagBits srcStage = (tex.vkImageLayout_ == VK_IMAGE_LAYOUT_GENERAL)
			? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			: VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		tex.transitionLayout(vkCmdBuffer->wrapper_->cmdBuf_,
							 VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
							 srcStage,
							 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, // wait for all subsequent operations
							 VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS });
	}

	const bool shouldPresent = hasSwapchain() && present;

	vkCmdBuffer->lastSubmitHandle_ = immediate_->submit(*vkCmdBuffer->wrapper_);

	if( shouldPresent )
	{
		swapchain_->present(immediate_->acquireLastSubmitSemaphore());
	}

	//processDeferredTasks();

	Vulkan::SubmitHandle handle = vkCmdBuffer->lastSubmitHandle_;

	// reset
	currentCommandBuffer_ = {};

	return handle;



}

void DeviceVulkan::addGraphicsPipeline(Vulkan::GraphicsPipeline* pipeline)
{
	map_renderPipelines[ nextPipelineId ] = pipeline;
	nextPipelineId++;
}

void DeviceVulkan::bindDefaultDescriptorSets(VkCommandBuffer cmdBuf, VkPipelineBindPoint bindPoint, VkPipelineLayout layout) const
{

	const VkDescriptorSet dsets[ 2 ] = { vkDSet_, vkDSet_ };
	vkCmdBindDescriptorSets(
		cmdBuf,
		bindPoint,
		layout,
		0,
		(uint32_t)ARRAY_NUM_ELEMENTS(dsets),
		dsets,
		0,
		nullptr);
}



void DeviceVulkan::destroy(Vulkan::GraphicsPipelineHandleSM handle)
{
	slotMapRenderPipelines_.destroy(handle);
}

void DeviceVulkan::destroy(Vulkan::ShaderModuleHandleSM handle)
{
	const Vulkan::ShaderModuleState* state = slotMapShaderModules_.get(handle);

	if( !state )
	{
		IFNITY_LOG(LogCore, ERROR, "Invalid ShaderModuleHandleSM to destroy fail");
		return;
	}

	if( state->sm != VK_NULL_HANDLE )
	{
		// a shader module can be destroyed while pipelines created using its shaders are still in use
		// https://registry.khronos.org/vulkan/specs/1.3/html/chap9.html#vkDestroyShaderModule
		vkDestroyShaderModule(device_.device, state->sm, nullptr);
	}

	slotMapShaderModules_.destroy(handle);

}

void DeviceVulkan::destroy(Vulkan::BufferHandleSM handle)
{
	SCOPE_EXIT{
	  slotMapBuffers_.destroy(handle);
	};

	Vulkan::VulkanBuffer* buf = slotMapBuffers_.get(handle);

	if( !buf )
	{
		return;
	}

	if( VMA_ALLOCATOR_VK )
	{
		if( buf->mappedPtr_ )
		{
			vmaUnmapMemory((VmaAllocator)getVmaAllocator(), buf->vmaAllocation_);
		}

		vmaDestroyBuffer((VmaAllocator)getVmaAllocator(), buf->vkBuffer_, buf->vmaAllocation_);
	}
	else
	{
		if( buf->mappedPtr_ )
		{
			vkUnmapMemory(device_, buf->vkMemory_);
		}

		vkDestroyBuffer(device_, buf->vkBuffer_, nullptr);
		vkFreeMemory(device_, buf->vkMemory_, nullptr);
	}
}

void DeviceVulkan::destroy(Vulkan::SamplerHandleSM handle)
{
	VkSampler sampler = *slotMapSamplers_.get(handle);

	//In the future will be delete at pipeline time or futureTask delete 
	if( sampler != VK_NULL_HANDLE )
	{
		vkDestroySampler(device_, sampler, nullptr);
	}
	slotMapSamplers_.destroy(handle);



}

Vulkan::SamplerHandleSM DeviceVulkan::createSampler(const VkSamplerCreateInfo& ci, const char* debugName)
{

	VkSamplerCreateInfo scinfo{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 0.0f,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = static_cast<float>(Vulkan::MAX_MIP_LEVELS - 1),
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
	};

	VkSampler sampler = VK_NULL_HANDLE;
	VK_ASSERT(vkCreateSampler(device_, &ci, nullptr, &sampler));
	VK_ASSERT(setDebugObjectName(device_, VK_OBJECT_TYPE_SAMPLER, (uint64_t)sampler, debugName));

	Vulkan::SamplerHandleSM handle = slotMapSamplers_.create(VkSampler(sampler));

	awaitingCreation_ = true;

	return handle;



}

void DeviceVulkan::checkAndUpdateDescriptorSets()
{

	if( !awaitingCreation_ )
	{
		// nothing to update here
		return;
	}



	// update Vulkan descriptor set here

	// make sure the guard values are always there
	IFNITY_ASSERT(slootMapTextures_.numObjects() >= 1);
	IFNITY_ASSERT(slotMapSamplers_.numObjects() >= 1);

	uint32_t newMaxTextures = currentMaxTextures_;
	uint32_t newMaxSamplers = currentMaxSamplers_;


	while( slootMapTextures_.slots.size() > newMaxTextures )
	{
		newMaxTextures *= 2;
	}
	while( slotMapSamplers_.slots.size() > newMaxSamplers )
	{
		newMaxSamplers *= 2;
	}

	if( newMaxTextures != currentMaxTextures_ || newMaxSamplers != currentMaxSamplers_)
	{
		growDescriptorPool(newMaxTextures, newMaxSamplers);
	}

	// 1. Sampled and storage images
	std::vector<VkDescriptorImageInfo> infoSampledImages;
	std::vector<VkDescriptorImageInfo> infoStorageImages;
	

	infoSampledImages.reserve(slootMapTextures_.numObjects());
	infoStorageImages.reserve(slootMapTextures_.numObjects());


	// use the dummy texture to avoid sparse array
	VkImageView dummyImageView = slootMapTextures_.slots[ 0 ].obj.imageView_;

	for( const auto& obj : slootMapTextures_.slots )
	{
		const Vulkan::VulkanImage& img = obj.obj;
		const VkImageView view = obj.obj.imageView_;
		const VkImageView storageView = obj.obj.imageViewStorage_ ? obj.obj.imageViewStorage_ : view;
		// multisampled images cannot be directly accessed from shaders
		const bool isTextureAvailable = (img.vkSamples_ & VK_SAMPLE_COUNT_1_BIT) == VK_SAMPLE_COUNT_1_BIT;
		const bool isSampledImage = isTextureAvailable && img.isSampledImage();
		const bool isStorageImage = isTextureAvailable && img.isStorageImage();
		infoSampledImages.push_back(VkDescriptorImageInfo{
			.sampler = VK_NULL_HANDLE,
			.imageView = isSampledImage ? view : dummyImageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
									});

		//Not for now
	/*	IFNITY_ASSERT(infoSampledImages.back().imageView != VK_NULL_HANDLE);
		infoStorageImages.push_back(VkDescriptorImageInfo{
			.sampler = VK_NULL_HANDLE,
			.imageView = isStorageImage ? storageView : dummyImageView,
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
									});*/
	}

	// 2. Samplers
	std::vector<VkDescriptorImageInfo> infoSamplers;
	infoSamplers.reserve(slotMapSamplers_.numObjects());

	for( const auto& sampler : slotMapSamplers_.slots )
	{
		infoSamplers.push_back({
			.sampler = sampler.obj ? sampler.obj : slotMapSamplers_.slots[ 0 ].obj,
			.imageView = VK_NULL_HANDLE,
			.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
							   });
	}

	

	VkWriteDescriptorSet write[ kBinding_NumBindings ] = {};
	uint32_t numWrites = 0;

	

	if( !infoSampledImages.empty() )
	{
		write[ numWrites++ ] = VkWriteDescriptorSet{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vkDSet_,
			.dstBinding = kBinding_Textures,
			.dstArrayElement = 0,
			.descriptorCount = (uint32_t)infoSampledImages.size(),
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.pImageInfo = infoSampledImages.data(),
		};
	}

	if( !infoSamplers.empty() )
	{
		write[ numWrites++ ] = VkWriteDescriptorSet{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vkDSet_,
			.dstBinding = kBinding_Samplers,
			.dstArrayElement = 0,
			.descriptorCount = (uint32_t)infoSamplers.size(),
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
			.pImageInfo = infoSamplers.data(),
		};
	}

	if( !infoStorageImages.empty() )
	{
		write[ numWrites++ ] = VkWriteDescriptorSet{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vkDSet_,
			.dstBinding = kBinding_StorageImages,
			.dstArrayElement = 0,
			.descriptorCount = (uint32_t)infoStorageImages.size(),
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			.pImageInfo = infoStorageImages.data(),
		};
	}

	

	// do not switch to the next descriptor set if there is nothing to update
	if( numWrites )
	{
		IFNITY_LOG(LogCore, TRACE, "Updating descriptor set with %u writes", numWrites);
		immediate_->wait(immediate_->getLastSubmitHandle());
		
		vkUpdateDescriptorSets(device_, numWrites, write, 0, nullptr);
		
	}

	awaitingCreation_ = false;
}



void DeviceVulkan::destroy(Vulkan::TextureHandleSM handle)
{



	Vulkan::VulkanImage* tex = slootMapTextures_.get(handle);

	if( !tex )
	{
		return;
	}

	if( !tex->isOwningVkImage_ )
	{
		return;
	}

	SCOPE_EXIT{
		if( tex->imageView_ != VK_NULL_HANDLE )
	  {
		  vkDestroyImageView(device_, tex->imageView_, nullptr);
	  }
		  slootMapTextures_.destroy(handle);

	};

	if( VMA_ALLOCATOR_VK )
	{
		if( tex->mappedPtr_ )
		{
			vmaUnmapMemory((VmaAllocator)getVmaAllocator(), tex->vmaAllocation_);
		}

		vmaDestroyImage((VmaAllocator)getVmaAllocator(), tex->vkImage_, tex->vmaAllocation_);
	}
	else
	{
		if( tex->mappedPtr_ )
		{
			vkUnmapMemory(device_, tex->vkMemory_[ 0 ]);
		}


		vkFreeMemory(device_, tex->vkMemory_[ 0 ], nullptr);
	}
}




Vulkan::ShaderModuleState DeviceVulkan::createShaderModuleFromSpirVconst(const void* spirv, size_t numBytes, const char* debugName)
{
	return Vulkan::ShaderModuleState();
}

void DeviceVulkan::EndRenderDocTrace(VkCommandBuffer commandBuffer)
{
	if( vkCmdEndDebugUtilsLabelEXT )
	{
		vkCmdEndDebugUtilsLabelEXT(commandBuffer);
	}
}


IFNITY_END_NAMESPACE


