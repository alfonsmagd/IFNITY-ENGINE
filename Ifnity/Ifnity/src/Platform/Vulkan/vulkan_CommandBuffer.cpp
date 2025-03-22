

#include "vulkan_CommandBuffer.hpp"
#include "../Windows/UtilsVulkan.h"
#include "../Windows/DeviceVulkan.h"
#include "../Windows/vk_constans.hpp"

IFNITY_NAMESPACE



namespace Vulkan
{

	//help function to transition to color attachment
	void static transitionToColorAttachment(VkCommandBuffer buffer, VulkanImage* colorTex)
	{
		if( !(colorTex) )
		{
			return;
		}

		if( !(!colorTex->isDepthFormat_ && !colorTex->isStencilFormat_) )
		{
			IFNITY_LOG(LogCore, ERROR, "Color attachments cannot have depth/stencil formats");
			return;
		}
		IFNITY_ASSERT_MSG(colorTex->vkImageFormat_ != VK_FORMAT_UNDEFINED, "Invalid color attachment format");
		colorTex->transitionLayout(buffer,
								   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
								   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,// wait for all subsequent// fragment/compute shaders
								   VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS });
	}

	Vulkan::CommandBuffer::CommandBuffer(DeviceVulkan* ctx): ctx_(ctx), wrapper_(&ctx->immediate_->acquire())
	{}

	CommandBuffer::~CommandBuffer()
	{}


	void CommandBuffer::cmdBindViewport(const ViewPortState& viewport)
	{
		// https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
		const VkViewport vp = {
		.x = (float)viewport.x, // float x;
		.y = (float)viewport.y + (float)viewport.height, // float y;
		.width = (float)viewport.width, // float width;
		.height = -(float)viewport.height, // float height;
		.minDepth = (float)viewport.minDepth, // float minDepth;
		.maxDepth = (float)viewport.maxDepth, // float maxDepth;
		};
		vkCmdSetViewport(wrapper_->cmdBuf_, 0, 1, &vp);
	}

	void CommandBuffer::cmdBindScissorRect(const ScissorRect& rect)
	{
		const VkRect2D scissor =
		{
			 VkOffset2D{(int32_t)rect.x, (int32_t)rect.y},
			 VkExtent2D{rect.width, rect.height},
		};
		vkCmdSetScissor(wrapper_->cmdBuf_, 0, 1, &scissor);
	}

	void CommandBuffer::cmdBindDepthState(const DepthState& desc)
	{


		const VkCompareOp op = compareOpToVkCompareOp(desc.compareOp);
		vkCmdSetDepthWriteEnable(wrapper_->cmdBuf_, desc.isDepthWriteEnabled ? VK_TRUE : VK_FALSE);
		vkCmdSetDepthTestEnable(wrapper_->cmdBuf_, op != VK_COMPARE_OP_ALWAYS || desc.isDepthWriteEnabled);

		#if defined(ANDROID)
		// This is a workaround for the issue.
		// On Android (Mali-G715-Immortalis MC11 v1.r38p1-01eac0.c1a71ccca2acf211eb87c5db5322f569)
		// if depth-stencil texture is not set, call of vkCmdSetDepthCompareOp leads to disappearing of all content.
		if( !framebuffer_.depthStencil.texture )
		{
			return;
		}
		#endif
		vkCmdSetDepthCompareOp(wrapper_->cmdBuf_, op);
	}

	void CommandBuffer::cmdBeginRendering(const RenderPass& renderPass, Framebuffer& fb)
	{
		//Steps to begin rendering
		//1. Check if the rendering is already in progress
		//2. Transition the color and depth attachments


		//1. Check if the rendering is already in progress
		_ASSERT(!isRendering_);

		isRendering_ = true;

		const uint32_t numFbColorAttachments = fb.getNumColorAttachments();
		const uint32_t numPassColorAttachments = renderPass.getNumColorAttachments();

		_ASSERT(numPassColorAttachments == numFbColorAttachments);

		framebuffer_ = fb;

		//2. transition all the color attachments and depth-stencil attachment
			// transition all the color attachments
		for( uint32_t i = 0; i != numFbColorAttachments; i++ )
		{
			if( const auto handle = fb.color[ i ].texture )
			{
				VulkanImage* colorTex = ctx_->slootMapTextures_.get(handle);
				transitionToColorAttachment(wrapper_->cmdBuf_, colorTex);
			}
			// handle MSAA
			if( TextureHandleSM handle = fb.color[ i ].resolveTexture )
			{
				VulkanImage* colorResolveTex = ctx_->slootMapTextures_.get(handle);
				transitionToColorAttachment(wrapper_->cmdBuf_, colorResolveTex);
			}
		}
		// transition depth-stencil attachment

		TextureHandleSM depthTex = fb.depthStencil.texture;
		if( depthTex )
		{
			const VulkanImage& depthImg = *ctx_->slootMapTextures_.get(depthTex);
			IFNITY_ASSERT_MSG(depthImg.vkImageFormat_ != VK_FORMAT_UNDEFINED, "Invalid depth attachment format");
			const VkImageAspectFlags flags = depthImg.getImageAspectFlags();
			depthImg.transitionLayout(wrapper_->cmdBuf_,
									  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
									  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
									  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, // wait for all subsequent
									  // operations
									  VkImageSubresourceRange{ flags, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS });
		}

		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		uint32_t mipLevel = 0;
		uint32_t fbWidth = 0;
		uint32_t fbHeight = 0;


		//3. New structures are used to define the attachments used in dynamic rendering
		VkRenderingAttachmentInfo colorAttachments[ MAX_COLOR_ATTACHMENTS ];

		for( uint32_t i = 0; i != numFbColorAttachments; i++ )
		{
			Framebuffer::AttachmentDesc& attachment = fb.color[ i ];
			//_ASSERT(!attachment.texture.empty());

			auto& colorTexture = *ctx_->slootMapTextures_.get(attachment.texture);
			auto& descColor = renderPass.color[ i ];
			if( mipLevel && descColor.level )
			{
				IFNITY_ASSERT_MSG(descColor.level == mipLevel, "All color attachments should have the same mip-level");
			}
			const VkExtent3D dim = colorTexture.vkExtent_;
			if( fbWidth )
			{
				IFNITY_ASSERT_MSG(dim.width == fbWidth, "All attachments should have the same width");
			}
			if( fbHeight )
			{
				IFNITY_ASSERT_MSG(dim.height == fbHeight, "All attachments should have the same height");
			}
			mipLevel = descColor.level;
			fbWidth = dim.width;
			fbHeight = dim.height;
			samples = colorTexture.vkSamples_;
			colorAttachments[ i ] =
			{
				 .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
				 .pNext = nullptr,
				 .imageView = colorTexture.getOrCreateVkImageViewForFramebuffer(*ctx_, descColor.level, descColor.layer),
				 .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				 .resolveMode = (samples > 1) ? VK_RESOLVE_MODE_AVERAGE_BIT : VK_RESOLVE_MODE_NONE,
				 .resolveImageView = VK_NULL_HANDLE,
				 .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				 .loadOp = loadOpToVkAttachmentLoadOp(descColor.loadOp),
				 .storeOp = storeOpToVkAttachmentStoreOp(descColor.storeOp),
				 .clearValue =
					  {.color = {.float32 = {descColor.clearColor[ 0 ], descColor.clearColor[ 1 ], descColor.clearColor[ 2 ], descColor.clearColor[ 3 ]}}},
			};
			// handle MSAA 
			/*if(descColor.storeOp == StoreOp_MsaaResolve)
			{
				_ASSERT(samples > 1);
				_ASSERT_MSG(!attachment.resolveTexture.empty(), "Framebuffer attachment should contain a resolve texture");
				::VulkanImage& colorResolveTexture = *ctx_->texturesPool_.get(attachment.resolveTexture);
				colorAttachments[ i ].resolveImageView =
					colorResolveTexture.getOrCreateVkImageViewForFramebuffer(*ctx_, descColor.level, descColor.layer);
				colorAttachments[ i ].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}*/
		}

		//4. Depth attachment Info
		VkRenderingAttachmentInfo depthAttachment = {};

		if( fb.depthStencil.texture )
		{

			auto& depthTexture = *ctx_->slootMapTextures_.get(fb.depthStencil.texture);
			const RenderPass::AttachmentDesc& descDepth = renderPass.depth;
			IFNITY_ASSERT_MSG(descDepth.level == mipLevel, "Depth attachment should have the same mip-level as color attachments");
			depthAttachment = {
				 .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
				 .pNext = nullptr,
				 .imageView = depthTexture.getOrCreateVkImageViewForFramebuffer(*ctx_, descDepth.level, descDepth.layer),
				 .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				 .resolveMode = VK_RESOLVE_MODE_NONE,
				 .resolveImageView = VK_NULL_HANDLE,
				 .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				 .loadOp = loadOpToVkAttachmentLoadOp(descDepth.loadOp),
				 .storeOp = storeOpToVkAttachmentStoreOp(descDepth.storeOp),
				 .clearValue = {.depthStencil = {.depth = descDepth.clearDepth, .stencil = descDepth.clearStencil}},
			};
			// handle depth MSAA
			/*if(descDepth.storeOp == StoreOp_MsaaResolve)
			{
				_ASSERT(depthTexture.vkSamples_ == samples);
				const ::Framebuffer::AttachmentDesc& attachment = fb.depthStencil;
				_ASSERT_MSG(!attachment.resolveTexture.empty(), "Framebuffer depth attachment should contain a resolve texture");
				::VulkanImage& depthResolveTexture = *ctx_->texturesPool_.get(attachment.resolveTexture);
				depthAttachment.resolveImageView = depthResolveTexture.getOrCreateVkImageViewForFramebuffer(*ctx_, descDepth.level, descDepth.layer);
				depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				depthAttachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
			}*/
			const VkExtent3D dim = depthTexture.vkExtent_;
			if( fbWidth )
			{
				IFNITY_ASSERT_MSG(dim.width == fbWidth, "All attachments should have the same width");
			}
			if( fbHeight )
			{
				IFNITY_ASSERT_MSG(dim.height == fbHeight, "All attachments should have the same height");
			}
			mipLevel = descDepth.level;
			fbWidth = dim.width;
			fbHeight = dim.height;
		}

		const uint32_t width = std::max(fbWidth >> mipLevel, 1u);
		const uint32_t height = std::max(fbHeight >> mipLevel, 1u);
		const ViewPortState viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, +1.0f };
		const ScissorRect scissor = { 0, 0, width, height };

		VkRenderingAttachmentInfo stencilAttachment = depthAttachment;

		const bool isStencilFormat = renderPass.stencil.loadOp != LoadOp_Invalid;

		//5. Rendering Info Creation and binding differents RenderingInfos.
		const VkRenderingInfo renderingInfo = {
			 .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			 .pNext = nullptr,
			 .flags = 0,
			 .renderArea = {VkOffset2D{(int32_t)scissor.x, (int32_t)scissor.y}, VkExtent2D{scissor.width, scissor.height}},
			 .layerCount = 1,
			 .viewMask = 0,
			 .colorAttachmentCount = numFbColorAttachments,
			 .pColorAttachments = colorAttachments,
			 .pDepthAttachment = fb.depthStencil.texture ? &depthAttachment : nullptr,
			 .pStencilAttachment = isStencilFormat ? &stencilAttachment : nullptr,
		};

		cmdBindViewport(viewport);
		cmdBindScissorRect(scissor);
		cmdBindDepthState({});

		//ctx_->checkAndUpdateDescriptorSets();

		vkCmdSetDepthCompareOp(wrapper_->cmdBuf_, VK_COMPARE_OP_ALWAYS);
		vkCmdSetDepthBiasEnable(wrapper_->cmdBuf_, VK_FALSE);

		vkCmdBeginRendering(wrapper_->cmdBuf_, &renderingInfo);

	}



	void CommandBuffer::cmdBindRenderPipeline(GraphicsPipelineHandleSM pipeline)
	{
		if( pipeline.empty() )
		{
			return;
		}

		currentPipelineGraphics_ = pipeline;

		GraphicsPipeline* gp = ctx_->slotMapRenderPipelines_.get(pipeline);

		RenderPipelineState& rps = gp->getRenderPipelineState();
		if( !rps.pipeline_ )
		{
			return;
		}
		VkPipeline vkpipeline = rps.pipeline_;


		//First Bind the pipeline then the descriptor sets
		//
		if( lastPipelineBound_ != vkpipeline )
		{
			lastPipelineBound_ = vkpipeline;
			vkCmdBindPipeline(wrapper_->cmdBuf_, VK_PIPELINE_BIND_POINT_GRAPHICS, vkpipeline);
			ctx_->bindDefaultDescriptorSets(wrapper_->cmdBuf_, VK_PIPELINE_BIND_POINT_GRAPHICS, rps.pipelineLayout_);
		}

	}

	void CommandBuffer::cmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t baseInstance)
	{

		if( vertexCount == 0 )
		{
			return;
		}

		vkCmdDraw(wrapper_->cmdBuf_, vertexCount, instanceCount, firstVertex, baseInstance);
		//vkCmdDrawIndexed(wrapper_->cmdBuf_, vertexCount, 1, 0, 0, 0);

	}

	void CommandBuffer::cmdDraw(DrawModeUse drawMode, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t baseInstance)
	{
		if( drawMode == DRAW )
		{
			this->cmdDraw(vertexCount, instanceCount, firstVertex, baseInstance);
		}
		else
		{
			if( vertexCount == 0 ) { return; }

			vkCmdDrawIndexed(wrapper_->cmdBuf_, vertexCount, 1, 0, 0, 0);
		}


	}

	void CommandBuffer::cmdPushConstants(const void* data, size_t size, size_t offset)
	{
		_ASSERT(isRendering_);
		_ASSERT(currentPipelineGraphics_);
		GraphicsPipeline* gp = ctx_->slotMapRenderPipelines_.get(currentPipelineGraphics_);

		RenderPipelineState* rps = &gp->getRenderPipelineState();

		vkCmdPushConstants(wrapper_->cmdBuf_,
						   rps->pipelineLayout_,
						   rps->shaderStageFlags_,
						   (uint32_t)offset,
						   (uint32_t)size,
						   data);
	}

	void CommandBuffer::cmdBindVertexBuffer(uint32_t index, BufferHandleSM buffer, uint64_t bufferOffset)
	{

		if( !IFNITY_VERIFY(!buffer.empty()) )
		{
			return;
		}

		VulkanBuffer* buf = ctx_->slotMapBuffers_.get(buffer);

		IFNITY_ASSERT(buf->vkUsageFlags_ & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

		vkCmdBindVertexBuffers(wrapper_->cmdBuf_, index, 1, &buf->vkBuffer_, &bufferOffset);

	}

	void CommandBuffer::cmdBindIndexBuffer(BufferHandleSM indexBuffer, rhi::IndexFormat indexFormat, uint64_t indexBufferOffset)
	{
		VulkanBuffer* buf = ctx_->slotMapBuffers_.get(indexBuffer);

		IFNITY_ASSERT(buf->vkUsageFlags_ & VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

		const VkIndexType type = indexFormatToVkIndexType(indexFormat);
		vkCmdBindIndexBuffer(wrapper_->cmdBuf_, buf->vkBuffer_, indexBufferOffset, type);
	}



	void CommandBuffer::cmdSetDepthBias(float constantFactor, float slopeFactor, float clamp)
	{
		vkCmdSetDepthBias(wrapper_->cmdBuf_, constantFactor, clamp, slopeFactor);
	}

	void CommandBuffer::cmdSetDepthBiasEnable(bool enable)
	{
		vkCmdSetDepthBiasEnable(wrapper_->cmdBuf_, enable ? VK_TRUE : VK_FALSE);
	}



	void CommandBuffer::cmdEndRendering()
	{
		assert(isRendering_);

		isRendering_ = false;

		vkCmdEndRendering(wrapper_->cmdBuf_);

		const uint32_t numFbColorAttachments = framebuffer_.getNumColorAttachments();

		// set image layouts after the render pass
		for( uint32_t i = 0; i != numFbColorAttachments; i++ )
		{
			auto& attachment = framebuffer_.color[ i ];
			const VulkanImage& tex = *ctx_->slootMapTextures_.get(attachment.texture);

			tex.vkImageLayout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		if( framebuffer_.depthStencil.texture )
		{
			auto& depthStencil = framebuffer_.depthStencil;
			const VulkanImage& tex = *ctx_->slootMapTextures_.get(depthStencil.texture);
			// this must match the final layout of the render pass
			tex.vkImageLayout_ = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		framebuffer_ = {};
	}



}

IFNITY_END_NAMESPACE