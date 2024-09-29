

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
#ifdef IMGUI_DEMO


static bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension)
{
	for (const VkExtensionProperties& p : properties)
		if (strcmp(p.extensionName, extension) == 0)
			return true;
	return false;
}

static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice()
{
	uint32_t gpu_count;
	VkResult err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
	check_vk_result(err);
	IM_ASSERT(gpu_count > 0);

	ImVector<VkPhysicalDevice> gpus;
	gpus.resize(gpu_count);
	err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.Data);
	check_vk_result(err);

	// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
	// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
	// dedicated GPUs) is out of scope of this sample.
	for (VkPhysicalDevice& device : gpus)
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return device;
	}

	// Use first GPU (Integrated) is a Discrete one is not available.
	if (gpu_count > 0)
		return gpus[0];
	return VK_NULL_HANDLE;
}

static void SetupVulkan(ImVector<const char*> instance_extensions)
{
	VkResult err;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
	volkInitialize();
#endif

	// Create Vulkan Instance
	{
		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		// Enumerate available extensions
		uint32_t properties_count;
		ImVector<VkExtensionProperties> properties;
		vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
		properties.resize(properties_count);
		err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.Data);
		check_vk_result(err);

		// Enable required extensions
		if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
			instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
		{
			instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		}
#endif

		// Enabling validation layers
#ifdef APP_USE_VULKAN_DEBUG_REPORT
		const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
		create_info.enabledLayerCount = 1;
		create_info.ppEnabledLayerNames = layers;
		instance_extensions.push_back("VK_EXT_debug_report");
#endif

		// Create Vulkan Instance
		create_info.enabledExtensionCount = (uint32_t)instance_extensions.Size;
		create_info.ppEnabledExtensionNames = instance_extensions.Data;
		err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
		check_vk_result(err);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
		volkLoadInstance(g_Instance);
#endif

		// Setup the debug report callback
#ifdef APP_USE_VULKAN_DEBUG_REPORT
		auto f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT");
		IM_ASSERT(f_vkCreateDebugReportCallbackEXT != nullptr);
		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = debug_report;
		debug_report_ci.pUserData = nullptr;
		err = f_vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci, g_Allocator, &g_DebugReport);
		check_vk_result(err);
#endif
	}

	// Select Physical Device (GPU)
	g_PhysicalDevice = SetupVulkan_SelectPhysicalDevice();

	// Select graphics queue family
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
		VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
		for (uint32_t i = 0; i < count; i++)
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				g_QueueFamily = i;
				break;
			}
		free(queues);
		IM_ASSERT(g_QueueFamily != (uint32_t)-1);
	}

	// Create Logical Device (with 1 queue)
	{
		ImVector<const char*> device_extensions;
		device_extensions.push_back("VK_KHR_swapchain");

		// Enumerate physical device extension
		uint32_t properties_count;
		ImVector<VkExtensionProperties> properties;
		vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count, nullptr);
		properties.resize(properties_count);
		vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
			device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

		const float queue_priority[] = { 1.0f };
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = g_QueueFamily;
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = (uint32_t)device_extensions.Size;
		create_info.ppEnabledExtensionNames = device_extensions.Data;
		err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
		check_vk_result(err);
		vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
	}

	// Create Descriptor Pool
	// The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
	// If you wish to load e.g. additional textures you may need to alter pools sizes.
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
		err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
		check_vk_result(err);
	}
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
{
	wd->Surface = surface;

	// Check for WSI support
	VkBool32 res;
	vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
	if (res != VK_TRUE)
	{
		fprintf(stderr, "Error no WSI support on physical device 0\n");
		exit(-1);
	}

	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

	// Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
	wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

	// Create SwapChain, RenderPass, Framebuffer, etc.
	IM_ASSERT(g_MinImageCount >= 2);
	ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
}

static void CleanupVulkan()
{
	vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

#ifdef APP_USE_VULKAN_DEBUG_REPORT
	// Remove the debug report callback
	auto f_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT");
	f_vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, g_Allocator);
#endif // APP_USE_VULKAN_DEBUG_REPORT

	vkDestroyDevice(g_Device, g_Allocator);
	vkDestroyInstance(g_Instance, g_Allocator);
}

static void CleanupVulkanWindow()
{
	ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
}

static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
	VkResult err;

	VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		g_SwapChainRebuild = true;
		return;
	}
	check_vk_result(err);

	ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
	{
		err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		check_vk_result(err);

		err = vkResetFences(g_Device, 1, &fd->Fence);
		check_vk_result(err);
	}
	{
		err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
		check_vk_result(err);
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
		check_vk_result(err);
	}
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = wd->RenderPass;
		info.framebuffer = fd->Framebuffer;
		info.renderArea.extent.width = wd->Width;
		info.renderArea.extent.height = wd->Height;
		info.clearValueCount = 1;
		info.pClearValues = &wd->ClearValue;
		vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	// Record dear imgui primitives into command buffer
	ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

	// Submit command buffer
	vkCmdEndRenderPass(fd->CommandBuffer);
	{
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &fd->CommandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(fd->CommandBuffer);
		check_vk_result(err);
		err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
		check_vk_result(err);
	}
}

static void FramePresent(ImGui_ImplVulkanH_Window* wd)
{
	if (g_SwapChainRebuild)
		return;
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &wd->Swapchain;
	info.pImageIndices = &wd->FrameIndex;
	VkResult err = vkQueuePresentKHR(g_Queue, &info);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		g_SwapChainRebuild = true;
		return;
	}
	check_vk_result(err);
	wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
}

static void InitGui_demo(ImGui_ImplVulkanH_Window* wd)
{
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = g_Instance;
	init_info.PhysicalDevice = g_PhysicalDevice;
	init_info.Device = g_Device;
	init_info.QueueFamily = g_QueueFamily;
	init_info.Queue = g_Queue;
	init_info.PipelineCache = g_PipelineCache;
	init_info.DescriptorPool = g_DescriptorPool;
	init_info.RenderPass = wd->RenderPass;
	init_info.Subpass = 0;
	init_info.MinImageCount = g_MinImageCount;
	init_info.ImageCount = wd->ImageCount;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = g_Allocator;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info);
}

#endif

void DeviceVulkan::OnUpdate()
{
	if (vkWaitForFences(m_Device.device, 1, &rdRenderFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to wait for fences");

	}

	if (vkResetFences(m_Device.device, 1, &rdRenderFence) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to reset fences");

	}

	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(m_Device.device,
		m_Swapchain,
		UINT64_MAX,
		m_PresentSemaphore,
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


	if (vkResetCommandBuffer(m_CommandBuffers, 0) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to reset command buffer");
	}

	VkCommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(m_CommandBuffers, &cmdBeginInfo) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to begin command buffer");
	}

	VkClearValue colorClearValue;
	colorClearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f } };

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


	vkCmdBeginRenderPass(m_CommandBuffers, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

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
	
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffers);

	vkCmdEndRenderPass(m_CommandBuffers);

	if (vkEndCommandBuffer(m_CommandBuffers) != VK_SUCCESS)
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
	submitInfo.pWaitSemaphores = &m_PresentSemaphore;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_RenderSemaphore;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers;

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, rdRenderFence) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to submit command buffer");
		
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_RenderSemaphore;

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


#ifdef IMGUI_DEMO
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImDrawData* draw_data = ImGui::GetDrawData();
	ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
	const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
	if (!is_minimized)
	{
		wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
		wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
		wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
		wd->ClearValue.color.float32[3] = clear_color.w;
		FrameRender(wd, draw_data);
		FramePresent(wd);
	}

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
#ifdef IMGUI_DEMO

	ImVector<const char*> extensions;
	uint32_t extensions_count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
	for (uint32_t i = 0; i < extensions_count; i++)
		extensions.push_back(glfw_extensions[i]);
	SetupVulkan(extensions);


	// Create Window Surface
	VkSurfaceKHR surface;
	VkResult err = glfwCreateWindowSurface(g_Instance, m_Window, g_Allocator, &surface);
	check_vk_result(err);

	// Create Framebuffers
	int w, h;
	glfwGetFramebufferSize(m_Window, &w, &h);
	ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
	SetupVulkanWindow(wd, surface, w, h);



#endif


	if (!CreateSurface() ||
		!CreatePhysicalDevice() ||
		!CreateDevice() ||
		!CreateVmaAllocator() ||
		!GetQueue() ||
		!CreateSwapChain() ||
		!CreateCommandPool() ||
		!CreateCommandBuffers() ||
		!CreateRenderPass() ||
		!CreateFrameBuffer() ||
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

#ifdef IMGUI_DEMO
	InitGui_demo(&g_MainWindowData);
#endif
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
	VkCommandBufferAllocateInfo bufferAllocInfo{};
	bufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferAllocInfo.commandPool = m_CommandPool;
	bufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	bufferAllocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_Device.device, &bufferAllocInfo, &m_CommandBuffers) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to allocate command buffers");
		return false;
	}

	return true;
}

bool DeviceVulkan::CreateSyncObjects()
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(m_Device.device, &semaphoreInfo, nullptr, &m_PresentSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(m_Device.device, &semaphoreInfo, nullptr, &m_RenderSemaphore) != VK_SUCCESS ||
		vkCreateFence(m_Device.device, &fenceInfo, nullptr, &rdRenderFence) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create synchronization objects for a frame");
		return false;
	}
	return true;


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
#ifdef _DEBUG
	//assert(false == is_active);

	vkDebugMarkerSetObjectTag = (PFN_vkDebugMarkerSetObjectTagEXT)vkGetDeviceProcAddr(i_device, "vkDebugMarkerSetObjectTagEXT");
	vkDebugMarkerSetObjectName = (PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(i_device, "vkDebugMarkerSetObjectNameEXT");
	vkCmdDebugMarkerBegin = (PFN_vkCmdDebugMarkerBeginEXT)vkGetDeviceProcAddr(i_device, "vkCmdDebugMarkerBeginEXT");
	vkCmdDebugMarkerEnd = (PFN_vkCmdDebugMarkerEndEXT)vkGetDeviceProcAddr(i_device, "vkCmdDebugMarkerEndEXT");
	vkCmdDebugMarkerInsert = (PFN_vkCmdDebugMarkerInsertEXT)vkGetDeviceProcAddr(i_device, "vkCmdDebugMarkerInsertEXT");

	//is_active = (vkDebugMarkerSetObjectName != VK_NULL_HANDLE);
#endif
}




IFNITY_END_NAMESPACE


