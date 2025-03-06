#pragma once

//This Vulkan Classes are based on lightweight vulkan https://github.com/corporateshark/lightweightvk

#include <pch.h>
#include <VkBootstrap.h>
#include "vk_mem_alloc.h"
#include "../Windows/vk_constans.hpp"
#include "Ifnity/Utils/SlotMap.hpp"
#include "Ifnity\Graphics\Interfaces\IBuffer.hpp"




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
	enum { MAX_MIP_LEVELS = 16 };

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
	// TO MOVE IN FILE TODO STRUCTS 
	//-----------------------------------------------//

	enum LoadOp: uint8_t
	{
		LoadOp_Invalid = 0,
		LoadOp_DontCare,
		LoadOp_Load,
		LoadOp_Clear,
		LoadOp_None,
	};

	enum StoreOp: uint8_t
	{
		StoreOp_DontCare = 0,
		StoreOp_Store,
		StoreOp_MsaaResolve,
		StoreOp_None,
	};



	//-----------------------------------------------//
	// STRUCTS
	//-----------------------------------------------//

	struct UsageMapping { BufferType usageBit; VkBufferUsageFlags vkFlag; };

	constexpr UsageMapping usageMappings[] = {
		{ BufferType::INDEX_BUFFER, VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
		{ BufferType::VERTEX_BUFFER, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT },
		{ BufferType::DEFAULT_BUFFER, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR },
		{ BufferType::STORAGE_BUFFER, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR },
		{ BufferType::INDIRECT_BUFFER, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR },

	};





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




	//-----------------------------------------------//
	// TO MOVE IN FILE TODO STRUCTS 
	//-----------------------------------------------//



	inline VkAttachmentLoadOp loadOpToVkAttachmentLoadOp(LoadOp a)
	{
		switch(a)
		{
		case LoadOp_Invalid:
			_ASSERT(false);
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		case LoadOp_DontCare:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		case LoadOp_Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		case LoadOp_Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case LoadOp_None:
			return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
		}
		_ASSERT(false);
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}

	inline VkAttachmentStoreOp storeOpToVkAttachmentStoreOp(StoreOp a)
	{
		switch(a)
		{
		case StoreOp_DontCare:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		case StoreOp_Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
		case StoreOp_MsaaResolve:
			// for MSAA resolve, we have to store data into a special "resolve" attachment
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		case StoreOp_None:
			return VK_ATTACHMENT_STORE_OP_NONE;
		}
		_ASSERT(false);
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}

	constexpr inline VkBufferUsageFlags getVkBufferUsageFlags(BufferType type)
	{
		for(const auto& mapping : usageMappings)
		{
			if(mapping.usageBit == type)
			{
				return mapping.vkFlag;
			}
		}
		return 0;
	}

	inline VkMemoryPropertyFlags storageTypeToVkMemoryPropertyFlags(StorageType storage)
	{
		VkMemoryPropertyFlags memFlags{ 0 };

		switch(storage)
		{
		case StorageType::Device:
			memFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			break;
		case StorageType::HostVisible:
			memFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			break;
		case StorageType::Memoryless:
			memFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
			break;
		}
		return memFlags;
	}

}

IFNITY_END_NAMESPACE