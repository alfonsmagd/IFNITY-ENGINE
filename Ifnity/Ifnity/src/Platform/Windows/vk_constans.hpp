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
	static rhi::Format GetRHIFormat(VkFormat vkFormat)
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