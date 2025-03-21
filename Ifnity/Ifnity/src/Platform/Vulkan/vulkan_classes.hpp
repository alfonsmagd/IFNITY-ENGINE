#pragma once

//This Vulkan Classes are based on lightweight vulkan https://github.com/corporateshark/lightweightvk

#include <pch.h>
#include <VkBootstrap.h>
#include "vk_mem_alloc.h"
#include "../Windows/vk_constans.hpp"
#include "vulkan_conversions.hpp"
#include "Ifnity/Utils/SlotMap.hpp"





IFNITY_NAMESPACE

class DeviceVulkan;
struct ViewPortState;
struct ScissorRect;

namespace Vulkan
{
	class Framebuffer;


	using TextureHandleSM = Handle<struct VulkanImage>;
	using GraphicsPipelineHandleSM = Handle<struct GraphicsPipeline>;
	using ShaderModuleHandleSM = Handle< struct ShaderModuleState>;
	using BufferHandleSM = Handle<struct VulkanBuffer>;
	using SamplerHandleSM = Handle<VkSampler>;

	template<typename Handle>
	void destroy(DeviceVulkan* ctx, Handle handle);


	template<typename ImplObjectType>
	using Holder = std::unique_ptr<Handle<ImplObjectType>, std::function<void(Handle<ImplObjectType>*)>>;

	template<typename DeviceVulkan, typename Handle>
	concept HasDestroy = requires(DeviceVulkan * ctx, Handle handle)
	{
		{ destroy(ctx, handle) } -> std::same_as<void>;
	};

	template<typename ImplObjectType, typename DeviceVulkan>
		requires HasDestroy<DeviceVulkan, Handle<ImplObjectType>>
	Holder<ImplObjectType> makeHolder(DeviceVulkan* ctx, Handle<ImplObjectType> handle)
	{
		return Holder<ImplObjectType>(
			std::make_unique<Handle<ImplObjectType>>(handle).release(),
			[ ctx, handle ](Handle<ImplObjectType>* ptr)
			{   // Custom deleter con acceso a ctx
				if(ctx)
				{
					destroy(ctx, handle);
					std::cout << "Destroy object make holder ... \n";
				}
				delete ptr;
			}
		);
	}

	//Holder its a unique pointer that contains a handle and a custom deleter that Vulkan will be destroy , this way we can ensure that the object will be destroyed when the holder is destroyed.
	using HolderShaderSM = Holder<ShaderModuleState>;
	using HolderTextureSM = Holder<VulkanImage>;
	using HolderGraphicsPipelineSM = Holder<GraphicsPipeline>;
	using HolderBufferSM = Holder<VulkanBuffer>;






	struct ShaderModuleState final
	{
		VkShaderModule sm = VK_NULL_HANDLE;
		uint32_t pushConstantsSize = 0;
	};

	struct DepthState final
	{
		rhi::CompareOp compareOp = rhi::CompareOp::CompareOp_AlwaysPass;
		bool isDepthWriteEnabled = false;
	};

	struct VulkanImage;
	struct RenderPass final
	{
		struct AttachmentDesc final
		{
			LoadOp loadOp = LoadOp_Invalid;
			StoreOp storeOp = StoreOp_Store;
			uint8_t layer = 0;
			uint8_t level = 0;
			float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
			float clearDepth = 1.0f;
			uint32_t clearStencil = 0;
		};

		AttachmentDesc color[ MAX_COLOR_ATTACHMENTS ] = {};
		AttachmentDesc depth = { .loadOp = LoadOp_DontCare, .storeOp = StoreOp_DontCare };
		AttachmentDesc stencil = { .loadOp = LoadOp_Invalid, .storeOp = StoreOp_DontCare };

		uint32_t getNumColorAttachments() const
		{
			uint32_t n = 0;
			while(n < MAX_COLOR_ATTACHMENTS && color[ n ].loadOp != LoadOp_Invalid)
			{
				n++;
			}
			return n;
		}
	};

	struct Framebuffer final
	{

		struct AttachmentDesc
		{
			TextureHandleSM texture;
			TextureHandleSM resolveTexture;
		};

		AttachmentDesc color[ MAX_COLOR_ATTACHMENTS ] = {};
		AttachmentDesc depthStencil;

		const char* debugName = "";

		uint32_t getNumColorAttachments() const
		{
			uint32_t n = 0;
			while(n < MAX_COLOR_ATTACHMENTS && color[ n ].texture)
			{
				n++;
			}
			return n;
		}
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
		static constexpr size_t SPECIALIZATION_CONSTANTS_MAX = 16; // Maximum number of specialization constants
		std::array<SpecializationConstantEntry, SPECIALIZATION_CONSTANTS_MAX> entries = {}; // Array of specialization constant entries
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

	

	struct RenderPipelineState final
	{
		uint32_t numBindings_ = 0;
		uint32_t numAttributes_ = 0;
		VkVertexInputBindingDescription vkBindings_[ rhi::VertexInput::VERTEX_BUFFER_MAX ] = {};
		VkVertexInputAttributeDescription vkAttributes_[rhi::VertexInput::VERTEX_ATTRIBUTES_MAX ] = {};

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




	

}

IFNITY_END_NAMESPACE