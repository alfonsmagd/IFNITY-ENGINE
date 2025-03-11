#pragma once

#include "Ifnity/Graphics/ifrhi.h"
#include <vulkan/vulkan.h>
#include "Ifnity\Graphics\Interfaces\IBuffer.hpp"
#include "Ifnity\Graphics\Interfaces\IGraphicsPipeline.hpp"

IFNITY_NAMESPACE

namespace Vulkan
{


	//================================================================================================
	//ENUMS
	//================================================================================================



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


	/**
	* @brief Structure mapping RHI format to VK format.
	*/
	struct FormatMapping
	{
		rhi::Format rhiFormat;  /**< RHI format. */
		VkFormat vkFormat;      /**< Corresponding Vulkan format. */
	};

	static const std::array<FormatMapping, size_t(rhi::Format::COUNT)> c_FormatMap = { {
	{ rhi::Format::R8G8B8_UINT, VK_FORMAT_R8G8B8_UINT },
	{ rhi::Format::R8G8B8, VK_FORMAT_R8G8B8_UNORM },
	{ rhi::Format::R8G8B8A8, VK_FORMAT_R8G8B8A8_UNORM },
	{ rhi::Format::R32G32B32_FLOAT, VK_FORMAT_R32G32B32_SFLOAT },
	{ rhi::Format::R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM },
	{ rhi::Format::B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM },
	{ rhi::Format::R_UNORM8, VK_FORMAT_R8_UNORM },
	{ rhi::Format::R_UINT16, VK_FORMAT_R16_UINT },
	{ rhi::Format::R_UINT32, VK_FORMAT_R32_UINT },
	{ rhi::Format::R_UNORM16, VK_FORMAT_R16_UNORM },
	{ rhi::Format::R_FLOAT16, VK_FORMAT_R16_SFLOAT },
	{ rhi::Format::R_FLOAT32, VK_FORMAT_R32_SFLOAT },
	{ rhi::Format::RG_UNORM8, VK_FORMAT_R8G8_UNORM },
	{ rhi::Format::RG_UINT16, VK_FORMAT_R16G16_UINT },
	{ rhi::Format::RG_UINT32, VK_FORMAT_R32G32_UINT },
	{ rhi::Format::RG_UNORM16, VK_FORMAT_R16G16_UNORM },
	{ rhi::Format::RG_FLOAT16, VK_FORMAT_R16G16_SFLOAT },
	{ rhi::Format::RG_FLOAT32, VK_FORMAT_R32G32_SFLOAT },
	{ rhi::Format::RGBA_UNORM8, VK_FORMAT_R8G8B8A8_UNORM },
	{ rhi::Format::RGBA_UINT32, VK_FORMAT_R32G32B32A32_UINT },
	{ rhi::Format::RGBA_FLOAT16, VK_FORMAT_R16G16B16A16_SFLOAT },
	{ rhi::Format::RGBA_FLOAT32, VK_FORMAT_R32G32B32A32_SFLOAT },
	{ rhi::Format::RGBA_SRGB8, VK_FORMAT_R8G8B8A8_SRGB },
	{ rhi::Format::BGRA_UNORM8, VK_FORMAT_B8G8R8A8_UNORM },
	{ rhi::Format::BGRA_SRGB8, VK_FORMAT_B8G8R8A8_SRGB },
	{ rhi::Format::ETC2_RGB8, VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK },
	{ rhi::Format::ETC2_SRGB8, VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK },
	{ rhi::Format::BC7_RGBA, VK_FORMAT_BC7_UNORM_BLOCK },
	{ rhi::Format::Z_UNORM16, VK_FORMAT_D16_UNORM },
	{ rhi::Format::Z_UNORM24, VK_FORMAT_X8_D24_UNORM_PACK32 },
	{ rhi::Format::Z_FLOAT32, VK_FORMAT_D32_SFLOAT },
	{ rhi::Format::Z_UNORM24_S_UINT8, VK_FORMAT_D24_UNORM_S8_UINT },
	{ rhi::Format::Z_FLOAT32_S_UINT8, VK_FORMAT_D32_SFLOAT_S8_UINT }
} };


	/**
	* @brief Get the RHI format corresponding to the given Vulkan format.
	* @param vkFormat The Vulkan format.
	* @return The corresponding RHI format, or rhi::Format::UNKNOWN if not found.
	*/
	inline rhi::Format GetRHIFormat(VkFormat vkFormat)
	{
		for(const auto& mapping : c_FormatMap)
		{
			if(mapping.vkFormat == vkFormat)
			{
				return mapping.rhiFormat;
			}
		}
		return rhi::Format::UNKNOWN; // Return UNKNOWN if the format is not found
	}

	inline VkFormat   formatToVkFormat(rhi::Format format)
	{

		VkFormat result = format < rhi::Format::COUNT ?  
													 Vulkan::c_FormatMap[ (uint8_t)format ].vkFormat : 
													 VK_FORMAT_UNDEFINED;
		return result;
	}


	 /**
	  * @brief Converts blend factor to Vulkan blend factor.
	  *
	  * @param factor The blend factor.
	  * @return The corresponding Vulkan blend factor.
	  */
	inline VkBlendFactor ConvertToVkBlendFactor(rhi::BlendFactor factor)
	{
		switch(factor)
		{
		case rhi::BlendFactor::ZERO:
			return VK_BLEND_FACTOR_ZERO;
		case rhi::BlendFactor::ONE:
			return VK_BLEND_FACTOR_ONE;
		case rhi::BlendFactor::SRC_COLOR:
			return VK_BLEND_FACTOR_SRC_COLOR;
		case rhi::BlendFactor::ONE_MINUS_SRC_COLOR:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case rhi::BlendFactor::SRC_ALPHA:
			return VK_BLEND_FACTOR_SRC_ALPHA;
		case rhi::BlendFactor::ONE_MINUS_SRC_ALPHA:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		default:
			return VK_BLEND_FACTOR_ONE; // Default value
		}
	}

	inline VkPrimitiveTopology ConvertToVkPrimitiveTopology(rhi::PrimitiveType type)
	{
		switch(type)
		{
		case rhi::PrimitiveType::TriangleList:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case rhi::PrimitiveType::TriangleStrip:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case rhi::PrimitiveType::LineList:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case rhi::PrimitiveType::PointList:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		default:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Default value
		}
	}

	inline VkPolygonMode ConverToVkPolygonMode(rhi::PolygonModeType mode)
	{
		switch(mode)
		{
		case rhi::PolygonModeType::Fill:
			return VK_POLYGON_MODE_FILL;
		case rhi::PolygonModeType::Line:
			return VK_POLYGON_MODE_LINE;
		default:
			return VK_POLYGON_MODE_FILL; // Default value
		}
	}

	inline VkStencilOp ConvertstencilOpToVkStencilOp(rhi::StencilOp op)
	{
		switch(op)
		{
		case rhi::StencilOp::StencilOp_Keep:
			return VK_STENCIL_OP_KEEP;
		case rhi::StencilOp::StencilOp_Zero:
			return VK_STENCIL_OP_ZERO;
		case rhi::StencilOp::StencilOp_Replace:
			return VK_STENCIL_OP_REPLACE;
		case rhi::StencilOp::StencilOp_IncrementClamp:
			return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		case rhi::StencilOp::StencilOp_DecrementClamp:
			return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		case rhi::StencilOp::StencilOp_Invert:
			return VK_STENCIL_OP_INVERT;
		case rhi::StencilOp::StencilOp_IncrementWrap:
			return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		case rhi::StencilOp::StencilOp_DecrementWrap:
			return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		}
		assert(false);
		return VK_STENCIL_OP_KEEP;
	}

	inline VkCompareOp compareOpToVkCompareOp(rhi::CompareOp func)
	{
		switch(func)
		{
		case rhi::CompareOp::CompareOp_Never:
			return VK_COMPARE_OP_NEVER;
		case rhi::CompareOp::CompareOp_Less:
			return VK_COMPARE_OP_LESS;
		case rhi::CompareOp::CompareOp_Equal:
			return VK_COMPARE_OP_EQUAL;
		case rhi::CompareOp::CompareOp_LessEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case rhi::CompareOp::CompareOp_Greater:
			return VK_COMPARE_OP_GREATER;
		case rhi::CompareOp::CompareOp_NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;
		case rhi::CompareOp::CompareOp_GreaterEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case rhi::CompareOp::CompareOp_AlwaysPass:
			return VK_COMPARE_OP_ALWAYS;
		}
		
		return VK_COMPARE_OP_ALWAYS;
	}


	inline VkCullModeFlags cullModeToVkCullMode(rhi::CullModeType mode)
	{
		switch(mode)
		{
		case rhi::CullModeType::None:
			return VK_CULL_MODE_NONE;
		case rhi::CullModeType::Front:
			return VK_CULL_MODE_FRONT_BIT;
		case rhi::CullModeType::Back:
			return VK_CULL_MODE_BACK_BIT;
		}
		return VK_CULL_MODE_NONE;
	}

	inline VkFrontFace windingModeToVkFrontFace(rhi::FrontFaceType mode)
	{
		switch(mode)
		{
		case rhi::FrontFaceType::Clockwise:
			return VK_FRONT_FACE_CLOCKWISE;
		case rhi::FrontFaceType::CounterClockwise:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	}


	/**
	 * @brief Converts blend operation to Vulkan blend factor.
	 *
	 * @param factor The blend factor.
	 * @return The corresponding Vulkan blend factor.
	 */
	inline VkBlendOp  ConvertToVkBlendOp(rhi::BlendFactor factor)
	{
		switch(factor)
		{
		case rhi::BlendFactor::OPERATION_ADD:
			return VK_BLEND_OP_ADD;
		default:
			return VK_BLEND_OP_ADD; // Default value
		}
	}



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

	inline VkIndexType indexFormatToVkIndexType(rhi::IndexFormat format)
	{
		switch(format)
		{
		case rhi::IndexFormat::IndexFormat_UINT8:
			return VK_INDEX_TYPE_UINT8_EXT;
		case rhi::IndexFormat::IndexFormat_UINT16:
			return VK_INDEX_TYPE_UINT16;
		case rhi::IndexFormat::IndexFormat_UINT32:
			return VK_INDEX_TYPE_UINT32;
		}

		return VK_INDEX_TYPE_UINT16;

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


    inline std::vector<VkFormat> getCompatibleDepthStencilFormats(rhi::Format format)  
    {  
       switch(format)  
       {  
       case rhi::Format::Z_UNORM16:  
           return { VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT };  
       case rhi::Format::Z_UNORM24:  
           return { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM_S8_UINT };  
       case rhi::Format::Z_FLOAT32:  
           return { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };  
       case rhi::Format::Z_UNORM24_S_UINT8:  
           return { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT };  
       case rhi::Format::Z_FLOAT32_S_UINT8:  
           return { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT };  
       default:  
           return { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT };  
       }  
       return { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT };  
    }






};




IFNITY_END_NAMESPACE