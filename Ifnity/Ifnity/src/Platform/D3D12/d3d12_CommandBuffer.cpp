//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-24 by alfonsmagd







#include "d3d12_CommandBuffer.hpp"
#include "d3d12_ImmediateCommands.hpp"

#include "../Windows/DeviceD3D12.hpp"


#include "Ifnity/Graphics/Interfaces/IGraphicsPipeline.hpp"

IFNITY_NAMESPACE

namespace D3D12
{


	// Helper function to transition a color attachment
	inline static void transitionToColorAttachment( ID3D12GraphicsCommandList* cmdList, D3D12Image* colorTex )
	{
		if( !colorTex )
			return;

		if( colorTex->isDepthFormat_ || colorTex->isStencilFormat_ )
		{
			IFNITY_LOG( LogCore, ERROR, "Color attachments cannot have depth/stencil formats" );
			return;
		}

		if( colorTex->format_ == DXGI_FORMAT_UNKNOWN )
		{
			IFNITY_LOG( LogCore, ERROR, "Invalid color attachment format" );
			return;
		}

		// Apply barrier
		const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			colorTex->resource_.Get(),
			colorTex->currentState_,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		cmdList->ResourceBarrier( 1, &barrier );

		// Update current state (si mantienes tracking)
		colorTex->currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	inline static void transitionToPresent( ID3D12GraphicsCommandList* cmdList, D3D12Image* image )
	{
		if( !image || !image->isSwapchainImage_ )
			return;

		if( image->currentState_ != D3D12_RESOURCE_STATE_PRESENT )
		{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				image->resource_.Get(),
				image->currentState_,
				D3D12_RESOURCE_STATE_PRESENT
			);
			cmdList->ResourceBarrier( 1, &barrier );
			image->currentState_ = D3D12_RESOURCE_STATE_PRESENT;
		}
	}



	CommandBuffer::CommandBuffer( DeviceD3D12* ctx ): ctx_( ctx ), wrapper_( &ctx->m_ImmediateCommands->acquire() )
	{}

	CommandBuffer::~CommandBuffer()
	{}

	void CommandBuffer::cmdBindScissorRect( const ScissorRect& rect )
	{
		const D3D12_RECT scissor{
				.left = static_cast< LONG >(rect.x),
				.top = static_cast< LONG >(rect.y),
				.right = static_cast< LONG >(rect.x + rect.width),
				.bottom = static_cast< LONG >(rect.y + rect.height)
		};
		wrapper_->commandList->RSSetScissorRects( 1, &scissor );

	}

	void CommandBuffer::cmdRenderImgui( ImDrawData* drawData, ID3D12DescriptorHeap* pCbvSrvHeap )
	{
		if( !drawData )
			return;
		// Set the viewport and scissor rect.
		wrapper_->commandList->RSSetViewports( 1, &ctx_->m_ScreenViewport );
		wrapper_->commandList->RSSetScissorRects( 1, &ctx_->m_ScissorRect );
		// Bind the descriptor heap for ImGui
		ID3D12DescriptorHeap* heaps[] = { pCbvSrvHeap };
		wrapper_->commandList->SetDescriptorHeaps( _countof( heaps ), heaps );
		ImGui_ImplDX12_RenderDrawData( drawData, wrapper_->commandList.Get() );


	}
	void CommandBuffer::cmdBindVertexBuffer( BufferHandleSM& bf, uint32_t stride, uint32_t offset )
	{
		if( !IFNITY_VERIFY( !bf.empty() ) )
		{
			return;
		}

		D3D12Buffer* buf = ctx_->slotMapBuffers_.get( bf );

		IFNITY_ASSERT( buf->bufferType_ == BufferType::VERTEX_BUFFER, "Buffer is not a vertex buffer" );

		//get vertexbufferview
		D3D12_VERTEX_BUFFER_VIEW m_vbview = buf->getVertexBufferView( stride );

		wrapper_->commandList->IASetVertexBuffers( 0, 1, &m_vbview );

	}
	void CommandBuffer::cmdBindIndexBuffer( BufferHandleSM& bf, uint32_t offset )
	{
		if( !IFNITY_VERIFY( !bf.empty() ) )
		{
			return;
		}
		D3D12Buffer* buf = ctx_->slotMapBuffers_.get( bf );
		IFNITY_ASSERT( buf->bufferType_ == BufferType::INDEX_BUFFER, "Buffer is not a index buffer d3d12" );

		D3D12_INDEX_BUFFER_VIEW m_ibview = buf->getIndexBufferView();

		wrapper_->commandList->IASetIndexBuffer( &m_ibview );









	}
	void CommandBuffer::cmdBindRenderPipeline( GraphicsPipeline* pipeline )
	{
		if( !pipeline )
		{
			IFNITY_LOG( LogCore, ERROR, "Pipeline is null" );
			return;
		}

		ID3D12PipelineState* d3d12Pipeline = pipeline->getPipelineState();
		if( !d3d12Pipeline )
		{
			IFNITY_LOG( LogCore, ERROR, "Pipeline state is null after bind" );
			return;
		}

		if( d3d12Pipeline != lastPipelineBound_ )
		{

			
			ID3D12DescriptorHeap* heaps[] = { ctx_->m_BindlessHeap.Get() };

			wrapper_->commandList->SetDescriptorHeaps(ARRAY_NUM_ELEMENTS(heaps), heaps);
			lastPipelineBound_ = d3d12Pipeline;
			wrapper_->commandList->SetPipelineState( d3d12Pipeline );
			wrapper_->commandList->SetGraphicsRootSignature( ctx_->m_RootSignature.Get() );

		}
		else
		{
			IFNITY_LOG( LogCore, WARNING, "Pipeline already bound" );
		}

	}

	void CommandBuffer::cmdSetPrimitiveTopology( rhi::PrimitiveType primitiveType )
	{
		D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		switch( primitiveType )
		{
			case rhi::PrimitiveType::TriangleList:
				topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				break;
			case rhi::PrimitiveType::LineList:
				topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
				break;
			case rhi::PrimitiveType::PointList:
				topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
				break;
			default:
				IFNITY_LOG( LogCore, ERROR, "Unsupported primitive type" );
				break;
		}
		wrapper_->commandList->IASetPrimitiveTopology( topology );

	}

	void CommandBuffer::cmdDrawIndexedIndirect(BufferHandleSM indirectBuffer,
												size_t indirectBufferOffset,
												uint32_t drawCount,
												uint32_t stride)
	{
		if( indirectBuffer.empty() || drawCount == 0 )
		{
			IFNITY_LOG( LogCore, ERROR, "Indirect buffer is empty or draw count is zero" );
			return;
		}
		D3D12Buffer* buf = ctx_->slotMapBuffers_.get( indirectBuffer );
		if( !buf || buf->bufferType_ != BufferType::INDIRECT_BUFFER )
		{
			IFNITY_LOG( LogCore, ERROR, "Invalid indirect buffer" );
			return;
		}

		//Check if we have a CommandSignature created 
		if( !ctx_->m_CommandSignature )
		{
			IFNITY_LOG( LogCore, WARNING , "Indirect command signature is not set, try to build " );

			ctx_->CreateCommandSignature( ctx_->m_Device.Get(), 
										  ctx_->m_CommandSignature.GetAddressOf() ,
										  ctx_->m_RootSignature.Get());
			if( !ctx_->m_CommandSignature )
			{
				IFNITY_LOG( LogCore, ERROR, "Failed to create command signature for indirect draw" );
				return;
			}
			else
			{
				IFNITY_LOG( LogCore, INFO, "Indirect command signature created successfully" );
			}
		}

		//Execute the indirect draw command
		wrapper_->commandList->ExecuteIndirect(
			ctx_->m_CommandSignature.Get(),
			drawCount, // Number of draws
			buf->resource_.Get(), // Indirect buffer
			indirectBufferOffset, // Offset in the indirect buffer
			nullptr, // Argument buffer (not used)
			0 // Argument buffer offset (not used)
		);
		



	}



	void CommandBuffer::cmdDraw( DrawModeUse drawMode,
								 uint32_t vertexCount,
								 uint32_t instanceCount,
								 uint32_t firstVertex,
								 uint32_t baseInstance )
	{
		if( vertexCount == 0 ) return;

		if( drawMode == DRAW )
		{
			// Dibujo sin ndice
			wrapper_->commandList->DrawInstanced(
				vertexCount,     // VertexCountPerInstance
				instanceCount,   // InstanceCount
				firstVertex,     // StartVertexLocation
				baseInstance     // StartInstanceLocation
			);
		}
		else if( drawMode == DRAW_INDEXED )
		{
			// Dibujo con ndice
			wrapper_->commandList->DrawIndexedInstanced(
				vertexCount,     // IndexCountPerInstance
				instanceCount,   // InstanceCount
				firstVertex,     // StartIndexLocation
				baseInstance,    // BaseVertexLocation
				0                // StartInstanceLocation
			);
		}
		else
		{
			IFNITY_LOG( LogCore, ERROR, "Unsupported draw mode" );
			return;
		}
	}

	void CommandBuffer::cmdBeginRendering( D3D12Image* colorTex )
	{
		// 1. Check if rendering is already in progress
		_ASSERT( !isRendering_ );
		isRendering_ = true;


		ID3D12GraphicsCommandList4* cmdList = wrapper_->commandList.Get();

		// 2. Transition to RENDER_TARGET
		transitionToColorAttachment( cmdList, colorTex );

		// 3. Setup render target description
		D3D12_RENDER_PASS_RENDER_TARGET_DESC rtDesc = {};
		rtDesc.cpuDescriptor = colorTex->getRTV();
		rtDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		rtDesc.BeginningAccess.Clear.ClearValue.Color[ 0 ] = ctx_->m_ClearColor[ 0 ];
		rtDesc.BeginningAccess.Clear.ClearValue.Color[ 1 ] = ctx_->m_ClearColor[ 1 ];
		rtDesc.BeginningAccess.Clear.ClearValue.Color[ 2 ] = ctx_->m_ClearColor[ 2 ];
		rtDesc.BeginningAccess.Clear.ClearValue.Color[ 3 ] = ctx_->m_ClearColor[ 3 ];
		rtDesc.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

		// 4. Setup depth-stencil
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.cpuDescriptor = ctx_->DepthStencilView();
		dsDesc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		dsDesc.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = 1.0f;
		dsDesc.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Stencil = 0;
		dsDesc.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
		dsDesc.StencilBeginningAccess = dsDesc.DepthBeginningAccess;
		dsDesc.StencilEndingAccess = dsDesc.DepthEndingAccess;

		// 5. Begin render pass
		cmdList->BeginRenderPass( 1,
								  &rtDesc,
								  &dsDesc,
								  D3D12_RENDER_PASS_FLAG_NONE );

		// 6. Set viewport and scissor
		ViewPortState viewport = { 0.0f, 0.0f, colorTex->width_, colorTex->height_, 0.0f, 1.0f };
		ScissorRect scissor = { 0, 0, colorTex->width_, colorTex->height_ };
		cmdBindViewport( viewport );
		cmdBindScissorRect( scissor );



	}
	void CommandBuffer::cmdBeginRendering( const RenderPass renderPass, Framebuffer& fb )
	{
		uint32_t width = 0;
		uint32_t height = 0;

		// 1. Check if rendering is already in progress
		_ASSERT( !isRendering_ );
		isRendering_ = true;
		ID3D12GraphicsCommandList4* cmdList = wrapper_->commandList.Get();


		const uint32_t numFbColorAttachments = fb.getNumColorAttachments();
		const uint32_t numPassColorAttachments = renderPass.getNumColorAttachments();

		_ASSERT( numPassColorAttachments == numFbColorAttachments );

		auto& colorAttachment = fb.color[ 0 ].texture;
		if( colorAttachment )
		{
			D3D12Image* colorTex = ctx_->slotMapTextures_.get( colorAttachment );
			width = colorTex->width_;
			height = colorTex->height_;
		}


		//3. New structures are used to define the attachments used in dynamic rendering
		D3D12_RENDER_PASS_RENDER_TARGET_DESC colorAttachments[ _MAX_COLOR_ATACHMENT_ ] = {};

		// 2. Transition to RENDER_TARGET // depthbuffer nots necessary the drive does for us. 
		for( uint32_t i = 0; i != numFbColorAttachments; i++ )
		{
			if( const auto handle = fb.color[ i ].texture )
			{
				D3D12Image* colorTex = ctx_->slotMapTextures_.get( handle );

				transitionToColorAttachment( cmdList, colorTex );
				colorAttachments[ i ] = {
				   .cpuDescriptor = colorTex->getRTV(),
				   .BeginningAccess =
					   {.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR,
						 .Clear =
							 {.ClearValue =
								   {.Color =
										 { ctx_->m_ClearColor[ 0 ],
										   ctx_->m_ClearColor[ 1 ],
										   ctx_->m_ClearColor[ 2 ],
										   ctx_->m_ClearColor[ 3 ] } } } },
				   .EndingAccess =
					   {.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE } };
			}

		}
		const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC dsDesc = {
			.cpuDescriptor = ctx_->DepthStencilView(),
			 .DepthBeginningAccess =
				 {.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR,
				   .Clear =
					   {.ClearValue =
							 {.DepthStencil =
								   { 1.0f, 0 } } } },
			 .StencilBeginningAccess =
				 {.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR,
				   .Clear =
					   {.ClearValue =
							 {.DepthStencil =
								   { 1.0f, 0 } } } },
			.DepthEndingAccess =
			{.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE },
			 .StencilEndingAccess = dsDesc.DepthEndingAccess };

		// 5. Begin render pass
		cmdList->BeginRenderPass( numFbColorAttachments,
								  &colorAttachments[ 0 ],      //if you put colorAttachments, warning C6001
								  &dsDesc,
								  D3D12_RENDER_PASS_FLAG_NONE );

		// 6. Set viewport and scissor
		ViewPortState viewport = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
		ScissorRect scissor = { 0, 0, width, height };
		cmdBindViewport( viewport );
		cmdBindScissorRect( scissor );


	}
	void CommandBuffer::cmdEndRendering()
	{
		// Transition the color attachment to present state
		// This will be load a frameatachment but now its test 
		ID3D12GraphicsCommandList4* cmdList = wrapper_->commandList.Get();
		cmdList->EndRenderPass();
		isRendering_ = false;
	}
	void CommandBuffer::cmdBindViewport( const ViewPortState& state )
	{

		const  D3D12_VIEWPORT screenViewport{
			 .TopLeftX = static_cast< float >(state.x),
			 .TopLeftY = static_cast< float >(state.y),
			 .Width = static_cast< float >(state.width),
			 .Height = static_cast< float >(state.height),
			 .MinDepth = static_cast< float >(state.minDepth),
			 .MaxDepth = static_cast< float >(state.maxDepth)

		};
		wrapper_->commandList->RSSetViewports( 1, &screenViewport );

	}


	void CommandBuffer::cmdPushConstants( const void* data, size_t size, size_t offset)
	{
		//Check size multiple x4 

		if( !data || size == 0 )
		{
			IFNITY_LOG( LogCore, INFO, "Invalid push constant data" );
			return;
		}
		//Check in compiletime with consteval size ==4 

		IFNITY_ASSERT( size % sizeof( uint32_t ) == 0, "Push constant size must be multiple of 4" );

		wrapper_->commandList->SetGraphicsRoot32BitConstants( DeviceD3D12::kBinding_RootConstant,
															  static_cast< UINT >(size / sizeof( uint32_t )),
															  data,
															  static_cast< UINT >(offset) );
	}

}
IFNITY_END_NAMESPACE