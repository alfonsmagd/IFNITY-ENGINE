

#include "vulkan_PipelineBuilder.hpp"
#include "../Windows/UtilsVulkan.h"



IFNITY_NAMESPACE


namespace Vulkan
{



	VulkanPipelineBuilder::VulkanPipelineBuilder()
	{
		// Configuración de vertexInputState_
		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = 0;
		vertexInputState.pVertexBindingDescriptions = nullptr;
		vertexInputState.vertexAttributeDescriptionCount = 0;
		vertexInputState.pVertexAttributeDescriptions = nullptr;
		vertexInputState_ = vertexInputState;

		// Configuración de inputAssembly_
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.flags = 0;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		inputAssembly_ = inputAssembly;

		// Configuración de tessellationState_
		VkPipelineTessellationStateCreateInfo tessellationState = {};
		tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		tessellationState.flags = 0;
		tessellationState.patchControlPoints = 0;
		tessellationState_ = tessellationState;

		// Configuración de rasterizationState_
		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.flags = 0;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.depthBiasConstantFactor = 0.0f;
		rasterizationState.depthBiasClamp = 0.0f;
		rasterizationState.depthBiasSlopeFactor = 0.0f;
		rasterizationState.lineWidth = 1.0f;
		rasterizationState_ = rasterizationState;

		// Configuración de multisampleState_
		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.minSampleShading = 0.0f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;
		multisampleState_ = multisampleState;

		// Configuración de depthStencilState_
		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.pNext = nullptr;
		depthStencilState.flags = 0;
		depthStencilState.depthTestEnable = VK_FALSE;
		depthStencilState.depthWriteEnable = VK_FALSE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front =
		{
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.depthFailOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_NEVER,
			.compareMask = 0,
			.writeMask = 0,
			.reference = 0,
		};
		depthStencilState.back =
		{
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.depthFailOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_NEVER,
			.compareMask = 0,
			.writeMask = 0,
			.reference = 0,
		};
		depthStencilState.minDepthBounds = 0.0f;
		depthStencilState.maxDepthBounds = 1.0f;
		depthStencilState_ = depthStencilState;
	}

	VulkanPipelineBuilder& VulkanPipelineBuilder::dynamicState(VkDynamicState state)
	{
		if(numDynamicStates_ < LVK_MAX_DYNAMIC_STATES)
		{
			dynamicStates_[ numDynamicStates_++ ] = state;
		}
		return *this;
	}

	VulkanPipelineBuilder& VulkanPipelineBuilder::primitiveTopology(VkPrimitiveTopology topology)
	{
		inputAssembly_.topology = topology;
		return *this;
	}

	VulkanPipelineBuilder& VulkanPipelineBuilder::rasterizationSamples(VkSampleCountFlagBits samples, float minSampleShading)
	{
		multisampleState_.rasterizationSamples = samples;
		multisampleState_.sampleShadingEnable = minSampleShading > 0.0f ? VK_TRUE : VK_FALSE;
		multisampleState_.minSampleShading = minSampleShading;
		return *this;
	}


	VulkanPipelineBuilder& VulkanPipelineBuilder::shaderStage(VkPipelineShaderStageCreateInfo stage)
	{
		if(stage.module != VK_NULL_HANDLE)
		{
			IFNITY_ASSERT_MSG(numShaderStages_ < ARRAY_NUM_ELEMENTS(shaderStages_), "Too many shader stages");
			shaderStages_[ numShaderStages_++ ] = stage;
		}
		return *this;
	}


	VulkanPipelineBuilder& VulkanPipelineBuilder::stencilStateOps(VkStencilFaceFlags faceMask,
		VkStencilOp failOp,
		VkStencilOp passOp,
		VkStencilOp depthFailOp,
		VkCompareOp compareOp)
	{
		depthStencilState_.stencilTestEnable = depthStencilState_.stencilTestEnable == VK_TRUE ||
			failOp != VK_STENCIL_OP_KEEP ||
			passOp != VK_STENCIL_OP_KEEP ||
			depthFailOp != VK_STENCIL_OP_KEEP ||
			compareOp != VK_COMPARE_OP_ALWAYS
			? VK_TRUE
			: VK_FALSE;

		if(faceMask & VK_STENCIL_FACE_FRONT_BIT)
		{
			VkStencilOpState& s = depthStencilState_.front;
			s.failOp = failOp;
			s.passOp = passOp;
			s.depthFailOp = depthFailOp;
			s.compareOp = compareOp;
		}

		if(faceMask & VK_STENCIL_FACE_BACK_BIT)
		{
			VkStencilOpState& s = depthStencilState_.back;
			s.failOp = failOp;
			s.passOp = passOp;
			s.depthFailOp = depthFailOp;
			s.compareOp = compareOp;
		}
		return *this;
	}

	VulkanPipelineBuilder& VulkanPipelineBuilder::stencilMasks(VkStencilFaceFlags faceMask,
		uint32_t compareMask,
		uint32_t writeMask,
		uint32_t reference)
	{
		if(faceMask & VK_STENCIL_FACE_FRONT_BIT)
		{
			VkStencilOpState& s = depthStencilState_.front;
			s.compareMask = compareMask;
			s.writeMask = writeMask;
			s.reference = reference;
		}

		if(faceMask & VK_STENCIL_FACE_BACK_BIT)
		{
			VkStencilOpState& s = depthStencilState_.back;
			s.compareMask = compareMask;
			s.writeMask = writeMask;
			s.reference = reference;
		}
		return *this;
	}

	//Cull mode 
	VulkanPipelineBuilder& VulkanPipelineBuilder::cullMode(VkCullModeFlags mode)
	{
		rasterizationState_.cullMode = mode;
		return *this;
	}


	//Front face
	VulkanPipelineBuilder& VulkanPipelineBuilder::frontFace(VkFrontFace mode)
	{
		rasterizationState_.frontFace = mode;
		return *this;
	}

	//Polygon mode
	VulkanPipelineBuilder& VulkanPipelineBuilder::polygonMode(VkPolygonMode mode)
	{
		rasterizationState_.polygonMode = mode;
		return *this;
	}


	// color attachments 
	VulkanPipelineBuilder& VulkanPipelineBuilder::colorAttachments(const VkPipelineColorBlendAttachmentState* states,
		const VkFormat* formats,
		uint32_t numColorAttachments)
	{
		assert(states);
		assert(formats);
		assert(numColorAttachments <= ARRAY_NUM_ELEMENTS(colorBlendAttachmentStates_));
		assert(numColorAttachments <= ARRAY_NUM_ELEMENTS(colorAttachmentFormats_));
		for(uint32_t i = 0; i != numColorAttachments; i++)
		{
			colorBlendAttachmentStates_[ i ] = states[ i ];
			colorAttachmentFormats_[ i ] = formats[ i ];
		}
		numColorAttachments_ = numColorAttachments;
		return *this;
	}


	// depth Attachment Format 
	VulkanPipelineBuilder& VulkanPipelineBuilder::depthAttachmentFormat(VkFormat format)
	{
		depthAttachmentFormat_ = format;
		return *this;
	}

	// stencil Attachment Format
	VulkanPipelineBuilder& VulkanPipelineBuilder::stencilAttachmentFormat(VkFormat format)
	{
		stencilAttachmentFormat_ = format;
		return *this;
	}

	//Vertex Input State
	VulkanPipelineBuilder& VulkanPipelineBuilder::vertexInputState(const VkPipelineVertexInputStateCreateInfo& state)
	{
		vertexInputState_ = state;
		return *this;
	}

	//Build the Graphics Pipeline

	VkResult VulkanPipelineBuilder::build(VkDevice device,
		VkPipelineCache pipelineCache,
		VkPipelineLayout pipelineLayout,
		VkPipeline* outPipeline, const char* debugName) noexcept
	{
		const VkPipelineDynamicStateCreateInfo dynamicState =
		{
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		 .dynamicStateCount = numDynamicStates_,
		 .pDynamicStates = dynamicStates_,
		};
		// viewport and scissor can be NULL if the viewport state is dynamic
		// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkPipelineViewportStateCreateInfo.html
		const VkPipelineViewportStateCreateInfo viewportState =
		{
			 .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			 .viewportCount = 1,
			 .pViewports = nullptr,
			 .scissorCount = 1,
			 .pScissors = nullptr,
		};
		const VkPipelineColorBlendStateCreateInfo colorBlendState =
		{
			 .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			 .logicOpEnable = VK_FALSE,
			 .logicOp = VK_LOGIC_OP_COPY,
			 .attachmentCount = numColorAttachments_,
			 .pAttachments = colorBlendAttachmentStates_,
		};

		//Using the dynamic rendering , not uses renderpass . 
		const VkPipelineRenderingCreateInfo renderingInfo =
		{
			 .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
			 .pNext = nullptr,
			 .colorAttachmentCount = numColorAttachments_,
			 .pColorAttachmentFormats = colorAttachmentFormats_,
			 .depthAttachmentFormat = depthAttachmentFormat_,
			 .stencilAttachmentFormat = stencilAttachmentFormat_,
		};

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
			 .pDepthStencilState = &depthStencilState_,
			 .pColorBlendState = &colorBlendState,
			 .pDynamicState = &dynamicState,
			 .layout = pipelineLayout,
			 .renderPass = VK_NULL_HANDLE,
			 .subpass = 0,
			 .basePipelineHandle = VK_NULL_HANDLE,
			 .basePipelineIndex = -1,
		};

		const auto result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &ci, nullptr, outPipeline);

		if(!(result == VK_SUCCESS))
		{
			return result;
		}

		numPipelinesCreated_++;

		// set debug name
		// set debug name
		return setDebugObjectName(device, VK_OBJECT_TYPE_PIPELINE, (uint64_t)*outPipeline, debugName);
	}

}

IFNITY_END_NAMESPACE