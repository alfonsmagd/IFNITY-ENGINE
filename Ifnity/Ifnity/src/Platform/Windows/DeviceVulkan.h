#pragma once 

#include "pch.h"
#include "Ifnity/GraphicsDeviceManager.hpp"
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>



IFNITY_NAMESPACE

class DeviceVulkan final : public GraphicsDeviceManager
{
private:

	vkb::Instance m_Instance;  // Vulkan instance 
	VmaAllocator  m_Allocator; // Vulkan memory allocator
	VkSurfaceKHR  m_Surface;   // Vulkan surface

	vkb::Device   m_Device; // Vulkan device bootstrapper
	
	vkb::PhysicalDevice m_PhysicalDevice; // Vulkan physical device bootstrapper. 


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

};


IFNITY_END_NAMESPACE