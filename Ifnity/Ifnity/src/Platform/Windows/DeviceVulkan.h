#pragma once 

#include "UtilsVulkan.h"
#include "Ifnity/GraphicsDeviceManager.hpp"




IFNITY_NAMESPACE

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
	std::vector<VkCommandBuffer> m_CommandBuffers;

	// Sync objects
	VkSemaphore m_PresentSemaphore = VK_NULL_HANDLE;
	VkSemaphore m_RenderSemaphore = VK_NULL_HANDLE;
	VkFence rdRenderFence = VK_NULL_HANDLE;

	//Auxiliary 
	uint32_t m_imageIndex = 0;
	uint32_t m_commandBufferIndex = 0;
	uint32_t m_commandBufferCount = 0;

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
};


IFNITY_END_NAMESPACE