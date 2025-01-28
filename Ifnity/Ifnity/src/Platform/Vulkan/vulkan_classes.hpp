#pragma once

//This Vulkan Classes are based on lightweight vulkan https://github.com/corporateshark/lightweightvk

#include <pch.h>
#include <VkBootstrap.h>
#include "vk_mem_alloc.h"





IFNITY_NAMESPACE

class DeviceVulkan;
struct ViewPortState;
struct ScissorRect;

namespace Vulkan
{

	//================================================================================================
	//ENUMS
	//================================================================================================

	enum class VertexFormat: uint8_t
	{
		Invalid = 0,
		Float1,
		Float2,
		Float3,
		Float4,

		Byte1,
		Byte2,
		Byte3,
		Byte4,
		UByte1,
		UByte2,
		UByte3,
		UByte4,

		Short1,
		Short2,
		Short3,
		Short4,

		UShort1,
		UShort2,
		UShort3,
		UShort4,

		Byte2Norm,
		Byte4Norm,

		UByte2Norm,
		UByte4Norm,

		Short2Norm,
		Short4Norm,

		UShort2Norm,
		UShort4Norm,

		Int1,
		Int2,
		Int3,
		Int4,

		UInt1,
		UInt2,
		UInt3,
		UInt4,

		HalfFloat1,
		HalfFloat2,
		HalfFloat3,
		HalfFloat4,

		Int_2_10_10_10_REV,
	};

	enum { MAX_COLOR_ATTACHMENTS = 8 };

	enum ShaderStage: uint8_t
	{
		Stage_Vert,
		Stage_Tesc,
		Stage_Tese,
		Stage_Geom,
		Stage_Frag,
		Stage_Comp,
		Stage_Task,
		Stage_Mesh,
		// ray tracing
		Stage_RayGen,
		Stage_AnyHit,
		Stage_ClosestHit,
		Stage_Miss,
		Stage_Intersection,
		Stage_Callable,
	};

	union ClearColorValue
	{
		float float32[ 4 ];
		int32_t int32[ 4 ];
		uint32_t uint32[ 4 ];
	};

	//-----------------------------------------------//
	// STRUCTS
	//-----------------------------------------------//
	struct ShaderModuleState final
	{
		VkShaderModule sm = VK_NULL_HANDLE;
		uint32_t pushConstantsSize = 0;
	};


	// Structure to hold a single specialization constant entry
	struct SpecializationConstantEntry
	{
		uint32_t constantId = 0; // ID of the specialization constant
		uint32_t offset = 0; // Offset within ShaderSpecializationConstantDesc::data
		size_t size = 0; // Size of the specialization constant
	};

	// Structure to hold a description of specialization constants
	struct SpecializationConstantDesc
	{
		enum { SPECIALIZATION_CONSTANTS_MAX = 16 }; // Maximum number of specialization constants
		SpecializationConstantEntry entries[ SPECIALIZATION_CONSTANTS_MAX ] = {}; // Array of specialization constant entries
		const void* data = nullptr; // Pointer to the data of specialization constants
		size_t dataSize = 0; // Size of the data

		// Function to get the number of specialization constants
		uint32_t getNumSpecializationConstants() const
		{
			uint32_t n = 0;
			while(n < SPECIALIZATION_CONSTANTS_MAX && entries[ n ].size)
			{
				n++;
			}
			return n;
		}
	};

	struct VertexInput final
	{
		enum { VERTEX_ATTRIBUTES_MAX = 16 };
		enum { VERTEX_BUFFER_MAX = 16 };
		struct VertexAttribute final
		{
			uint32_t location = 0; // a buffer which contains this attribute stream
			uint32_t binding = 0;
			VertexFormat format = VertexFormat::Invalid; // per-element format
			uintptr_t offset = 0; // an offset where the first element of this attribute stream starts
		} attributes[ VERTEX_ATTRIBUTES_MAX ];
		struct VertexInputBinding final
		{
			uint32_t stride = 0;
		} inputBindings[ VERTEX_BUFFER_MAX ];

		uint32_t getNumAttributes() const
		{
			uint32_t n = 0;
			while(n < VERTEX_ATTRIBUTES_MAX && attributes[ n ].format != VertexFormat::Invalid)
			{
				n++;
			}
			return n;
		}
		uint32_t getNumInputBindings() const
		{
			uint32_t n = 0;
			while(n < VERTEX_BUFFER_MAX && inputBindings[ n ].stride)
			{
				n++;
			}
			return n;
		}

		bool operator==(const VertexInput& other) const
		{
			return memcmp(this, &other, sizeof(VertexInput)) == 0;
		}
	};

	struct RenderPipelineState final
	{
		uint32_t numBindings_ = 0;
		uint32_t numAttributes_ = 0;
		VkVertexInputBindingDescription vkBindings_[ VertexInput::VERTEX_BUFFER_MAX ] = {};
		VkVertexInputAttributeDescription vkAttributes_[ VertexInput::VERTEX_ATTRIBUTES_MAX ] = {};

		// non-owning, the last seen VkDescriptorSetLayout from VulkanContext::vkDSL_ (if the context has a new layout, invalidate all VkPipeline objects)
		VkDescriptorSetLayout lastVkDescriptorSetLayout_ = VK_NULL_HANDLE;

		VkShaderStageFlags shaderStageFlags_ = 0;
		VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
		VkPipeline pipeline_ = VK_NULL_HANDLE;

		void* specConstantDataStorage_ = nullptr;
	};

	struct DeviceQueues final
	{
		const static uint32_t INVALID = 0xFFFFFFFF;
		uint32_t graphicsQueueFamilyIndex = INVALID;
		uint32_t computeQueueFamilyIndex = INVALID;
		uint32_t presentQueueFamilyIndex = INVALID;

		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue computeQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
	};


	//Get struct 
	struct SubmitHandle
	{
		uint32_t bufferIndex_ = 0; // Index buffer
		uint32_t submitId_ = 0;	// Submit ID
		SubmitHandle() = default;
		explicit SubmitHandle(uint64_t handle): bufferIndex_(uint32_t(handle & 0xffffffff)),
			submitId_(uint32_t(handle >> 32))
		{
			assert(submitId_);
		}
		bool empty() const
		{
			return submitId_ == 0;
		}
		//Return handle in 64 bit with information about buffer index and submit ID
		uint64_t handle() const
		{
			return (uint64_t(submitId_) << 32) + bufferIndex_;
		}
	};

	//-----------------------------------=============//
	// CLASSES--------------------------==============//
	//-----------------------------------//






	#pragma region CLASSES 

	//-----------------------------------------------//
	// VulkanImmediateCommands 
	//-----------------------------------------------//
	class VulkanImmediateCommands final
	{
	public:
		// the maximum number of command buffers which can similtaneously exist in the system; when we run out of buffers, we stall and wait until
		// an existing buffer becomes available
		static constexpr uint32_t kMaxCommandBuffers = 64;

		VulkanImmediateCommands(VkDevice device, uint32_t queueFamilyIndex, const char* debugName);
		~VulkanImmediateCommands();
		VulkanImmediateCommands(const VulkanImmediateCommands&) = delete;
		VulkanImmediateCommands& operator=(const VulkanImmediateCommands&) = delete;

		struct CommandBufferWrapper
		{
			VkCommandBuffer cmdBuf_ = VK_NULL_HANDLE;
			VkCommandBuffer cmdBufAllocated_ = VK_NULL_HANDLE;
			SubmitHandle handle_ = {};
			VkFence fence_ = VK_NULL_HANDLE;
			VkSemaphore semaphore_ = VK_NULL_HANDLE;
			bool isEncoding_ = false;
		};

		// returns the current command buffer (creates one if it does not exist)
		const CommandBufferWrapper& acquire();
		SubmitHandle submit(const CommandBufferWrapper& wrapper);
		void waitSemaphore(VkSemaphore semaphore);
		VkSemaphore acquireLastSubmitSemaphore();
		VkFence getVkFence(SubmitHandle handle) const;
		SubmitHandle getLastSubmitHandle() const;
		SubmitHandle getNextSubmitHandle() const;
		bool isReady(SubmitHandle handle, bool fastCheckNoVulkan = false) const;
		void wait(SubmitHandle handle);
		void waitAll();

	private:
		void purge();

	private:
		VkDevice device_ = VK_NULL_HANDLE;
		VkQueue queue_ = VK_NULL_HANDLE;
		VkCommandPool commandPool_ = VK_NULL_HANDLE;
		uint32_t queueFamilyIndex_ = 0;
		const char* debugName_ = "";
		CommandBufferWrapper buffers_[ kMaxCommandBuffers ];
		SubmitHandle lastSubmitHandle_ = SubmitHandle();
		SubmitHandle nextSubmitHandle_ = SubmitHandle();
		VkSemaphore lastSubmitSemaphore_ = VK_NULL_HANDLE;
		VkSemaphore waitSemaphore_ = VK_NULL_HANDLE;
		uint32_t numAvailableCommandBuffers_ = kMaxCommandBuffers;
		uint32_t submitCounter_ = 1;
	};

	//-----------------------------------------------//
	// CommandBuffer
	//-----------------------------------------------//
	class CommandBuffer final
	{
	public:
		CommandBuffer() = default;
		explicit CommandBuffer(DeviceVulkan* ctx);
		~CommandBuffer();

		CommandBuffer& operator=(CommandBuffer&& other) = default;

		void cmdBindViewport(const ViewPortState& state);
		void cmdBindScissorRect(const ScissorRect& rect);


	private:
		friend class DeviceVulkan;
		DeviceVulkan* ctx_ = nullptr ;
		const VulkanImmediateCommands::CommandBufferWrapper* wrapper_ = nullptr;

		SubmitHandle lastSubmitHandle_ = {};

		VkPipeline lastPipelineBound_ = VK_NULL_HANDLE;

		bool isRendering_ = false;
	};


	//-----------------------------------------------//
	// VulkanImage class.
	//-----------------------------------------------//
	struct VulkanImage final
	{
		// clang-format off
		[[nodiscard]] inline bool isSampledImage() const { return (vkUsageFlags_ & VK_IMAGE_USAGE_SAMPLED_BIT) > 0; }
		[[nodiscard]] inline bool isStorageImage() const { return (vkUsageFlags_ & VK_IMAGE_USAGE_STORAGE_BIT) > 0; }
		[[nodiscard]] inline bool isColorAttachment() const { return (vkUsageFlags_ & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) > 0; }
		[[nodiscard]] inline bool isDepthAttachment() const { return (vkUsageFlags_ & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) > 0; }
		[[nodiscard]] inline bool isAttachment() const { return (vkUsageFlags_ & (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) > 0; }
		// clang-format on

		/*
		 * Setting `numLevels` to a non-zero value will override `mipLevels_` value from the original Vulkan image, and can be used to create
		 * image views with different number of levels.
		 */
		[[nodiscard]] VkImageView createImageView(VkDevice device,
			VkImageViewType type,
			VkFormat format,
			VkImageAspectFlags aspectMask,
			uint32_t baseLevel,
			uint32_t numLevels = VK_REMAINING_MIP_LEVELS,
			uint32_t baseLayer = 0,
			uint32_t numLayers = 1,
			const VkComponentMapping mapping = { .r = VK_COMPONENT_SWIZZLE_IDENTITY,
															.g = VK_COMPONENT_SWIZZLE_IDENTITY,
															.b = VK_COMPONENT_SWIZZLE_IDENTITY,
															.a = VK_COMPONENT_SWIZZLE_IDENTITY },
			const VkSamplerYcbcrConversionInfo* ycbcr = nullptr,
			const char* debugName = nullptr) const;

		//void generateMipmap(VkCommandBuffer commandBuffer) const;
		void transitionLayout(VkCommandBuffer commandBuffer,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			const VkImageSubresourceRange& subresourceRange) const;

		//[[nodiscard]] VkImageAspectFlags getImageAspectFlags() const;

		// framebuffers can render only into one level/layer

		[[nodiscard]] static bool isDepthFormat(VkFormat format);
		[[nodiscard]] static bool isStencilFormat(VkFormat format);

	public:
		VkImage vkImage_ = VK_NULL_HANDLE;
		VkImageUsageFlags vkUsageFlags_ = 0;
		VkDeviceMemory vkMemory_[ 3 ] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };
		VmaAllocation vmaAllocation_ = VK_NULL_HANDLE;
		VkFormatProperties vkFormatProperties_ = {};
		VkExtent3D vkExtent_ = { 0, 0, 0 };
		VkImageType vkType_ = VK_IMAGE_TYPE_MAX_ENUM;
		VkFormat vkImageFormat_ = VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits vkSamples_ = VK_SAMPLE_COUNT_1_BIT;
		void* mappedPtr_ = nullptr;
		bool isSwapchainImage_ = false;
		bool isOwningVkImage_ = true;
		uint32_t numLevels_ = 1u;
		uint32_t numLayers_ = 1u;
		bool isDepthFormat_ = false;
		bool isStencilFormat_ = false;
		// current image layout
		mutable VkImageLayout vkImageLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
		// precached image views - owned by this VulkanImage
		VkImageView imageView_ = VK_NULL_HANDLE; // default view with all mip-levels
		VkImageView imageViewStorage_ = VK_NULL_HANDLE; // default view with identity swizzle (all mip-levels)

	};

	//-------------------------------------------------------------------------------------------------------//
	// SwapChain class.  
	//-------------------------------------------------------------------------------------------------------//

	class VulkanSwapchain final
	{
		enum { LVK_MAX_SWAPCHAIN_IMAGES = 16 };

	public:
		VulkanSwapchain(DeviceVulkan& ctx, uint32_t width, uint32_t height);
		~VulkanSwapchain();


		VkResult present(VkSemaphore waitSemaphore);
		VulkanImage getCurrentTexture();
		/*
		VkImage getCurrentVkImage() const;
		VkImageView getCurrentVkImageView() const;

		const VkSurfaceFormatKHR& getSurfaceFormat() const;
		uint32_t getNumSwapchainImages() const;*/
		uint32_t getCurrentImageIndex() const;
		const VkSurfaceFormatKHR& getSurfaceFormat() const;
	private:
		DeviceVulkan& ctx_;
		VkDevice device_ = VK_NULL_HANDLE;
		VkQueue graphicsQueue_ = VK_NULL_HANDLE;
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		uint32_t numSwapchainImages_ = 0;
		uint32_t currentImageIndex_ = 0;
		bool getNextImage_ = true;
		VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
		VkSurfaceFormatKHR surfaceFormat_ = { .format = VK_FORMAT_UNDEFINED };
		VulkanImage swapchainTextures_[ LVK_MAX_SWAPCHAIN_IMAGES ] = {};
		VkSemaphore acquireSemaphore_ = VK_NULL_HANDLE;
		VkFence acquireFence_ = VK_NULL_HANDLE;
	};


	//-------------------------------------------------------------------------------------------------------//
	// VULKAN PIPELINE BUILDER						//
	//-------------------------------------------------------------------------------------------------------//

	class VulkanPipelineBuilder final
	{
	public:
		VulkanPipelineBuilder();
		~VulkanPipelineBuilder() = default;

		VulkanPipelineBuilder& dynamicState(VkDynamicState state);
		VulkanPipelineBuilder& primitiveTopology(VkPrimitiveTopology topology);
		VulkanPipelineBuilder& rasterizationSamples(VkSampleCountFlagBits samples, float minSampleShading);
		VulkanPipelineBuilder& shaderStage(VkPipelineShaderStageCreateInfo stage);
		VulkanPipelineBuilder& stencilStateOps(VkStencilFaceFlags faceMask,
			VkStencilOp failOp,
			VkStencilOp passOp,
			VkStencilOp depthFailOp,
			VkCompareOp compareOp);
		VulkanPipelineBuilder& stencilMasks(VkStencilFaceFlags faceMask, uint32_t compareMask, uint32_t writeMask, uint32_t reference);
		VulkanPipelineBuilder& cullMode(VkCullModeFlags mode);
		VulkanPipelineBuilder& frontFace(VkFrontFace mode);
		VulkanPipelineBuilder& polygonMode(VkPolygonMode mode);
		VulkanPipelineBuilder& vertexInputState(const VkPipelineVertexInputStateCreateInfo& state);
		VulkanPipelineBuilder& colorAttachments(const VkPipelineColorBlendAttachmentState* states,
			const VkFormat* formats,
			uint32_t numColorAttachments);
		VulkanPipelineBuilder& depthAttachmentFormat(VkFormat format);
		VulkanPipelineBuilder& stencilAttachmentFormat(VkFormat format);
		VulkanPipelineBuilder& patchControlPoints(uint32_t numPoints);

		VkResult build(VkDevice device,
			VkPipelineCache pipelineCache,
			VkPipelineLayout pipelineLayout,
			VkPipeline* outPipeline,
			const char* debugName = nullptr) noexcept;

		static uint32_t getNumPipelinesCreated()
		{
			return numPipelinesCreated_;
		}

	private:
		enum { LVK_MAX_DYNAMIC_STATES = 128 };
		uint32_t numDynamicStates_ = 0;
		VkDynamicState dynamicStates_[ LVK_MAX_DYNAMIC_STATES ] = {};

		uint32_t numShaderStages_ = 0;
		VkPipelineShaderStageCreateInfo shaderStages_[ Stage_Frag + 1 ] = {};

		VkPipelineVertexInputStateCreateInfo vertexInputState_;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly_;
		VkPipelineRasterizationStateCreateInfo rasterizationState_;
		VkPipelineMultisampleStateCreateInfo multisampleState_;
		VkPipelineDepthStencilStateCreateInfo depthStencilState_;
		VkPipelineTessellationStateCreateInfo tessellationState_;

		uint32_t numColorAttachments_ = 0;
		VkPipelineColorBlendAttachmentState colorBlendAttachmentStates_[MAX_COLOR_ATTACHMENTS ] = {};
		VkFormat colorAttachmentFormats_[MAX_COLOR_ATTACHMENTS ] = {};

		VkFormat depthAttachmentFormat_ = VK_FORMAT_UNDEFINED;
		VkFormat stencilAttachmentFormat_ = VK_FORMAT_UNDEFINED;

		static uint32_t numPipelinesCreated_;

	
	};



	#pragma endregion CLASSES
}

IFNITY_END_NAMESPACE