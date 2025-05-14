//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-13 by alfonsmagd



#include "d3d12_backend.hpp"
#include "DeviceD3D12.hpp"
#include "Platform/D3D12/d3d12_PipelineBuilder.hpp"
#include "Platform/D3D12/d3d12_Buffer.hpp"
#include "Ifnity/Graphics/Utils.hpp"

#include "ShaderBuilding/ShaderBuilder.hpp"
#include "Platform/D3D12/d3d12_constants.hpp"

#include "D3D12MemAlloc.h"





IFNITY_NAMESPACE

namespace D3D12
{


	//------------------------------------------------------------------------------------//
	//  GRAPHICS PIPELINE D3D12                                                          //
	//-------------------------------------------------------------------------------------//
	GraphicsPipeline::GraphicsPipeline( GraphicsPipelineDescription&& desc, DeviceD3D12* dev ): m_Description( std::move( desc ) ), m_DeviceD3D12( dev )
	{
		//Check if m_DeviceD31D12 has been created well
		IFNITY_ASSERT_MSG( m_DeviceD3D12 != nullptr, "DeviceD3D12 is null" );
		IFNITY_ASSERT_MSG( m_DeviceD3D12->m_Device.Get() != nullptr, "ID3D12Device is null" );


	}

	GraphicsPipeline::~GraphicsPipeline()
	{
		//SlotMaps shader destroy 
		ShaderModuleState* mvert = m_DeviceD3D12->slotMapShaderModules_.get( m_shaderVert );
		ShaderModuleState* mps = m_DeviceD3D12->slotMapShaderModules_.get( m_shaderPixel );

		if( mvert )
		{
			mvert->bytecode->Release();
			m_DeviceD3D12->slotMapShaderModules_.destroy( m_shaderVert );
		}
		if( mps )
		{
			mps->bytecode->Release();
			m_DeviceD3D12->slotMapShaderModules_.destroy( m_shaderPixel );
		}

		if( m_PipelineState )
		{
			m_PipelineState.Reset();
			m_PipelineState = nullptr;
		}



	}





	void GraphicsPipeline::BindPipeline( IDevice* device )
	{
		Device* d3Device = dynamic_cast< Device* >(device);
		IFNITY_ASSERT_MSG( d3Device != nullptr, "Device is not a D3D12 Device" );

		// If the pipeline state already exists, set the actual pipeline and return
		if( m_PipelineState )
		{
			IFNITY_LOG( LogApp, INFO, "Pipeline creator" );
			d3Device->setActualPipeline( this );
			return;
		}

		// Retrieve shader module states
		ShaderModuleState* mvert = m_DeviceD3D12->slotMapShaderModules_.get( m_shaderVert );
		ShaderModuleState* mps = m_DeviceD3D12->slotMapShaderModules_.get( m_shaderPixel );
		IFNITY_ASSERT_MSG( mvert && mps, "Shader modules are not valid" );

		// Build the pipeline state
		D3D12PipelineBuilder builder;
		builder.setVS( mvert->bytecode->GetBufferPointer(), mvert->bytecode->GetBufferSize() )
			.setPS( mps->bytecode->GetBufferPointer(), mps->bytecode->GetBufferSize() )
			.setRootSignature( m_DeviceD3D12->m_RootSignature.Get() )
			.setInputLayout( m_rD3D12PipelineState.inputLayout_ )
			.build( m_DeviceD3D12->m_Device.Get(), OUT m_PipelineState.GetAddressOf() );

		d3Device->setActualPipeline( this );



	}



	void GraphicsPipeline::configureVertexAttributes()
	{
		auto& d3dState = m_rD3D12PipelineState;
		const auto& vertexInput = m_Description.vertexInput;

		d3dState.inputElementCount_ = vertexInput.getNumAttributes();
		IFNITY_ASSERT_MSG( d3dState.inputElementCount_ <= rhi::VertexInput::VERTEX_ATTRIBUTES_MAX, "Too many attributes" );

		for( uint32_t i = 0; i < d3dState.inputElementCount_; ++i )
		{
			const auto& attr = vertexInput.attributes[ i ];
			const auto& binding = vertexInput.inputBindings[ attr.binding ];

			D3D12_INPUT_ELEMENT_DESC& element = d3dState.inputElements_[ i ];

			element.SemanticName = rhi::getSemanticName( attr.semantic ); // e.g., "POSITION" 
			element.SemanticIndex = 0; //  not supported yet 
			element.Format = formatToDxgiFormat( attr.format );
			element.InputSlot = attr.binding;
			element.AlignedByteOffset = static_cast< UINT >(attr.offset);
			element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			element.InstanceDataStepRate = 0;

		}

		d3dState.inputLayout_ = {
			.pInputElementDescs = d3dState.inputElements_,
			.NumElements = d3dState.inputElementCount_,
		};


	}






	//--------------------------------------------------------------------//
	//  DEVICE D3D12                                                        //
	//----------------------------------------------------------------------//

	Device::Device( DeviceD3D12* ptr ): m_DeviceD3D12( ptr )
	{
		IFNITY_ASSERT_MSG( m_DeviceD3D12 != nullptr, "DeviceD3D12 is null" );
		//Check if m_DeviceD31D12 has been created well 
		IFNITY_ASSERT_MSG( m_DeviceD3D12->m_Device.Get() != nullptr, "ID3D12Device is null" );
	}



	void Device::setActualPipeline( GraphicsPipeline* pipeline )
	{
		m_DeviceD3D12->actualPipeline_ = pipeline;
	}



	void Device::StartRecording()
	{

		D3D12::CommandBuffer& cmdb = m_DeviceD3D12->acquireCommandBuffer();
		cmdBuffer = std::move( cmdb );

		currentTexture_ = m_DeviceD3D12->getCurrentSwapChainTexture();


		RenderPass renderPass = {
			.color = { {.loadOp = rhi::LoadOp_Clear, .clearColor = { 1.0f, 1.0f, 1.0f, 1.0f } } } };

		Framebuffer framebuffer = { .color = { {.texture = currentTexture_ } } };

		//cmdBuffer.cmdBeginRendering(textback);
		cmdBuffer.cmdBeginRendering( renderPass, framebuffer );


	}

	void Device::StopRecording()
	{
		cmdBuffer.cmdRenderImgui( ImGui::GetDrawData(), m_DeviceD3D12->m_CbvSrvUavHeap.Get() );
		cmdBuffer.cmdEndRendering();
		m_DeviceD3D12->submit( cmdBuffer, currentTexture_ );


	}

	void Device::BindingVertexAttributesBuffer( BufferHandle& bf )
	{
		Buffer* vBuffer = DCAST_BUFFER( bf.get() );
		if( !vBuffer )
		{
			IFNITY_LOG( LogCore, ERROR, "Failed to get D3D12 dynamic cast" );
			return;
		}

		currentVertexBuffer_ = vBuffer->getBufferHandleSM();

	}



	void Device::DrawObject( GraphicsPipelineHandle& pipeline, DrawDescription& desc )
	{
		////Get cmdlist 
		auto& cmdlist = cmdBuffer.wrapper_->commandList;

		pipeline->BindPipeline( this );
		//cmdlist->SetPipelineState();
		//cmdlist->SetGraphicsRootSignature(rootSignature.Get());
		//cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//cmdlist->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		//cmdlist->DrawInstanced(3, 1, 0, 0);

	}






	BufferHandle Device::CreateBuffer( const BufferDescription& desc )
	{
		StorageType storage = desc.storage;

		// Check if the storage type is valid
		if( desc.type == BufferType::NO_DEFINE_BUFFER )
		{
			IFNITY_LOG( LogCore, WARNING, "No define buffer " ); return{};
		}


		// Get heap type. 
		D3D12_HEAP_TYPE heapType = storageTypeToD3D12HeapType( storage );
		D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

		// Get flags and initial state based on buffer type.
		const auto& usageInfo = getD3D12UsageMapping( desc.type );
		D3D12_RESOURCE_FLAGS resourceFlags = usageInfo.resourceFlags;
		initialState = usageInfo.initialState;

		BufferHandleSM bufferHandle = CreateInternalD3D12Buffer( desc,
																 resourceFlags,
																 initialState,
																 heapType,
																 desc.debugName.c_str() );

		//THIS IS ONLY FOR NOW BECAUSE ITS ONLY HOST VISIBLE UPLOAD BUFFER.  [MOVE TO FUNCTION UPLOAD) D3D12 NEXT. 
		if( desc.data )
		{
			// Upload data if the buffer description has it
			D3D12Buffer* buffer = m_DeviceD3D12->slotMapBuffers_.getByIndex( bufferHandle.index() );
			if( !buffer )
			{
				IFNITY_LOG( LogCore, ERROR, "Buffer is null to write " );
				return {};
			}
			buffer->bufferSubData( *m_DeviceD3D12, 0, desc.byteSize, desc.data );
		}

		Buffer* handle = new Buffer( desc, std::move( bufferHandle ) );

		return BufferHandle( handle );
	}

	BufferHandleSM Device::CreateInternalD3D12Buffer( const BufferDescription& desc,
													  D3D12_RESOURCE_FLAGS resourceFlags,
													  D3D12_RESOURCE_STATES initialState,
													  D3D12_HEAP_TYPE heapType,
													  const char* debugName )
	{
		IFNITY_ASSERT_MSG( desc.byteSize > 0, "Buffer size is invalid" );

		//Create the buffer

		D3D12Buffer buffer = {

			.bufferSize_ = desc.byteSize,
			.resourceFlags_ = resourceFlags,
			.bufferType_ = desc.type
		};

		// Describe the buffer
		D3D12_RESOURCE_DESC descbuff = D3D12Buffer::bufferDesc( desc.byteSize, resourceFlags, initialState );

		if( D3D12VMA_ALLOCATOR )
		{
			D3D12MA::ALLOCATION_DESC allocDesc = {};
			allocDesc.HeapType = heapType;

			ThrowIfFailed( m_DeviceD3D12->g_Allocator->CreateResource(
				&allocDesc,
				&descbuff,
				initialState,
				nullptr, // No optimized clear value for buffers
				&buffer.allocation_,
				IID_PPV_ARGS( &buffer.resource_ )
			) );

			//Set the gpu address
			buffer.gpuAddress_ = buffer.resource_->GetGPUVirtualAddress();

			if( debugName )
			{
				std::wstring wdebugName( debugName, debugName + strlen( debugName ) );
				buffer.resource_->SetName( wdebugName.c_str() );
			}


			if( heapType == D3D12_HEAP_TYPE_UPLOAD )
			{
				void* mapped = nullptr;
				buffer.resource_->Map( 0, nullptr, &mapped );
				buffer.mappedPtr_ = mapped;
			}

		}

		BufferHandleSM bufferHandle = m_DeviceD3D12->slotMapBuffers_.create( std::move( buffer ) );
		IFNITY_ASSERT_MSG( bufferHandle.valid(), "Buffer handle is not valid" );
		IFNITY_LOG( LogCore, INFO, "Buffer created: " + std::to_string( bufferHandle.index() ) );

		return bufferHandle;

	}






	GraphicsPipelineHandle Device::CreateGraphicsPipeline( GraphicsPipelineDescription& desc )
	{

		auto* vs = desc.vs;
		auto* fs = desc.ps;
		auto* gs = desc.gs;

		// Get the shader description
		ShaderCreateDescription vsDesc = vs->GetShaderDescription();
		ShaderCreateDescription fsDesc = fs->GetShaderDescription();

		// Resolve the shader directory path
		VFS& vfs = VFS::GetInstance();
		std::string shaderDir = vfs.ResolvePath( "Shaders", "d3d12" );

		// Construct the file paths for _vs.cso and _ps.cso
		std::string vsFilePath = shaderDir + "/" + vsDesc.FileName + "_vs.cso";
		std::string fsFilePath = shaderDir + "/" + fsDesc.FileName + "_ps.cso";

		// Check if the files exist
		bool vsExists = std::filesystem::exists( vsFilePath );
		bool fsExists = std::filesystem::exists( fsFilePath );

		if( !vsExists || !fsExists )
		{
			IFNITY_LOG( LogApp, ERROR, "Shader files not found: " + vsFilePath + " or " + fsFilePath );
			return GraphicsPipelineHandle{};
		}

		// Load the blobs from the files
		IDxcBlob* vsBlob = ShaderCompiler::GetBlobFromFile( vsFilePath );
		IDxcBlob* fsBlob = ShaderCompiler::GetBlobFromFile( fsFilePath );

		if( !vsBlob || !fsBlob )
		{
			IFNITY_LOG( LogApp, ERROR, "Failed to load shader blobs from files: " + vsFilePath + " or " + fsFilePath );
			return GraphicsPipelineHandle{};
		}

		// Log success
		IFNITY_LOG( LogApp, INFO, "Successfully loaded shader blobs: " + vsFilePath + " and " + fsFilePath );

		//Set the ShaderModuleState with new GraphicsPipelineState

		GraphicsPipeline* pipeline = new GraphicsPipeline( std::move( desc ), m_DeviceD3D12 );

		// Create the StateShaderModule 
		ShaderModuleState vsState = { .bytecode = vsBlob };
		ShaderModuleState fsState = { .bytecode = fsBlob };


		//Add the StatheShaderModule to SlotMap  
		pipeline->m_shaderVert = m_DeviceD3D12->slotMapShaderModules_.create( std::move( vsState ) );
		pipeline->m_shaderPixel = m_DeviceD3D12->slotMapShaderModules_.create( std::move( fsState ) );
		pipeline->configureVertexAttributes();



		return GraphicsPipelineHandle{ pipeline };
	}














}

IFNITY_END_NAMESPACE