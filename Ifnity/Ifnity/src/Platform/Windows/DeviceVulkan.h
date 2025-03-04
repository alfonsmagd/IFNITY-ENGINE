
#include "Ifnity/GraphicsDeviceManager.hpp"
#include  "Platform/ImguiRender/ImguiVulkanRender.h"
#include "vk_mem_alloc.h"
#include <VkBootstrap.h>
#include "Platform\Vulkan\vulkan_classes.hpp"
#include "vk_backend.hpp"
#include "vk_constans.hpp"

//Vulkan Classes 
#include "Ifnity/Utils/SlotMap.hpp"
#include "../Vulkan/vulkan_SwapChain.hpp"
#include  "../Vulkan/vulkan_CommandBuffer.hpp"




IFNITY_NAMESPACE

// Data

//Rembeber that vkb its a namespace that contains the Vulkan Bootstrapper.
//Vulkan:: its a namespace that contains the Vulkan Classes dessigned by IFNITY.
// no namespace its DeviceContext derived by GraphicsDeviceManager.


class DeviceVulkan final: public GraphicsDeviceManager
{

public:
	IDevice* GetRenderDevice() const override { return m_RenderDevice.get(); }
	VkFormat GetSwapChainFormat() const;
	void bindDefaultDescriptorSets(VkCommandBuffer cmdBuf, VkPipelineBindPoint bindPoint, VkPipelineLayout layout) const;
	const VkPhysicalDeviceLimits& GetPhysicalDeviceLimits() const { return vkPhysicalDeviceProperties2_.properties.limits; }
	const uint32_t getApiVersion() const { return vkPhysicalDeviceProperties2_.properties.apiVersion; }

	//Destroy operations 
	void destroy(Vulkan::TextureHandleSM handle);
	void destroy(Vulkan::GraphicsPipelineHandleSM handle);
	void destroy(Vulkan::ShaderModuleHandleSM handle);



public:
	Vulkan::CommandBuffer currentCommandBuffer_;
	std::unique_ptr<Vulkan::VulkanImmediateCommands> immediate_;
	vkb::Device    device_; // Vulkan device bootstrapper
	vkb::Swapchain swapchainBootStraap_; // Vulkan swapchain
	std::unique_ptr<Vulkan::VulkanSwapchain> swapchain_;
	Vulkan::DeviceQueues deviceQueues_;
	VkPhysicalDeviceProperties2 properties2; //Physical Device Properties 2
	VkPipelineCache pipelineCache_ = VK_NULL_HANDLE;

	//Using instancing. 
	VkDescriptorSetLayout vkDSL_ = VK_NULL_HANDLE; // Descriptor Set Layout
	VkDescriptorPool vkDPool_ = VK_NULL_HANDLE;		// Descriptor Pool	
	VkDescriptorSet vkDSet_ = VK_NULL_HANDLE;		// Descriptor Set
	Vulkan::SubmitHandle lastSubmitHandle_ = Vulkan::SubmitHandle();


	Vulkan::GraphicsPipeline* actualPipeline_ = VK_NULL_HANDLE;

	//SlotMap estructures 
	SlotMap<Vulkan::VulkanImage> slootMapTextures_;
	SlotMap<Vulkan::GraphicsPipeline> slotMapRenderPipelines_;
	SlotMap<Vulkan::ShaderModuleState> slotMapShaderModules_;



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
	uint32_t currentMaxTextures_ = 0;
	uint32_t currentMaxSamplers_ = 0;

	float m_Color[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//DeviceHandle 
	DeviceHandle m_RenderDevice;

	VkPhysicalDeviceDriverProperties vkPhysicalDeviceDriverProperties_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES, nullptr };
	VkPhysicalDeviceVulkan12Properties vkPhysicalDeviceVulkan12Properties_ = {
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES,
		&vkPhysicalDeviceDriverProperties_,
	};
	// provided by Vulkan 1.1
	VkPhysicalDeviceProperties2 vkPhysicalDeviceProperties2_ = {
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
		&vkPhysicalDeviceVulkan12Properties_,
		VkPhysicalDeviceProperties{},
	};


	std::unordered_map<uint32_t, Vulkan::GraphicsPipeline*> map_renderPipelines;
	uint32_t nextPipelineId = 0;


	friend class Vulkan::Device;
	friend class Vulkan::GraphicsPipeline;



private:

	~DeviceVulkan() override;

	//Getter 

	uint32_t getFramebufferMSAABitMask() const;

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
	bool CreatePipelineCache();

	VkResult growDescriptorPool(uint32_t maxTextures, uint32_t maxSamplers);

	void getPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice);
	void createWeaknessDeviceReference();

	//Destroy private methods
	bool DestroyCommandPool();
	bool DestroyShaderStages();
	void DestroySyncObjects();
	void DestroyCommandBuffers();
	void CleanFrameBuffers();
	void DestroyRenderPass();
	void DestroyImmediateCommands();
	void DestroyPipelines();
	void DestroyPipelineCache();

	//OnRender private methods
	bool AcquireNextImage();
	bool PopulateCommandBuffer();
	bool SubmitCommandBuffer();
	bool PresentImage();
	bool InitGui();
	void CheckSpirvVersion(VkPhysicalDevice physicalDevice);


	//Imgui private methods
	bool CreateImGuiDescriptorPool();
	void setupCallbacks(VkDevice& i_device);
	void BeginRenderDocTrace(VkCommandBuffer commandBuffer, const char* markerName, float color[ 4 ]);

	//TestMethods to move a Device class 
	bool createVulkanImmediateCommands();
	Vulkan::CommandBuffer& acquireCommandBuffer();
	Vulkan::TextureHandleSM getCurrentSwapChainTexture();
	bool hasSwapchain() const noexcept;
	Vulkan::SubmitHandle submit(Vulkan::CommandBuffer& commandBuffer, Vulkan::TextureHandleSM present);
	void addGraphicsPipeline(Vulkan::GraphicsPipeline* pipeline);

	//This move to device create s
	Vulkan::ShaderModuleState createShaderModuleFromSpirVconst(const void* spirv,
		size_t numBytes,
		const char* debugName);

	void EndRenderDocTrace(VkCommandBuffer commandBuffer);



};






IFNITY_END_NAMESPACE