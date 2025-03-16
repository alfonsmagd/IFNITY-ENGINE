

#include "vulkan_PipelineBuilder.hpp"
#include "../Windows/UtilsVulkan.h"



IFNITY_NAMESPACE


namespace Vulkan
{



	VulkanPipelineBuilder::VulkanPipelineBuilder():
	vertexInputState_(VkPipelineVertexInputStateCreateInfo{
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		 .vertexBindingDescriptionCount = 0,
		 .pVertexBindingDescriptions = nullptr,
		 .vertexAttributeDescriptionCount = 0,
		 .pVertexAttributeDescriptions = nullptr,
							}),
							inputAssembly_({
								 .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
								 .flags = 0,
								 .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
								 .primitiveRestartEnable = VK_FALSE,
												}),
												tessellationState_({
													 .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
													 .flags = 0,
													 .patchControlPoints = 0,
																		 }),
																		 rasterizationState_({
																			  .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
																			  .flags = 0,
																			  .depthClampEnable = VK_FALSE,
																			  .rasterizerDiscardEnable = VK_FALSE,
																			  .polygonMode = VK_POLYGON_MODE_FILL,
																			  .cullMode = VK_CULL_MODE_NONE,
																			  .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
																			  .depthBiasEnable = VK_FALSE,
																			  .depthBiasConstantFactor = 0.0f,
																			  .depthBiasClamp = 0.0f,
																			  .depthBiasSlopeFactor = 0.0f,
																			  .lineWidth = 1.0f,
																									}),
																									multisampleState_({
																										 .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
																										 .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
																										 .sampleShadingEnable = VK_FALSE,
																										 .minSampleShading = 0.0f,
																										 .pSampleMask = nullptr,
																										 .alphaToCoverageEnable = VK_FALSE,
																										 .alphaToOneEnable = VK_FALSE,
																															}),
																															depthStencilState_({
																																 .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
																																 .pNext = NULL,
																																 .flags = 0,
																																 .depthTestEnable = VK_FALSE,
																																 .depthWriteEnable = VK_FALSE,
																																 .depthCompareOp = VK_COMPARE_OP_LESS,
																																 .depthBoundsTestEnable = VK_FALSE,
																																 .stencilTestEnable = VK_FALSE,
																																 .front =
																																	  {
																																			.failOp = VK_STENCIL_OP_KEEP,
																																			.passOp = VK_STENCIL_OP_KEEP,
																																			.depthFailOp = VK_STENCIL_OP_KEEP,
																																			.compareOp = VK_COMPARE_OP_NEVER,
																																			.compareMask = 0,
																																			.writeMask = 0,
																																			.reference = 0,
																																	  },
																																 .back =
																																	  {
																																			.failOp = VK_STENCIL_OP_KEEP,
																																			.passOp = VK_STENCIL_OP_KEEP,
																																			.depthFailOp = VK_STENCIL_OP_KEEP,
																																			.compareOp = VK_COMPARE_OP_NEVER,
																																			.compareMask = 0,
																																			.writeMask = 0,
																																			.reference = 0,
																																	  },
																																 .minDepthBounds = 0.0f,
																																 .maxDepthBounds = 1.0f,
																																					 })
{}

VulkanPipelineBuilder & VulkanPipelineBuilder::dynamicState(VkDynamicState state)
{
	IFNITY_ASSERT(numDynamicStates_ < LVK_MAX_DYNAMIC_STATES);
	dynamicStates_[ numDynamicStates_++ ] = state;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::primitiveTopology(VkPrimitiveTopology topology)
{
	inputAssembly_.topology = topology;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::rasterizationSamples(VkSampleCountFlagBits samples, float minSampleShading)
{
	multisampleState_.rasterizationSamples = samples;
	multisampleState_.sampleShadingEnable = minSampleShading > 0 ? VK_TRUE : VK_FALSE;
	multisampleState_.minSampleShading = minSampleShading;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::cullMode(VkCullModeFlags mode)
{
	rasterizationState_.cullMode = mode;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::frontFace(VkFrontFace mode)
{
	rasterizationState_.frontFace = mode;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::polygonMode(VkPolygonMode mode)
{
	rasterizationState_.polygonMode = mode;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::vertexInputState(const VkPipelineVertexInputStateCreateInfo & state)
{
	vertexInputState_ = state;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::colorAttachments(const VkPipelineColorBlendAttachmentState * states,
																								  const VkFormat * formats,
																								  uint32_t numColorAttachments)
{
	IFNITY_ASSERT(states);
	IFNITY_ASSERT(formats);
	IFNITY_ASSERT(numColorAttachments <= ARRAY_NUM_ELEMENTS(colorBlendAttachmentStates_));
	IFNITY_ASSERT(numColorAttachments <= ARRAY_NUM_ELEMENTS(colorAttachmentFormats_));
	for( uint32_t i = 0; i != numColorAttachments; i++ )
	{
		colorBlendAttachmentStates_[ i ] = states[ i ];
		colorAttachmentFormats_[ i ] = formats[ i ];
	}
	numColorAttachments_ = numColorAttachments;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::depthAttachmentFormat(VkFormat format)
{
	depthAttachmentFormat_ = format;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::stencilAttachmentFormat(VkFormat format)
{
	stencilAttachmentFormat_ = format;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::patchControlPoints(uint32_t numPoints)
{
	tessellationState_.patchControlPoints = numPoints;
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::shaderStage(VkPipelineShaderStageCreateInfo stage)
{
	if( stage.module != VK_NULL_HANDLE )
	{
		IFNITY_ASSERT(numShaderStages_ < ARRAY_NUM_ELEMENTS(shaderStages_));
		shaderStages_[ numShaderStages_++ ] = stage;
	}
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::stencilStateOps(VkStencilFaceFlags faceMask,
																								 VkStencilOp failOp,
																								 VkStencilOp passOp,
																								 VkStencilOp depthFailOp,
																								 VkCompareOp compareOp)
{
	depthStencilState_.stencilTestEnable = depthStencilState_.stencilTestEnable == VK_TRUE || failOp != VK_STENCIL_OP_KEEP ||
		passOp != VK_STENCIL_OP_KEEP || depthFailOp != VK_STENCIL_OP_KEEP ||
		compareOp != VK_COMPARE_OP_ALWAYS
		? VK_TRUE
		: VK_FALSE;

	if( faceMask & VK_STENCIL_FACE_FRONT_BIT )
	{
		VkStencilOpState & s = depthStencilState_.front;
		s.failOp = failOp;
		s.passOp = passOp;
		s.depthFailOp = depthFailOp;
		s.compareOp = compareOp;
	}

	if( faceMask & VK_STENCIL_FACE_BACK_BIT )
	{
		VkStencilOpState & s = depthStencilState_.back;
		s.failOp = failOp;
		s.passOp = passOp;
		s.depthFailOp = depthFailOp;
		s.compareOp = compareOp;
	}
	return *this;
}

VulkanPipelineBuilder & VulkanPipelineBuilder::stencilMasks(VkStencilFaceFlags faceMask,
																							 uint32_t compareMask,
																							 uint32_t writeMask,
																							 uint32_t reference)
{
	if( faceMask & VK_STENCIL_FACE_FRONT_BIT )
	{
		VkStencilOpState & s = depthStencilState_.front;
		s.compareMask = compareMask;
		s.writeMask = writeMask;
		s.reference = reference;
	}

	if( faceMask & VK_STENCIL_FACE_BACK_BIT )
	{
		VkStencilOpState & s = depthStencilState_.back;
		s.compareMask = compareMask;
		s.writeMask = writeMask;
		s.reference = reference;
	}
	return *this;
}

VkResult VulkanPipelineBuilder::build(VkDevice device,
														 VkPipelineCache pipelineCache,
														 VkPipelineLayout pipelineLayout,
														 VkPipeline * outPipeline,
														 const char * debugName) noexcept
{
	const VkPipelineDynamicStateCreateInfo dynamicState = {
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		 .dynamicStateCount = numDynamicStates_,
		 .pDynamicStates = dynamicStates_,
	};
	// viewport and scissor can be NULL if the viewport state is dynamic
	// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkPipelineViewportStateCreateInfo.html
	const VkPipelineViewportStateCreateInfo viewportState = {
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		 .viewportCount = 1,
		 .pViewports = nullptr,
		 .scissorCount = 1,
		 .pScissors = nullptr,
	};
	const VkPipelineColorBlendStateCreateInfo colorBlendState = {
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		 .logicOpEnable = VK_FALSE,
		 .logicOp = VK_LOGIC_OP_COPY,
		 .attachmentCount = numColorAttachments_,
		 .pAttachments = colorBlendAttachmentStates_,
	};
	const VkPipelineRenderingCreateInfoKHR renderingInfo = {
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
		 .pNext = nullptr,
		 .colorAttachmentCount = numColorAttachments_,
		 .pColorAttachmentFormats = colorAttachmentFormats_,
		 .depthAttachmentFormat = depthAttachmentFormat_,
		 .stencilAttachmentFormat = stencilAttachmentFormat_,
	};

	VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depthStencilStateCI.depthTestEnable = VK_TRUE;
		depthStencilStateCI.depthWriteEnable = VK_TRUE;
		depthStencilStateCI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilStateCI.depthBoundsTestEnable = VK_FALSE;
		depthStencilStateCI.back.failOp = VK_STENCIL_OP_KEEP;
		depthStencilStateCI.back.passOp = VK_STENCIL_OP_KEEP;
		depthStencilStateCI.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencilStateCI.stencilTestEnable = VK_FALSE;
		depthStencilStateCI.front = depthStencilStateCI.back;

	const VkGraphicsPipelineCreateInfo ci = {
		 .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		 .pNext = &renderingInfo,
		 .flags = 0,
		 .stageCount = numShaderStages_,
		 .pStages = shaderStages_,
		 .pVertexInputState = &vertexInputState_,
		 .pInputAssemblyState = &inputAssembly_,
		 .pTessellationState = &tessellationState_,
		 .pViewportState = &viewportState,
		 .pRasterizationState = &rasterizationState_,
		 .pMultisampleState = &multisampleState_,
		 .pDepthStencilState = &depthStencilStateCI,
		 .pColorBlendState = &colorBlendState,
		 .pDynamicState = &dynamicState,
		 .layout = pipelineLayout,
		 .renderPass = VK_NULL_HANDLE,
		 .subpass = 0,
		 .basePipelineHandle = VK_NULL_HANDLE,
		 .basePipelineIndex = -1,
	};

	const VkResult result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &ci, nullptr, outPipeline);

	if( !(result == VK_SUCCESS) )
	{
		return result;
	}
	std::cout << "Pipeline depth format: " << renderingInfo.depthAttachmentFormat << std::endl;
	

	numPipelinesCreated_++;

	// set debug name
	return setDebugObjectName(device, VK_OBJECT_TYPE_PIPELINE, (uint64_t)*outPipeline, debugName);
}

}

IFNITY_END_NAMESPACE