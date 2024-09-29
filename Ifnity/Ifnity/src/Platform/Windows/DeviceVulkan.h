#pragma once 

#include "UtilsVulkan.h"
#include "Ifnity/GraphicsDeviceManager.hpp"
#include  "Platform/ImguiRender/ImguiVulkanRender.h"

IFNITY_NAMESPACE

// Data
static VkAllocationCallbacks*   g_Allocator = nullptr;
static VkInstance               g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                 g_Device = VK_NULL_HANDLE;
static uint32_t                 g_QueueFamily = (uint32_t)-1;
static VkQueue                  g_Queue = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static int                      g_MinImageCount = 2;
static bool                     g_SwapChainRebuild = false;



 IFNITY_INLINE VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	// Select prefix depending on flags passed to the callback
	std::string prefix("");

	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		prefix = "VERBOSE: ";
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		prefix = "INFO: ";
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		prefix = "WARNING: ";
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		prefix = "ERROR: ";
	}

	std::cerr << prefix << " validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

 IFNITY_INLINE VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

class DeviceVulkan final : public GraphicsDeviceManager
{
private:


	vkb::Instance  m_Instance;  // Vulkan instance 
	VmaAllocator   m_Allocator; // Vulkan memory allocator
	VkSurfaceKHR   m_Surface;   // Vulkan surface
	vkb::Swapchain m_Swapchain; // Vulkan swapchain
	vkb::Device    m_Device; // Vulkan device bootstrapper
	
	vkb::PhysicalDevice m_PhysicalDevice; // Vulkan physical device bootstrapper. 

	VkCommandPool m_CommandPool = VK_NULL_HANDLE;
	VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
	VkQueue m_PresentQueue  = VK_NULL_HANDLE;


	// Depth buffer
	ImageBlock m_DepthBuffer;
	
	// Render pass
	VkRenderPass m_RenderPass = VK_NULL_HANDLE;

	// Frame buffer and Image, ImageViews
	std::vector<VkImage>  m_SwapchainImages;
	std::vector<VkImageView> m_SwapchainImageViews;
	std::vector<VkFramebuffer> m_Framebuffers;

	// Command buffers
	VkCommandBuffer m_CommandBuffers;

	// Sync objects
	VkSemaphore m_PresentSemaphore = VK_NULL_HANDLE;
	VkSemaphore m_RenderSemaphore = VK_NULL_HANDLE;
	VkFence rdRenderFence = VK_NULL_HANDLE;

	//Auxiliary 
	uint32_t m_imageIndex = 0;
	uint32_t m_commandBufferIndex = 0;
	uint32_t m_commandBufferCount = 0;

	//Descritpor Pool Imgui 
	VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;

	//DebugUtils
	VkDebugUtilsMessengerEXT            debugUtilsMessenger;
#ifdef _DEBUG
	PFN_vkDebugMarkerSetObjectTagEXT  vkDebugMarkerSetObjectTag = VK_NULL_HANDLE;
	PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectName = VK_NULL_HANDLE;
	PFN_vkCmdDebugMarkerBeginEXT      vkCmdDebugMarkerBegin = VK_NULL_HANDLE;
	PFN_vkCmdDebugMarkerEndEXT        vkCmdDebugMarkerEnd = VK_NULL_HANDLE;
	PFN_vkCmdDebugMarkerInsertEXT     vkCmdDebugMarkerInsert = VK_NULL_HANDLE;
	bool                              is_active = false;
#endif

protected:
	// Heredado vía GraphicsDeviceManager
	void OnUpdate() override;

	unsigned int GetWidth() const override;

	unsigned int GetHeight() const override;

	bool InitInternalInstance() override;

	bool InitializeDeviceAndContext() override;

	bool ConfigureSpecificHintsGLFW() const override;

	void SetVSync(bool enabled) override;

	bool IsVSync() const override;

	void ResizeSwapChain() override;

	void InitializeGui() override;

private:

	// Initialize private methods
	bool CreateSurface();
	bool CreatePhysicalDevice();
	bool CreateDevice();
	bool CreateVmaAllocator();
	bool GetQueue();
	bool CreateSwapChain();
	bool CreateCommandPool();
	bool DestroyCommandPool();
	bool CreateDepthBuffer();
	bool CreateRenderPass();
	bool CreateFrameBuffer();
	bool CreateCommandBuffers();
	bool CreateSyncObjects();

	//OnRender private methods
	bool AcquireNextImage();
	bool PopulateCommandBuffer();
	bool SubmitCommandBuffer();
	bool PresentImage();
	bool InitGui();

	//Imgui private methods
	bool CreateImGuiDescriptorPool();


	void setupCallbacks(VkDevice& i_device);

};



IFNITY_END_NAMESPACE