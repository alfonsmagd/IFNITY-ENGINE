#pragma once 


#include "Ifnity/GraphicsDeviceManager.hpp"
#include  "Platform/ImguiRender/ImguiVulkanRender.h"
#include "vk_mem_alloc.h"
#include <VkBootstrap.h>
#include "Platform\Vulkan\vulkan_classes.hpp"

IFNITY_NAMESPACE

// Data

//Rembeber that vkb its a namespace that contains the Vulkan Bootstrapper.
//Vulkan:: its a namespace that contains the Vulkan Classes dessigned by IFNITY.

class DeviceVulkan final : public GraphicsDeviceManager
{

public:
	Vulkan::CommandBuffer currentCommandBuffer_;
	std::unique_ptr<Vulkan::VulkanImmediateCommands> immediate_;
	vkb::Device    device_; // Vulkan device bootstrapper
	vkb::Swapchain swapchainBootStraap_; // Vulkan swapchain
	std::unique_ptr<Vulkan::VulkanSwapchain> swapchain_;

public:
	Vulkan::DeviceQueues deviceQueues_;


private:
	vkb::Instance  m_Instance;  // Vulkan instance 
	VmaAllocator   m_Allocator; // Vulkan memory allocator
	VkSurfaceKHR   m_Surface;   // Vulkan surface

	
	
	vkb::PhysicalDevice m_PhysicalDevice; // Vulkan physical device bootstrapper. 

	VkCommandPool m_CommandPool = VK_NULL_HANDLE;



	// Depth buffer
	//ImageBlock m_DepthBuffer;
	
	// Render pass
	VkRenderPass m_RenderPass = VK_NULL_HANDLE;

	// Frame buffer and Image, ImageViews
	std::vector<VkImage>  m_SwapchainImages;
	std::vector<VkImageView> m_SwapchainImageViews;
	std::vector<VkFramebuffer> m_Framebuffers;

	// Command buffers
	std::vector<VkCommandBuffer> m_CommandBuffers;

	// Sync objects
	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence>     m_InFlightFences;

	// Sync objects
	VkSemaphore m_PresentSemaphore = VK_NULL_HANDLE;
	VkSemaphore m_RenderSemaphore = VK_NULL_HANDLE;
	VkFence rdRenderFence = VK_NULL_HANDLE;
	uint32_t m_CurrentFrame = 0;

	//Auxiliary 
	uint32_t m_imageIndex = 0;
	uint32_t m_commandBufferIndex = 0;
	uint32_t m_commandBufferCount = 0;

	//Descritpor Pool Imgui 
	VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;

	//DebugUtils
	VkDebugUtilsMessengerEXT            debugUtilsMessenger;
	bool m_DebugUtilsSupported = false;
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{ nullptr };
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{ nullptr };
	PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT{ nullptr };
	PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT{ nullptr };
	PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT{ nullptr };
	PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT{ nullptr };
	PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT{ nullptr };
	PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT{ nullptr };
	PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{ nullptr };

	//TEST Members to get functionality to move a Device class. 
	

	//Vulkan::CommandBuffer m_CommandBuffer;

	float m_Color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

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

	void InternalPreDestroy() override;

	void ClearBackBuffer(float* color) override;

private:

	~DeviceVulkan() override;
	
	// Initialize private methods
	bool CreateSurface();
	bool CreatePhysicalDevice();
	bool CreateDevice();
	bool CreateVmaAllocator();
	bool GetQueue();
	bool CreateSwapChain();
	bool CreateCommandPool();
	bool CreateDepthBuffer();
	bool CreateRenderPass();
	bool CreateFrameBuffer();
	bool CreateCommandBuffers();
	bool CreateSyncObjects();

	//Destroy private methods
	bool DestroyCommandPool();
	void DestroySyncObjects();
	void DestroyCommandBuffers();
	void CleanFrameBuffers();
	void DestroyRenderPass();
	void DestroyImmediateCommands();
	
	//OnRender private methods
	bool AcquireNextImage();
	bool PopulateCommandBuffer();
	bool SubmitCommandBuffer();
	bool PresentImage();
	bool InitGui();

	//Imgui private methods
	bool CreateImGuiDescriptorPool();
	void setupCallbacks(VkDevice& i_device);
	void BeginRenderDocTrace(VkCommandBuffer commandBuffer, const char* markerName, float color[4]);

	//TestMethods to move a Device class 
	bool createVulkanImmediateCommands();
	Vulkan::CommandBuffer& acquireCommandBuffer();
	Vulkan::VulkanImage getCurrentTexture();
	bool hasSwapchain() const noexcept;


	void EndRenderDocTrace(VkCommandBuffer commandBuffer);

};






IFNITY_END_NAMESPACE