//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-17 by alfonsmagd





#pragma once

#include <pch.h>
#include "d3d12_classes.hpp"
#include "d3d12_ImmediateCommands.hpp"
#include <d3d12.h>
#include <wrl.h>
#include "Ifnity/Graphics/ifrhi.h"
#include "Ifnity/Graphics/Interfaces/IDevice.hpp"

IFNITY_NAMESPACE

class DeviceD3D12;
struct ScissorRect;
struct ViewPortState;
class D3D12Image;

namespace D3D12
{
	using namespace Microsoft::WRL;
	using RenderPass = rhi::RenderPass;
	using Framebuffer = rhi::Framebuffer<D3D12::TextureHandleSM>;

	class CommandBuffer final
	{

	public:
		CommandBuffer() = default;
		explicit CommandBuffer(DeviceD3D12* ctx);
		~CommandBuffer();

		CommandBuffer& operator=(CommandBuffer&& other) = default;

		void cmdBeginRendering(D3D12Image* colorTex);
		void cmdBeginRendering(const RenderPass renderPass, Framebuffer& fb);
		void cmdEndRendering();
		void cmdBindViewport(const ViewPortState& state);
		void cmdBindScissorRect(const ScissorRect& rect);
		void cmdRenderImgui(ImDrawData* drawData, ID3D12DescriptorHeap* pCbvSrvHeap);
		void cmdBindVertexBuffer( BufferHandleSM& bf, uint32_t stride = 0, uint32_t offset = 0 );
		void cmdBindIndexBuffer( BufferHandleSM& bf, uint32_t offset = 0 );
		void cmdBindRenderPipeline( GraphicsPipeline* pipeline );
		void cmdSetPrimitiveTopology( rhi::PrimitiveType primitiveType );

		void cmdDraw(DrawModeUse drawMode, uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t baseInstance = 0);

		template<PushConstentD3D12 Struct>
		void cmdPushConstants(const Struct& data, size_t offset = 0)
		{
			this->cmdPushConstants(&data, sizeof(Struct), offset);
		}
	
		void cmdPushConstants(const void* data, size_t size, size_t offset = 0);

	private:
		friend class DeviceD3D12;
		friend class Device;

		DeviceD3D12* ctx_ = nullptr;
		const D3D12ImmediateCommands::CommandListWrapper* wrapper_;

		SubmitHandle lastSubmitHandle_ = {};
		//Framebuffer framebuffer_ = {};

		ID3D12PipelineState* lastPipelineBound_ = nullptr;
		GraphicsPipelineHandleSM currentPipelineGraphics_;

		bool isRendering_ = false;



	};


}
IFNITY_END_NAMESPACE