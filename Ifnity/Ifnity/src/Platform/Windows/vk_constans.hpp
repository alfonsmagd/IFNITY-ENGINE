#pragma once

#include "Ifnity/Graphics/ifrhi.h"
#include <vulkan/vulkan.h>

IFNITY_NAMESPACE

namespace Vulkan
{

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
		{ rhi::Format::R8G8B8,      VK_FORMAT_R8G8B8_UNORM },
		{ rhi::Format::R8G8B8A8,     VK_FORMAT_R8G8B8A8_UNORM },
		{ rhi::Format::R32G32B32_FLOAT, VK_FORMAT_R32G32B32_SFLOAT },
		{ rhi::Format::R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM },
		{ rhi::Format::B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM }
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

		VkFormat result = format < rhi::Format::COUNT ?  Vulkan::c_FormatMap[ (uint8_t)format ].vkFormat :  VK_FORMAT_UNDEFINED;
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



	








};




IFNITY_END_NAMESPACE