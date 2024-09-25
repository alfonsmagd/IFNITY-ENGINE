

#include "DeviceVulkan.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"


IFNITY_NAMESPACE 




void DeviceVulkan::OnUpdate()
{
	// Get the index of the next available image in the swap chain
	
	AcquireNextImage();

	PopulateCommandBuffer();

	SubmitCommandBuffer();

	PresentImage();

	// WAITING FOR THE GPU TO COMPLETE THE FRAME BEFORE CONTINUING IS NOT BEST PRACTICE.
	// vkQueueWaitIdle is used for simplicity.
	// (so that we can reuse the command buffer indexed with m_commandBufferIndex)
	VK_CHECK(vkQueueWaitIdle(m_GraphicsQueue), "Failed to wait for queue to idle");

	// Update command buffer index
	m_commandBufferIndex = (m_commandBufferIndex + 1) % m_commandBufferCount;


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


	// Get extensions supported by the instance and store for later use
	uint32_t count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
	std::vector<VkExtensionProperties> instance_extensions;
	std::vector<std::string> extensions_supported;

	if (count > 0)
	{
		instance_extensions.resize(count);

		if (vkEnumerateInstanceExtensionProperties(nullptr, &count, &instance_extensions.front()) == VK_SUCCESS)
		{
			for (size_t idx = 0; idx < instance_extensions.size(); idx++)
			{
				extensions_supported.push_back(instance_extensions[idx].extensionName);
			}
		}
	}
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
		!CreateRenderPass()     || 
		!CreateFrameBuffer()    ||	
		!CreateCommandPool()	||
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
	m_Framebuffers.resize(m_Swapchain.get_image_views().value().size());

	for (size_t i = 0; i < m_Framebuffers.size(); i++)
	{
		attachments[0] = m_Swapchain.get_image_views().value()[i];
		VK_CHECK(vkCreateFramebuffer(m_Device.device, &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]), "Failed to create framebuffer");

	}

	return true;
}

bool DeviceVulkan::CreateCommandBuffers()
{
	//Create one command buffer for each swap chain image
	m_CommandBuffers.resize(m_Framebuffers.size());

	//Allocate command buffers
	VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = m_CommandPool;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	VK_CHECK(vkAllocateCommandBuffers(m_Device.device, &cmdBufferAllocInfo, m_CommandBuffers.data()), "Failed to allocate command buffers");

	return true;
}

bool DeviceVulkan::CreateSyncObjects()
{
	//Create synchronization objects
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;

	//Create a semaphore used to synchronize image presentation
	VK_CHECK(vkCreateSemaphore(m_Device.device, &semaphoreCreateInfo, nullptr, &m_PresentSemaphore), "Failed to create present semaphore");

	//Create a semaphore used to synchronize render completion
	VK_CHECK(vkCreateSemaphore(m_Device.device, &semaphoreCreateInfo, nullptr, &m_RenderSemaphore), "Failed to create render semaphore");


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

	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufInfo.pNext = nullptr;
	cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	// We use a single color attachment that is cleared at the start of the subpass.
	VkClearValue clearValues[1];
	clearValues[0].color = { { 0.0f, 0.2f, 0.4f, 1.0f } };

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = m_RenderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = m_Swapchain.extent.width;
	renderPassBeginInfo.renderArea.extent.height = m_Swapchain.extent.height;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.framebuffer = m_Framebuffers[m_imageIndex]; // Set the frame buffer to specify the color attachment																	  // (render target) where to draw the current frame.
	// Initialize begin command buffer 
	VK_CHECK(vkBeginCommandBuffer(m_CommandBuffers[m_commandBufferIndex], &cmdBufInfo), "Fail beginCommandBuffer ");

	//Command Scissors and Viewport
	VkViewport viewport = {};
	viewport.width = (float)m_Swapchain.extent.width;
	viewport.height = (float)m_Swapchain.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vkCmdSetViewport(m_CommandBuffers[m_commandBufferIndex], 0, 1, &viewport);

	//Set the scissor rectangle 

	VkRect2D scissor = {};
	scissor.extent.width = m_Swapchain.extent.width;
	scissor.extent.height = m_Swapchain.extent.height;
	scissor.offset.x = 0;
	scissor.offset.y = 0;

	vkCmdSetScissor(m_CommandBuffers[m_commandBufferIndex], 0, 1, &scissor);

	// Start the main render pass cmd 
	// Begin the render pass instance.
	// This will clear the color attachment.
	vkCmdBeginRenderPass(m_CommandBuffers[m_commandBufferIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	

	// End the render pass cmd

	vkCmdEndRenderPass(m_CommandBuffers[m_commandBufferIndex]);

	// End the command buffer 
	VK_CHECK(vkEndCommandBuffer(m_CommandBuffers[m_commandBufferIndex]), "Fail endCommandBuffer ");


	return false;
}

bool DeviceVulkan::SubmitCommandBuffer()
{
	// Pipeline stage at which the queue submission will wait(via pWaitSemaphores)
	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	//VkSubmitCreate Info 
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;				// One wait semaphore
	submitInfo.pWaitDstStageMask = &waitStageMask;  // Pointer to the list of pipeline stages that the semaphore waits will occur at
	submitInfo.commandBufferCount = 1;				// One command buffer
	submitInfo.pCommandBuffers = &m_CommandBuffers[m_commandBufferIndex];
	submitInfo.signalSemaphoreCount = 1;


	submitInfo.pSignalSemaphores = &m_RenderSemaphore; // Semaphore(s) to be signaled when command buffers have completed
	submitInfo.pWaitSemaphores = &m_PresentSemaphore;  // Semaphore(s) to wait upon before the submitted command buffers start executing

	VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE), "Failed to submit command buffer");

	return false;
}

bool DeviceVulkan::PresentImage()
{
	// Present the current image to the presentation engine.
	// Pass the semaphore from the submit info as the wait semaphore for swap chain presentation.
	// This ensures that the image is not presented to the windowing system until all commands have been executed.
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_Swapchain.swapchain;
	presentInfo.pImageIndices = &m_imageIndex;
	// Check if a wait semaphore has been specified to wait for before presenting the image
	if (m_RenderSemaphore != VK_NULL_HANDLE)
	{
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_RenderSemaphore;
	}

	VkResult present = vkQueuePresentKHR(m_PresentQueue, &presentInfo);
	if (!((present == VK_SUCCESS) || (present == VK_SUBOPTIMAL_KHR)))
	{
		if (present == VK_ERROR_OUT_OF_DATE_KHR)
		{
			ResizeSwapChain();
		}
			
	}
	return false;
}


IFNITY_END_NAMESPACE