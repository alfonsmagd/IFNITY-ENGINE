#pragma once 

#include "UtilsVulkan.h"
#include "Ifnity/GraphicsDeviceManager.hpp"




IFNITY_NAMESPACE

class DeviceVulkan final : public GraphicsDeviceManager
{
private:

	struct 

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
	bool CreateSurface();
	bool CreatePhysicalDevice();
	bool CreateDevice();
	bool CreateVmaAllocator();
	bool GetQueue();
	bool CreateSwapChain();
	bool CreateCommandPool();
	bool DestroyCommandPool();
	bool CreateDepthBuffer();

};


IFNITY_END_NAMESPACE