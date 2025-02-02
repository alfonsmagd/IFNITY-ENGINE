#pragma once

#include <pch.h>
#include <VkBootstrap.h>



IFNITY_NAMESPACE


namespace Vulkan
{

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
		VkPipelineShaderStageCreateInfo shaderStages_[ 5 ] = {};

		VkPipelineVertexInputStateCreateInfo vertexInputState_;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly_;
		VkPipelineRasterizationStateCreateInfo rasterizationState_;
		VkPipelineMultisampleStateCreateInfo multisampleState_;
		VkPipelineDepthStencilStateCreateInfo depthStencilState_;
		VkPipelineTessellationStateCreateInfo tessellationState_;

		uint32_t numColorAttachments_ = 0;
		VkPipelineColorBlendAttachmentState colorBlendAttachmentStates_[ 8 ] = {};
		VkFormat colorAttachmentFormats_[ 8 ] = {};

		VkFormat depthAttachmentFormat_ = VK_FORMAT_UNDEFINED;
		VkFormat stencilAttachmentFormat_ = VK_FORMAT_UNDEFINED;

		static uint32_t numPipelinesCreated_;


	};

} // namespace Vulkan

IFNITY_END_NAMESPACE