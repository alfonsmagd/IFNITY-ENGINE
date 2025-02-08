

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



	//----------------------------------------------------------------------------------------------------//
	//Destroy operations
	//----------------------------------------------------------------------------------------------------//
	void destroy(DeviceVulkan* ctx, TextureHandleSM handle)
	{
		if(ctx)
		{
			ctx->destroy(handle);
		}
		
	}

	void destroy(DeviceVulkan* ctx, GraphicsPipelineHandleSM handle)
	{
		if(ctx)
		{
			ctx->destroy(handle);
		}
	
	}


}
IFNITY_END_NAMESPACE