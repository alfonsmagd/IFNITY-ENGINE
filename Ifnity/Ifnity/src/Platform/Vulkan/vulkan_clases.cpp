

#include "vulkan_classes.hpp"
#include "../Windows/UtilsVulkan.h"
#include "Platform/Windows/DeviceVulkan.h"
#include "vulkan_PipelineBuilder.hpp"



IFNITY_NAMESPACE


namespace Vulkan
{
	//----------------------------------------------------------------------------------------------------//
	// Static Variables
	//----------------------------------------------------------------------------------------------------//

	uint32_t VulkanPipelineBuilder::numPipelinesCreated_ = 0;

	//Generic Implementation Destroy function 
	template<typename Handle>
	void destroy(DeviceVulkan* ctx, Handle handle)
	{
		if(ctx)
		{
			ctx->destroy(handle);
		}
	}

	// Instanciación explícita de las especializaciones
	template void destroy<TextureHandleSM>(DeviceVulkan* ctx, TextureHandleSM handle);
	template void destroy<GraphicsPipelineHandleSM>(DeviceVulkan* ctx, GraphicsPipelineHandleSM handle);
	template void destroy<ShaderModuleHandleSM>(DeviceVulkan* ctx, ShaderModuleHandleSM handle);


}
IFNITY_END_NAMESPACE