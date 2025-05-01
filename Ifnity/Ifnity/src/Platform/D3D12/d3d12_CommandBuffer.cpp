//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-01 by alfonsmagd





#include "d3d12_CommandBuffer.hpp"
#include "d3d12_ImmediateCommands.hpp"

#include "../Windows/DeviceD3D12.hpp"

#include "Ifnity/Graphics/Interfaces/IGraphicsPipeline.hpp"

IFNITY_NAMESPACE

namespace D3D12
{


	// Helper function to transition a color attachment
	inline static void transitionToColorAttachment(ID3D12GraphicsCommandList* cmdList, D3D12Image* colorTex)
	{
		if( !colorTex )
			return;

		if( colorTex->isDepthFormat_ || colorTex->isStencilFormat_ )
		{
			IFNITY_LOG(LogCore, ERROR, "Color attachments cannot have depth/stencil formats");
			return;
		}

		if( colorTex->format_ == DXGI_FORMAT_UNKNOWN )
		{
			IFNITY_LOG(LogCore, ERROR, "Invalid color attachment format");
			return;
		}

		// Apply barrier
		const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			colorTex->resource_.Get(),
			colorTex->currentState_,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		cmdList->ResourceBarrier(1, &barrier);

		// Update current state (si mantienes tracking)
		colorTex->currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	inline static void transitionToPresent(ID3D12GraphicsCommandList* cmdList, D3D12Image* image)
	{
		if (!image || !image->isSwapchainImage_)
			return;

		if (image->currentState_ != D3D12_RESOURCE_STATE_PRESENT)
		{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				image->resource_.Get(),
				image->currentState_,
				D3D12_RESOURCE_STATE_PRESENT
			);
			cmdList->ResourceBarrier(1, &barrier);
			image->currentState_ = D3D12_RESOURCE_STATE_PRESENT;
		}
	}



	CommandBuffer::CommandBuffer(DeviceD3D12* ctx): ctx_(ctx), wrapper_(&ctx->m_ImmediateCommands->acquire())
	{}

	CommandBuffer::~CommandBuffer()
	{}

	void CommandBuffer::cmdBindScissorRect(const ScissorRect& rect)
	{
		const D3D12_RECT scissor{
				.left = static_cast<LONG>(rect.x),
				.top = static_cast<LONG>(rect.y),
				.right = static_cast<LONG>(rect.x + rect.width),
				.bottom = static_cast<LONG>(rect.y + rect.height)
		};
		wrapper_->commandList->RSSetScissorRects(1, &scissor);

	}

	void CommandBuffer::cmdRenderImgui(ImDrawData* drawData, ID3D12DescriptorHeap* pCbvSrvHeap)
	{
		if( !drawData )
			return;
		// Set the viewport and scissor rect.
		wrapper_->commandList->RSSetViewports(1, &ctx_->m_ScreenViewport);
		wrapper_->commandList->RSSetScissorRects(1, &ctx_->m_ScissorRect);
		// Bind the descriptor heap for ImGui
		ID3D12DescriptorHeap* heaps[] = { pCbvSrvHeap };
		wrapper_->commandList->SetDescriptorHeaps(_countof(heaps), heaps);
		ImGui_ImplDX12_RenderDrawData(drawData, wrapper_->commandList.Get());
	
	
	}
	void CommandBuffer::cmdBeginRendering(D3D12Image* colorTex)
	{
		////1. Check if the rendering is already in progress
		_ASSERT(!isRendering_);
		isRendering_ = true;
		//2. transition all the color attachments and depth-stencil attachment

		transitionToColorAttachment(wrapper_->commandList.Get(), colorTex);
	
		//Clear RTV  AND DSV 
		wrapper_->commandList->ClearRenderTargetView(colorTex->getRTV(), ctx_->m_ClearColor, 0, nullptr);
		wrapper_->commandList->ClearDepthStencilView(ctx_->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
		float width = colorTex->width_;
		float height = colorTex->height_;

		const ViewPortState viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, +1.0f };
		const ScissorRect scissor = { 0, 0, width, height };
		// Set Viewport and Scissor
		cmdBindScissorRect(scissor);
		cmdBindViewport(viewport);

		//Get CPU descriptor handle for the color attachment
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = colorTex->getRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = ctx_->DepthStencilView();
		wrapper_->commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

		//tr
	
	
	
	}
	void CommandBuffer::cmdEndRendering()
	{
		// Transition the color attachment to present state
		// This will be load a frameatachment but now its test 

		
		isRendering_ = false;
	}
	void CommandBuffer::cmdBindViewport(const ViewPortState& state)
	{

		const  D3D12_VIEWPORT screenViewport{
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