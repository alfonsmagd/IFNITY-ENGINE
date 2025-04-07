#pragma once

#include <pch.h>
#include <VkBootstrap.h>
#include "vulkan_ImmediateCommands.hpp"



IFNITY_NAMESPACE

class DeviceVulkan;
enum DrawModeUse;

namespace Vulkan
{

	class CommandBuffer final
	{
	public:
		CommandBuffer() = default;
		explicit CommandBuffer(DeviceVulkan* ctx);
		~CommandBuffer();

		CommandBuffer& operator=(CommandBuffer&& other) = default;

		void cmdBindViewport(const ViewPortState& state);
		void cmdBindScissorRect(const ScissorRect& rect);
		void cmdBeginRendering(const RenderPass& renderPass, Framebuffer& fb);
		void cmdBindDepthState(const DepthState& state);
		void cmdBindRenderPipeline(GraphicsPipelineHandleSM pipeline);
		void  cmdDraw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t baseInstance = 0);
		void cmdDraw(DrawModeUse drawMode, uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t baseInstance = 0);
		void cmdPushConstants(const void* data, size_t size, size_t offset = 0);
		void cmdBindVertexBuffer( uint32_t index, BufferHandleSM buffer, uint64_t bufferOffset = 0);
		void cmdBindIndexBuffer(BufferHandleSM indexBuffer, rhi::IndexFormat indexFormat, uint64_t indexBufferOffset = 0);
		void cmdSetDepthBias(float constantFactor, float slopeFactor, float clamp);
		void cmdSetDepthBiasEnable(bool enable);
		template<PushConstentImpl Struct>
		void cmdPushConstants(const Struct& data, size_t offset = 0)
		{
			this->cmdPushConstants(&data, sizeof(Struct), offset);
		}
		void cmdEndRendering();

	private:
		friend class DeviceVulkan;
		friend class Device;
		DeviceVulkan* ctx_ = nullptr;
		const VulkanImmediateCommands::CommandBufferWrapper* wrapper_ = nullptr;

		SubmitHandle lastSubmitHandle_ = {};
		Framebuffer framebuffer_ = {};

		VkPipeline lastPipelineBound_ = VK_NULL_HANDLE;
		GraphicsPipelineHandleSM currentPipelineGraphics_;

		bool isRendering_ = false;
	};
	

} // namespace Vulkan

IFNITY_END_NAMESPACE