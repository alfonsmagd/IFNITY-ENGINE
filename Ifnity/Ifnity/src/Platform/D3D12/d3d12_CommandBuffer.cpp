//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-04-29 by alfonsmagd



#include "d3d12_CommandBuffer.hpp"
#include "d3d12_ImmediateCommands.hpp"

#include "../Windows/DeviceD3D12.hpp"

#include "Ifnity/Graphics/Interfaces/IGraphicsPipeline.hpp"

IFNITY_NAMESPACE

namespace D3D12
{

	CommandBuffer::CommandBuffer(DeviceD3D12* ctx): ctx_(ctx), wrapper_(&ctx->m_ImmediateCommands->acquire())
	{}

	CommandBuffer::~CommandBuffer()
	{}

	void CommandBuffer::cmdBindScissorRect(const ScissorRect& rect)
	{
		 D3D12_RECT scissor{
				.left = static_cast<LONG>(rect.x),
				.top = static_cast<LONG>(rect.y),
				.right = static_cast<LONG>(rect.x + rect.width),
				.bottom = static_cast<LONG>(rect.y + rect.height)
			};
			wrapper_->commandList->RSSetScissorRects(1, &scissor);
		
	}
	void CommandBuffer::cmdBindViewport(const ViewPortState& state)
	{
		
			 D3D12_VIEWPORT screenViewport{
				 .TopLeftX = static_cast<float>(state.x),
				 .TopLeftY = static_cast<float>(state.y),
				 .Width = static_cast<float>(state.width),
				 .Height = static_cast<float>(state.height),
				 .MinDepth = static_cast<float>(state.minDepth),
				 .MaxDepth = static_cast<float>(state.maxDepth)

			};
			wrapper_->commandList->RSSetViewports(1, &screenViewport);
		
	}

}
IFNITY_END_NAMESPACE