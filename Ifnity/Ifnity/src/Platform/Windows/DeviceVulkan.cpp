

#include "DeviceVulkan.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

//#define IMGUI_DEMO 

IFNITY_NAMESPACE

static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}


void DeviceVulkan::OnUpdate()
{
	if (vkWaitForFences(m_Device.device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to wait for fences");

	}

	if (vkResetFences(m_Device.device, 1, &m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to reset fences");

	}

	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(m_Device.device,
		m_Swapchain,
		UINT64_MAX,
		m_ImageAvailableSemaphores[m_CurrentFrame],
		VK_NULL_HANDLE,
		&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return ResizeSwapChain();
	}
	else
	{
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to acquire next image");
		}
	}


	if (vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to reset command buffer");
	}

	VkCommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &cmdBeginInfo) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to begin command buffer");
	}

	

	VkClearValue colorClearValue;
    colorClearValue.color = { { m_Color[0], m_Color[1], m_Color[2], m_Color[3] } };

	float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; // Rojo
	
	VkClearValue depthValue;
	depthValue.depthStencil.depth = 1.0f;

	VkClearValue clearValues[] = { colorClearValue, depthValue };

	VkRenderPassBeginInfo rpInfo{};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.renderPass = m_RenderPass;

	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = m_Swapchain.extent;
	rpInfo.framebuffer = m_Framebuffers[imageIndex];

	rpInfo.clearValueCount = 2;
	rpInfo.pClearValues = clearValues;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_Swapchain.extent.width);
	viewport.height = static_cast<float>(m_Swapchain.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_Swapchain.extent;

	BeginRenderDocTrace(m_CommandBuffers[m_CurrentFrame], "Render Pass Begin 11111", color);
	vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	///* the rendering itself happens here */
	//if (!mUseChangedShader) {
	//  vkCmdBindPipeline(mRenderData.rdCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderData.rdBasicPipeline);
	//} else {
	//  vkCmdBindPipeline(mRenderData.rdCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderData.rdChangedPipeline);
	//}

	///* required for dynamic viewport */
	//vkCmdSetViewport(mRenderData.rdCommandBuffer, 0, 1, &viewport);
	//vkCmdSetScissor(mRenderData.rdCommandBuffer, 0, 1, &scissor);

	///* the triangle drawing itself */
	//VkDeviceSize offset = 0;
	//vkCmdBindVertexBuffers(mRenderData.rdCommandBuffer, 0, 1, &mVertexBuffer, &offset);

	//vkCmdBindDescriptorSets(mRenderData.rdCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderData.rdPipelineLayout, 0, 1, &mRenderData.rdTextureDescriptorSet, 0, nullptr);
	//vkCmdBindDescriptorSets(mRenderData.rdCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderData.rdPipelineLayout, 1, 1, &mRenderData.rdUBODescriptorSet, 0, nullptr);

	//vkCmdDraw(mRenderData.rdCommandBuffer, mRenderData.rdTriangleCount * 3, 1, 0, 0);

	// imgui overlay
	
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffers[m_CurrentFrame]);

	vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);
	EndRenderDocTrace(m_CommandBuffers[m_CurrentFrame]);

	if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to end command buffer ");
		
	}

	///* upload UBO data after commands are created */
	//void* data;
	//vmaMapMemory(mRenderData.rdAllocator, mRenderData.rdUboBufferAlloc, &data);
	//std::memcpy(data, &mMatrices, static_cast<uint32_t>(sizeof(VkUploadMatrices)));
	//vmaUnmapMemory(mRenderData.rdAllocator, mRenderData.rdUboBufferAlloc);

	/* submit command buffer */
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submitInfo.pWaitDstStageMask = &waitStage;

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphores[m_CurrentFrame];

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to submit command buffer");
		
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_Swapchain.swapchain;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		return ResizeSwapChain();
	}
	else
	{
		if (result != VK_SUCCESS)
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to present image");	
		
		}
	}

	m_CurrentFrame = (m_CurrentFrame + 1) % 3;


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



	if (!CreateSurface() ||
		!CreatePhysicalDevice() ||
		!CreateDevice() ||
		!CreateVmaAllocator() ||
		!GetQueue() ||
		!CreateSwapChain() ||
		!CreateCommandPool() ||
		!CreateRenderPass() ||
		!CreateFrameBuffer() ||
		!CreateCommandBuffers() ||
		!CreateSyncObjects() 

		)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed Initialization Process ");
		return false;
	}

	IFNITY_LOG(LogCore, INFO, "Vulkan Device and Context Initialized");



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
	InitGui();
}

void DeviceVulkan::InternalPreDestroy()
{
	//Wait devide to finish and get idle state.
	vkDeviceWaitIdle(m_Device.device);
}

void DeviceVulkan::ClearBackBuffer(float* color)
{
	std::copy(color, color + 4, m_Color);
}


DeviceVulkan::~DeviceVulkan()
{
	

	//Destroy Descriptor Pool 
	vkDestroyDescriptorPool(m_Device.device, m_ImGuiDescriptorPool, nullptr);

	//Destroy Sync Objects
	DestroySyncObjects();
	DestroyCommandBuffers();
	DestroyCommandPool();
	CleanFrameBuffers();
	DestroyRenderPass();

	//Destroy Vma Allocator
	vmaDestroyAllocator(m_Allocator);

	// Destroy Debug Utils Messenger
	if (debugUtilsMessenger != VK_NULL_HANDLE)
	{
		DestroyDebugUtilsMessengerEXT(m_Instance, debugUtilsMessenger, nullptr);
	}
	

	// Destroy VkBootStrap
	m_Swapchain.destroy_image_views(m_SwapchainImageViews);

	vkb::destroy_swapchain(m_Swapchain);
	vkb::destroy_device(m_Device);
	vkb::destroy_surface(m_Instance, m_Surface);
	vkb::destroy_instance(m_Instance);

	

	IFNITY_LOG(LogCore, INFO, "Delete Device Vulkan Render.");

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

	vkb::PhysicalDeviceSelector physicalDevSel{ m_Instance };
	auto physicalDevSelRet = physicalDevSel.set_surface(m_Surface).select();
	if (!physicalDevSelRet)
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

	for (const auto& ext : extensions)
	{
		IFNITY_LOG(LogCore, INFO, "Supported extension: {}", ext.extensionName);
	}

	// Verificar si VK_EXT_debug_marker está en la lista de extensiones soportadas
	bool debugMarkerSupported = false;
	for (const auto& ext : extensions)
	{
		if (strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
		{
			debugMarkerSupported = true;
			break;
		}
	}

	if (debugMarkerSupported)
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
	if (!deviceRet)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create device in Vulkan Device");
		return false;
	}
	m_Device = deviceRet.value();

#ifdef _DEBUG
	VkDebugUtilsMessengerCreateInfoEXT create_info;
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debugUtilsMessengerCallback;
	create_info.pNext = NULL;
	create_info.flags = 0;


	if (CreateDebugUtilsMessengerEXT(m_Instance, &create_info, nullptr, &debugUtilsMessenger) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create debug utils messenger in Vulkan Device");
		return false;

	}

	setupCallbacks(m_Device.device);

#endif





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
	vkb::SwapchainBuilder swapChainBuild{ m_Device };

	/* VK_PRESENT_MODE_FIFO_KHR enables vsync */
	auto swapChainBuildRet = swapChainBuild.set_old_swapchain(m_Swapchain).set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR).build();
	if (!swapChainBuildRet)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create swapchain in Vulkan Device");
		return false;
	}

	vkb::destroy_swapchain(m_Swapchain);
	m_Swapchain = swapChainBuildRet.value();

	//Get image_count 
	m_commandBufferCount = m_Swapchain.image_count;

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

void DeviceVulkan::DestroySyncObjects()
{
	for (size_t i = 0; i < 3; i++)
	{
		vkDestroySemaphore(m_Device.device, m_ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_Device.device, m_RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_Device.device, m_InFlightFences[i], nullptr);
	}
	
}

void DeviceVulkan::DestroyCommandBuffers()
{
	vkFreeCommandBuffers(m_Device.device,
		m_CommandPool, 
		static_cast<uint32_t>(m_CommandBuffers.size()),
		m_CommandBuffers.data());

}

void DeviceVulkan::CleanFrameBuffers()
{
	for (auto& framebuffer : m_Framebuffers)
	{
		vkDestroyFramebuffer(m_Device.device, framebuffer, nullptr);
	}
}

void DeviceVulkan::DestroyRenderPass()
{
	vkDestroyRenderPass(m_Device.device, m_RenderPass, nullptr);
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
	attachments[0].format = m_Swapchain.image_format;								// Use the color format selected by the swapchain
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;                                 // We don't use multi sampling in this example
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear this attachment at the start of the render pass
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                          // Keep its contents after the render pass is finished (for displaying it)
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // Similar to loadOp, but for stenciling (we don't use stencil here)
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // Similar to storeOp, but for stenciling (we don't use stencil here)
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;                   // Layout to which the attachment is transitioned when the render pass is finished
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
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_NONE;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;


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

	VK_CHECK(vkCreateRenderPass(m_Device.device, &renderPassInfo, nullptr, &m_RenderPass), "Failed to create render pass");


	return true;
}

bool DeviceVulkan::CreateFrameBuffer()
{
	m_SwapchainImages     = m_Swapchain.get_images().value();
	m_SwapchainImageViews = m_Swapchain.get_image_views().value();


	VkImageView attachments[1] = {};


	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = NULL;
	frameBufferCreateInfo.renderPass = m_RenderPass;
	frameBufferCreateInfo.attachmentCount = 1;
	frameBufferCreateInfo.pAttachments = attachments;
	frameBufferCreateInfo.width = m_Swapchain.extent.width;
	frameBufferCreateInfo.height = m_Swapchain.extent.height;
	frameBufferCreateInfo.layers = 1;

	//Create Framebuffers for each swapchain image view
	m_Framebuffers.resize(m_SwapchainImageViews.size());

	for (size_t i = 0; i < m_Framebuffers.size(); i++)
	{
		attachments[0] = m_SwapchainImageViews.at(i);
		VK_CHECK(vkCreateFramebuffer(m_Device.device, &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]), "Failed to create framebuffer");

	}

	return true;
}

bool DeviceVulkan::CreateCommandBuffers()
{
	m_CommandBuffers.resize(m_Framebuffers.size());

	VkCommandBufferAllocateInfo bufferAllocInfo{};
	bufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferAllocInfo.commandPool = m_CommandPool;
	bufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	bufferAllocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_Device.device, &bufferAllocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
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

	for (size_t i = 0; i < 3; i++)
	{
		if (vkCreateSemaphore(m_Device.device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_Device.device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_Device.device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to create synchronization objects for a frame");
			return false;
		}
	}


	return true;
}

bool DeviceVulkan::AcquireNextImage()
{
	//Get the index of the next available swapchain image
	VkResult result = vkAcquireNextImageKHR(m_Device.device, m_Swapchain.swapchain, UINT64_MAX, m_PresentSemaphore, (VkFence)nullptr, &m_imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//Swapchain is out of date (e.g. the window was resized) and
		//must be recreated:
		ResizeSwapChain();
		return false;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
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
	if (!CreateImGuiDescriptorPool())
	{
		return false;
	}


	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_Instance;
	init_info.PhysicalDevice = m_PhysicalDevice;
	init_info.Device = m_Device;
	init_info.Queue = m_GraphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = m_ImGuiDescriptorPool;
	init_info.RenderPass = m_RenderPass;
	init_info.Subpass = 0;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info);

	//Upload Fonts if this was needed.


	return true;
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

	if (vkCreateDescriptorPool(m_Device, &pool_info, nullptr, &m_ImGuiDescriptorPool) != VK_SUCCESS)
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
		for (auto& extension : extensions)
		{
			if (strcmp(extension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
			{
				extensionPresent = true;
				break;
			}
		}

		if (extensionPresent)
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
		}
		else
		{
			IFNITY_LOG(LogCore, WARNING, "{} not present, debug utils are disabled.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			IFNITY_LOG(LogCore, WARNING, "Try running the sample from inside a Vulkan graphics debugger (e.g. RenderDoc)");

		}
	

}

void DeviceVulkan::BeginRenderDocTrace(VkCommandBuffer commandBuffer, const char* markerName, float color[4])
{
	if (vkCmdBeginDebugUtilsLabelEXT)
	{
		VkDebugUtilsLabelEXT labelInfo{};
		labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		labelInfo.pLabelName = markerName;
		labelInfo.color[0] = color[0];
		labelInfo.color[1] = color[1];
		labelInfo.color[2] = color[2];
		labelInfo.color[3] = color[3];
		vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &labelInfo);
	}
	else
	{
		IFNITY_LOG(LogCore, WARNING, "{} not present, debug utils are disabled.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		IFNITY_LOG(LogCore, WARNING, "Try running the sample from inside a Vulkan graphics debugger (e.g. RenderDoc)");
	}
}

void DeviceVulkan::EndRenderDocTrace(VkCommandBuffer commandBuffer)
{
	if (vkCmdEndDebugUtilsLabelEXT)
	{
		vkCmdEndDebugUtilsLabelEXT(commandBuffer);
	}
}


IFNITY_END_NAMESPACE


