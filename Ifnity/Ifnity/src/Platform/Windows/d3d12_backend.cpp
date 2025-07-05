//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-24 by alfonsmagd






#include "d3d12_backend.hpp"
#include "DeviceD3D12.hpp"
#include "Platform/D3D12/d3d12_PipelineBuilder.hpp"
#include "Platform/D3D12/d3d12_Buffer.hpp"
#include "Ifnity/Graphics/Utils.hpp"

#include "ShaderBuilding/ShaderBuilder.hpp"
#include "Platform/D3D12/d3d12_constants.hpp"


#include "D3D12MemAlloc.h"
#include <span>





IFNITY_NAMESPACE

namespace D3D12
{

	bool validateImageLimits( D3D12_RESOURCE_DIMENSION dimension,
							  UINT sampleCount,
							  UINT width,
							  UINT height,
							  UINT depth )
	{
		// MSAA solo vlido en 2D
		if( sampleCount > 1 && dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D )
		{
			IFNITY_LOG( LogCore, ERROR, "Sample count > 1 only supported for 2D textures" );
			return false;
		}

		if( dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D )
		{
			if( !(width <= D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION &&
				   height <= D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION) )
			{
				IFNITY_LOG( LogCore, ERROR, "2D texture size exceeded" );
				return false;
			}
		}
		else if( dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D )
		{
			if( !(width <= D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
				   height <= D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
				   depth <= D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) )
			{
				IFNITY_LOG( LogCore, ERROR, "3D texture size exceeded" );
				return false;
			}
			if( sampleCount > 1 )
			{
				IFNITY_LOG( LogCore, ERROR, "MSAA is not supported for 3D textures" );
				return false;
			}
		}
		else
		{
			IFNITY_LOG( LogCore, ERROR, "Unsupported texture type" );
			return false;
		}

		return true;
	}



	//------------------------------------------------------------------------------------//
	//  BUFFER  D3D12                                                          //
	//-------------------------------------------------------------------------------------//



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
			/*IFNITY_LOG( LogApp, INFO, "Pipeline creator" );*/
			d3Device->setActualPipeline( this );
			return;
		}

		// Retrieve shader module states
		ShaderModuleState* mvert = m_DeviceD3D12->slotMapShaderModules_.get( m_shaderVert );
		ShaderModuleState* mps = m_DeviceD3D12->slotMapShaderModules_.get( m_shaderPixel );
		IFNITY_ASSERT_MSG( mvert && mps, "Shader modules are not valid" );
		const auto& rasterState = m_Description.rasterizationState;
		const auto& depthState = m_Description.renderState;
		const BlendState& blendstate = depthState.blendState;


		D3D12_RENDER_TARGET_BLEND_DESC descRenderTarget{};

		if( !blendstate.blendEnable )
		{
			descRenderTarget.BlendEnable = FALSE;
			descRenderTarget.SrcBlend = D3D12_BLEND_ONE;
			descRenderTarget.DestBlend = D3D12_BLEND_ZERO;
			descRenderTarget.BlendOp = D3D12_BLEND_OP_ADD;
			descRenderTarget.SrcBlendAlpha = D3D12_BLEND_ONE;
			descRenderTarget.DestBlendAlpha = D3D12_BLEND_ZERO;
			descRenderTarget.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		}
		else
		{
			descRenderTarget.BlendEnable = TRUE;
			descRenderTarget.SrcBlend = ConvertToD3D12Blend( blendstate.srcColorBlendFactor );
			descRenderTarget.DestBlend = ConvertToD3D12Blend( blendstate.dstColorBlendFactor );
			descRenderTarget.BlendOp = ConvertToD3D12BlendOp( blendstate.colorBlendOp );
			descRenderTarget.SrcBlendAlpha = ConvertToD3D12Blend( blendstate.srcAlphaBlendFactor );
			descRenderTarget.DestBlendAlpha = ConvertToD3D12Blend( blendstate.dstAlphaBlendFactor );
			descRenderTarget.BlendOpAlpha = ConvertToD3D12BlendOp( blendstate.alphaBlendOp );
		}
		descRenderTarget.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[ 0 ] = descRenderTarget;


		// Build the pipeline state // for now its simple. 
		D3D12PipelineBuilder builder{};
		builder.setVS( mvert->bytecode->GetBufferPointer(), mvert->bytecode->GetBufferSize() )
			.setPS( mps->bytecode->GetBufferPointer(), mps->bytecode->GetBufferSize() )
			.setBlend( blendDesc )
			.setCullMode( convertToDxCullMode( rasterState.cullMode ) )
			.setFillMode( convertToDxFillMode( rasterState.polygonMode ) )
			.setFrontCounterClockwise( convertToDxFrontCounterClockwise( rasterState.frontFace ) )
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
			element.InstanceDataStepRate = 0; // not supported yet.

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


	D3D12::GraphicsPipeline* Device::getActualPipeline() const
	{
		if( m_DeviceD3D12->actualPipeline_ )
		{
			return m_DeviceD3D12->actualPipeline_;
		}
		else
		{
			IFNITY_LOG(LogApp, ERROR, "Actual pipeline is null");
			return nullptr;
		}
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

		//Set own pipeline
		if( m_DeviceD3D12->actualPipeline_ )
		{
			ID3D12PipelineState* d3d12Pipeline = m_DeviceD3D12->actualPipeline_->getPipelineState();

			if( d3d12Pipeline )
			{
				cmdBuffer.cmdBindRenderPipeline( m_DeviceD3D12->actualPipeline_ );
			}
			else
			{
				IFNITY_LOG( LogCore, ERROR, "Pipeline state is null" );
				return;
			}
		}

	}

	void Device::StopRecording()
	{
		cmdBuffer.cmdRenderImgui( ImGui::GetDrawData(), m_DeviceD3D12->m_ImguiHeap.Get() );
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

	void Device::BindingIndexBuffer( BufferHandle& bf )
	{
		Buffer* iBuffer = DCAST_BUFFER( bf.get() );
		if( !iBuffer )
		{
			IFNITY_LOG( LogCore, ERROR, "Failed to get D3D12 dynamic cast" );
			return;
		}
		currentIndexBuffer_ = iBuffer->getBufferHandleSM();
	}




	MeshObjectHandle Device::CreateMeshObject(const MeshObjectDescription& desc)
	{// Not implemented yet//Check if MeshData its valid ? 
		if( desc.meshData.indexData_.empty() || desc.meshData.vertexData_.empty() )
		{
			IFNITY_LOG(LogApp, ERROR, "MeshData its invalid, are you sure that mesh Object Desc has data? , you have to build, or use a IMeshObjectBuilder");
			return nullptr;
		}
		// Check if ist not a large mesh 


		//For now its a largeMesh 
		else if( desc.isLargeMesh )
		{
			//Create a MeshObject with the data. 

			/*MeshObject* mesh = new MeshObject(&desc.meshFileHeader, desc.meshData.meshes_.data(), desc.meshData.indexData_.data(), desc.meshData.vertexData_.data(), this);*/
			MeshObject* mesh = new MeshObject(std::move(const_cast<MeshObjectDescription&>(desc)), this);

			return MeshObjectHandle(mesh);
		}


		return nullptr;

	}

	MeshObjectHandle Device::CreateMeshObject(const MeshObjectDescription& desc, IMeshDataBuilder* meshbuilder)
	{
		if( meshbuilder )
		{
			meshbuilder->buildMeshData(const_cast<MeshObjectDescription&>(desc));
			return CreateMeshObject(desc);
		}
		else
		{
			IFNITY_LOG(LogApp, ERROR, "MeshDataBuilder its invalid");
			return nullptr;
		}
	}

	SceneObjectHandler Device::CreateSceneObject(const char* meshes, const char* scene, const char* materials)
	{
		//Create a SceneObject with the data. 
		SceneObject* sceneObject = new SceneObject(meshes, scene, materials);
		return SceneObjectHandler(sceneObject);


	}

	MeshObjectHandle Device::CreateMeshObjectFromScene(const SceneObjectHandler& scene)
	{
		// Not implemented yet

		MeshObject* mesh = new MeshObject(scene, this);
		return MeshObjectHandle(mesh);
	}





	void Device::DrawObject( GraphicsPipelineHandle& pipeline, DrawDescription& desc )
	{
		//Changes the DepthState like vulkan en runtime its not posible in D3D12
		// the solution its to create differents pipelines state and only changin the PSO in runtime.
		////Get cmdlist 
		GraphicsPipeline* pi = dynamic_cast< GraphicsPipeline* >(pipeline.get());
		if( !pi )
		{
			IFNITY_LOG( LogCore, ERROR, "Failed to get D3D12 dynamic cast" );
			return;
		}

		//Get Rasterize state 
		const RasterizationState& rasterState = pipeline->GetGraphicsPipelineDesc().rasterizationState;

		pipeline->BindPipeline( this );
		cmdBuffer.cmdBindRenderPipeline( pi );

		cmdBuffer.cmdSetPrimitiveTopology( rasterState.primitiveType );

		if( currentIndexBuffer_ )
		{
			cmdBuffer.cmdBindIndexBuffer( currentIndexBuffer_ );
		}
		if( currentVertexBuffer_ )
		{
			cmdBuffer.cmdBindVertexBuffer( currentVertexBuffer_ );
		}

		cmdBuffer.cmdPushConstants(pushConstants.data,
									pushConstants.size,
									pushConstants.offset);



		cmdBuffer.cmdDraw( desc.drawMode, desc.size, 1 );

	}


	TextureHandle Device::CreateTexture( TextureDescription& desc )
	{

		using namespace rhi;

		//For simplify 
		TextureDescription texdesc( desc );
		auto& ctx_ = *m_DeviceD3D12;

		//Get the format value in this case will check only if the format is valid or depthformat 
		if( !validateTextureDescription( texdesc ) )
		{
			IFNITY_LOG( LogCore, ERROR, "Texture description is invalid" );
			return {};
		}

		DXGI_FORMAT format = formatToDxgiFormat( texdesc.format );

		D3D12_RESOURCE_FLAGS usageFlags = getImageUsageFlags( texdesc );


		const bool hasDebugName = texdesc.debugName.size() > 0;

		//For now we force numLayers to 1 
		D3D12_RESOURCE_DIMENSION resourceDim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		D3D12_SRV_DIMENSION srvDim = D3D12_SRV_DIMENSION_TEXTURE2D;
		DXGI_SAMPLE_DESC sampleDesc = {};
		UINT sampleCount = 1;
		UINT arraySize = 1;
		uint32_t numLayers = 1;


		if( !IFNITY_VERIFY( validateImageLimits( resourceDim,
												 sampleCount,
												 texdesc.dimensions.width,
												 texdesc.dimensions.height,
												 texdesc.dimensions.depth ) ) )
		{
			IFNITY_LOG( LogCore, ERROR, "Texture dimension is invalidate checking after validate Image limits." );
			return {};
		}
		const auto texdim = texdesc.dimensions;

		IFNITY_ASSERT_MSG( texdesc.mipLevels > 0, "The image must contain at least one mip-level" );
		IFNITY_ASSERT_MSG( numLayers > 0, "The image must contain at least one layer" );
		IFNITY_ASSERT_MSG( numLayers > 0, "The image must contain at least one sample" );
		IFNITY_ASSERT( texdim.width > 0 );
		IFNITY_ASSERT( texdim.height > 0 );
		IFNITY_ASSERT( texdim.depth > 0 );

		D3D12_RESOURCE_DESC textureDesc = {};

		switch( texdesc.dimension )
		{
			case TextureType::TEXTURE2D:
				resourceDim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				srvDim = D3D12_SRV_DIMENSION_TEXTURE2D;
				sampleDesc = getD3D12SampleDesc( sampleCount );
				arraySize = 1;
				textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
					format,
					static_cast< UINT64 >(texdim.width),
					static_cast< UINT >(texdim.height),
					static_cast< UINT16 >(arraySize),
					texdesc.mipLevels,
					sampleDesc.Count,
					sampleDesc.Quality,
					usageFlags );
				break;

			case TextureType::TEXTURECUBE:
				IFNITY_LOG( LogCore, ERROR, "Cubemaps are not supported yet" );
				break;
			case TextureType::TEXTURE3D:
				IFNITY_LOG( LogCore, ERROR, "3D textures are not supported yet" );
				break;


			default:
				IFNITY_LOG( LogCore, ERROR, "Unsupported texture type" );

		}

		//Create D3D12Image after validation 
		D3D12Image image = {};
		image.usageFlags_ = usageFlags;
		image.type_ = resourceDim;
		image.width_ = texdim.width;
		image.height_ = texdim.height;
		image.isDepthFormat_ = D3D12Image::isDepthFormat( format );
		image.format_ = format;
		image.desc_ = textureDesc;


		// 2. allocateDesc D3D12MA and create resource. 
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		ThrowIfFailed( m_DeviceD3D12->g_Allocator->CreateResource(
			&allocDesc,
			&image.desc_,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, // No optimized clear value for buffers
			&image.allocation_,
			IID_PPV_ARGS( OUT image.resource_.GetAddressOf() )
		) );

		std::string debugNameImage = "Image: " + texdesc.debugName;
		DEBUG_D3D12_NAME( debugNameImage, image.resource_ );


		TextureHandleSM textHanlde = ctx_.slotMapTextures_.create( std::move( image ) );
		HolderTextureSM holder = makeHolder( &ctx_, textHanlde );

		uint32_t index = textHanlde.index();

		//This process is only for get SRV offset based in bindless. Its important ensure that SRV has offset based in index bindless. 
		//Get again the image.

		D3D12Image* imageHandle = ctx_.slotMapTextures_.get( textHanlde );
		IFNITY_ASSERT_MSG( imageHandle != nullptr, "Image handle is null error ?? stranger please call me" );

		//Descriptor preparer.


		imageHandle->indexSrv_ = index + DeviceD3D12::START_SLOT_TEXTURES;
		imageHandle->descriptorHandle_.srvHandle = ctx_.AllocateSRV( index + DeviceD3D12::START_SLOT_TEXTURES );


		ctx_.m_Device->CreateShaderResourceView( imageHandle->resource_.Get(),
												 nullptr,
												 imageHandle->descriptorHandle_.srvHandle );



		TextureHandle texture = std::make_shared<Texture>( texdesc, std::move( holder ) );


		if( texdesc.data )
		{
			upload( textHanlde, texdesc.mipLevels, desc.data );
		}


		return texture;


	}



	BufferHandle Device::CreateBuffer( const BufferDescription& desc )
	{
		StorageType storage = desc.storage;


		if( desc.type == BufferType::CONSTANT_BUFFER )
		{
			IFNITY_LOG( LogCore, INFO, "Constant buffer is managed inside D3D12. " );
			Buffer* buff = new Buffer( desc );
			return BufferHandle( buff );

		}

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

		HolderBufferSM buffer = CreateInternalD3D12Buffer( desc,
														   resourceFlags,
														   initialState,
														   heapType,
														   desc.debugName.c_str() );

		//THIS IS ONLY FOR NOW BECAUSE ITS ONLY HOST VISIBLE UPLOAD BUFFER.  [MOVE TO FUNCTION UPLOAD) D3D12 NEXT. 
		if( desc.data )
		{
			upload( *buffer, desc.data, desc.byteSize, desc.offset );
		}

		Buffer* handle = new Buffer( desc, std::move( buffer ) );

		return BufferHandle( handle );
	}

	HolderBufferSM Device::CreateInternalD3D12Buffer( const BufferDescription& desc,
													  D3D12_RESOURCE_FLAGS resourceFlags,
													  D3D12_RESOURCE_STATES initialState,
													  D3D12_HEAP_TYPE heapType,
													  const char* debugName )
	{
		IFNITY_ASSERT_MSG( desc.byteSize > 0, "Buffer size is invalid" );

		//Create the buffer

		D3D12Buffer buffer = {

			.bufferSize_ = desc.byteSize,
			.bufferStride_ = desc.strideSize,
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
				buffer.allocation_->SetName( wdebugName.c_str() );
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
		
		IFNITY_LOG( LogCore, INFO, "Buffer created: " + std::string(debugName) + std::to_string( bufferHandle.index() ) );

		// Crear SRV si es STORAGE_BUFFER
		if( desc.type == BufferType::STORAGE_BUFFER )
		{
			uint32_t bindlessIndex = bufferHandle.index();

			if( bindlessIndex >= DeviceD3D12::MAX_INDEX_SLOT_BUFFERS )
			{
				IFNITY_LOG( LogCore, ERROR, "Bindless index out of range" );
				return {};
			}

			// Get the buffer from the slot map
			D3D12Buffer* finalBuffer = m_DeviceD3D12->slotMapBuffers_.get( bufferHandle );
			IFNITY_ASSERT_MSG( finalBuffer != nullptr, "Final buffer is null ERROR creating buffer or calling it " );


			finalBuffer->srvHandle = m_DeviceD3D12->AllocateSRV( bindlessIndex );

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = desc.strideSize > 0 ? desc.byteSize / desc.strideSize : 1.0 ;
			srvDesc.Buffer.StructureByteStride = desc.strideSize;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

			m_DeviceD3D12->m_Device->CreateShaderResourceView(
				finalBuffer->resource_.Get(),
				&srvDesc,
				finalBuffer->srvHandle
			);
		}

		return makeHolder( m_DeviceD3D12, bufferHandle );


	}





	void Device::upload( D3D12Buffer* buffer, const void* data, size_t size, uint32_t offset )
	{

		//Previos check if the buffer is null and check it 
		if( !data )
		{
			IFNITY_LOG( LogCore, ERROR, "Data is null to upload " );
			return;
		}

		IFNITY_ASSERT_MSG( size, "Data size should be non-zero" );


		if( !buffer )
		{
			IFNITY_LOG( LogCore, ERROR, "Buffer is null to upload " );
			return;
		}

		if( !IFNITY_VERIFY( offset + size <= buffer->bufferSize_ ) )
		{
			IFNITY_LOG( LogCore, ERROR, "Buffer is enough size " );
			return;
		}

		//Lets to staginDevice to upload data 
		m_DeviceD3D12->stagingDevice_->bufferSubData( *buffer, offset, size, data );


	}

	void Device::upload( TextureHandleSM handle, uint32_t miplevel, const void* data )
	{

		auto& ctx_ = *m_DeviceD3D12;

		//Previos check if the buffer is null and check it 
		if( !data )
		{
			IFNITY_LOG( LogCore, ERROR, "Data is null to upload " );
			return;
		}
		IFNITY_ASSERT_MSG( data, "Data size should be non-zero" );

		//Get the image from the slotmap
		D3D12Image* image = m_DeviceD3D12->slotMapTextures_.get( handle );
		if( !image )
		{
			IFNITY_LOG( LogCore, ERROR, "Buffer is null to upload " );
			return;
		}


		// 4. Calcular el layout de subrecursos
		UINT64 uploadBufferSize = 0;
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts( miplevel );
		std::vector<UINT> numRows( miplevel );
		std::vector<UINT64> rowSizes( miplevel );

		ctx_.m_Device->GetCopyableFootprints(
			&image->desc_,
			0,
			1,
			0,
			layouts.data(),
			numRows.data(),
			rowSizes.data(),
			&uploadBufferSize );

		// 5. Crear buffer intermedio
		CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer( uploadBufferSize );

		D3D12MA::ALLOCATION_DESC uploadAllocDesc = {};
		uploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		ComPtr<ID3D12Resource> uploadBuffer;
		D3D12MA::Allocation* uploadAlloc = nullptr;
		ctx_.g_Allocator->CreateResource( &uploadAllocDesc,
										  &uploadDesc,
										  D3D12_RESOURCE_STATE_GENERIC_READ,
										  nullptr,
										  &uploadAlloc,
										  IID_PPV_ARGS( OUT & uploadBuffer ) );

		// 6. Preparar estructura para UpdateSubresources
		std::vector<D3D12_SUBRESOURCE_DATA> subresources( miplevel );
		const uint8_t* basePtr = reinterpret_cast< const uint8_t* >(data); // only chunk memory for now 
		UINT offset = 0;
		for( UINT i = 0; i < miplevel; ++i )
		{
			subresources[ i ].pData = basePtr + offset;
			subresources[ i ].RowPitch = rowSizes[ i ];
			subresources[ i ].SlicePitch = rowSizes[ i ] * numRows[ i ];

			offset += static_cast< UINT >( subresources[ i ].SlicePitch );
		}

		const D3D12ImmediateCommands::CommandListWrapper&  wrapper = ctx_.m_ImmediateCommands->acquire();


		UpdateSubresources( wrapper.commandList.Get(),
							image->resource_.Get(),
							uploadBuffer.Get(),
							0,
							0,
							static_cast< UINT >(miplevel),
							subresources.data() );

		// 7. Transition the image to the appropriate state
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			image->resource_.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE );

		wrapper.commandList->ResourceBarrier( 1, &barrier );

		//Submit the command list

		SubmitHandle submithandle = ctx_.m_ImmediateCommands->submit( wrapper );
		//Defer the destruction of the staging buffer
		ctx_.addDeferredTask(
			std::packaged_task<void()>( [ res = std::move( uploadBuffer ), alloc = std::move( uploadAlloc ) ]() mutable
										{
											if( res ) res.Reset();
											if( alloc )
											{
												alloc->Release();
												alloc = nullptr;
											}
										} ),
			submithandle );

	}

	void Device::DrawObjectIndirect( GraphicsPipelineHandle& pipeline, DrawDescription& desc, BufferHandle& bf )
	{
	
	
		//Changes the DepthState like vulkan en runtime its not posible in D3D12
		// the solution its to create differents pipelines state and only changin the PSO in runtime.
		////Get cmdlist 
		GraphicsPipeline* pi = dynamic_cast< GraphicsPipeline* >(pipeline.get());
		if( !pi )
		{
			IFNITY_LOG( LogCore, ERROR, "Failed to get D3D12 dynamic cast" );
			return;
		}

		//Get Rasterize state 
		const RasterizationState& rasterState = pipeline->GetGraphicsPipelineDesc().rasterizationState;

		pipeline->BindPipeline( this );
		cmdBuffer.cmdBindRenderPipeline( pi );

		cmdBuffer.cmdSetPrimitiveTopology( rasterState.primitiveType );

		if( currentIndexBuffer_ )
		{
			cmdBuffer.cmdBindIndexBuffer( currentIndexBuffer_ );
		}
		if( currentVertexBuffer_ )
		{
			cmdBuffer.cmdBindVertexBuffer( currentVertexBuffer_ );
		}

		cmdBuffer.cmdPushConstants(pushConstants.data,
									pushConstants.size,
									pushConstants.offset);

		BufferHandleSM indirectBufferHandle = DCAST_BUFFER( bf.get() )->getBufferHandleSM();


		cmdBuffer.cmdDrawIndexedIndirect(indirectBufferHandle, 0, desc.instanceCount);

	
	
	
	
	
	
	
	
	}


	void Device::upload( BufferHandleSM& buffer, const void* data, size_t size, uint32_t offset )
	{

		//Previos check if the buffer is null and check it 
		if( !data )
		{
			IFNITY_LOG( LogCore, ERROR, "Data is null to upload " );
			return;
		}

		IFNITY_ASSERT_MSG( size, "Data size should be non-zero" );

		D3D12Buffer* buf = m_DeviceD3D12->slotMapBuffers_.get( buffer );

		if( !buf )
		{
			IFNITY_LOG( LogCore, ERROR, "Buffer is null to upload " );
			return;
		}

		if( !IFNITY_VERIFY( offset + size <= buf->bufferSize_ ) )
		{
			return;
		}



		//Lets to staginDevice to upload data 
		m_DeviceD3D12->stagingDevice_->bufferSubData( *buf, offset, size, data );


	}


	D3D12_RESOURCE_FLAGS Device::getImageUsageFlags( const TextureDescription& texdesc )
	{
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		if( static_cast< uint8_t >(texdesc.usage) & static_cast< uint8_t >(rhi::TextureUsageBits::STORAGE) )
		{
			IFNITY_ASSERT_MSG( texdesc.sampleCount <= 1, "Unordered Access (storage image) not allowed on multisampled textures" );
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		if( static_cast< uint8_t >(texdesc.usage) & static_cast< uint8_t >(rhi::TextureUsageBits::ATTACHMENT) )
		{
			if( isDepthFormat( texdesc.format ) )
			{
				flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			}
			else
			{
				flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			}
		}

		return flags;
	}

	bool Device::validateTextureDescription( TextureDescription& texdesc )
	{
		const rhi::TextureType type = texdesc.dimension;

		if( texdesc.width <= 0 && texdesc.height <= 0 && texdesc.depth <= 0 )
		{
			IFNITY_LOG( LogCore, ERROR, "Texture dimension is invalid" );
			return false;
		}
		if( !(type == TextureType::TEXTURE2D || type == TextureType::TEXTURECUBE || type == TextureType::TEXTURE3D) )
		{
			IFNITY_ASSERT( false, "Only 2D, 3D and Cube textures are supported" );
			return false;
		}

		if( texdesc.mipLevels == 0 )
		{
			IFNITY_LOG( LogCore, WARNING, "The number of mip-levels is 0. Setting it to 1." );
			texdesc.mipLevels = 1;
		}

		if( texdesc.sampleCount > 1 && texdesc.mipLevels != 1 )
		{
			IFNITY_LOG( LogCore, WARNING, "Multisampled textures must have only one mip-level. Setting it to 1." );
			return false;
		}

		if( texdesc.sampleCount > 1 && type == rhi::TextureType::TEXTURE3D )
		{
			IFNITY_LOG( LogCore, WARNING, "Multisampled 3D textures are not supported. Setting it to 1." );
			texdesc.sampleCount = 1;
			return false;
		}

		if( !(texdesc.mipLevels <= Utils::getNumMipMapLevels2D( texdesc.dimensions.width, texdesc.dimensions.height )) )
		{
			IFNITY_LOG( LogCore, WARNING, "The number of mip-levels is too high. Setting it to the maximum possible value." );
			texdesc.mipLevels = Utils::getNumMipMapLevels2D( texdesc.dimensions.width, texdesc.dimensions.height );
		}

		if( texdesc.usage == rhi::TextureUsageBits::UNKNOW )
		{
			IFNITY_LOG( LogCore, WARNING, "Texture usage is not set. Setting it to sampled." );
			texdesc.usage = rhi::TextureUsageBits::SAMPLED;
		}

		return true;
	}


	void Device::WriteBuffer( BufferHandle& buffer, const void* data, size_t size, uint32_t offset )
	{
		if( buffer->GetBufferDescription().type == BufferType::CONSTANT_BUFFER )
		{
			//Internal cache buffer 
			pushConstants.data = data;
			pushConstants.size = size;
			pushConstants.offset = offset;
			return;
			/*cmdBuffer.cmdPushConstants( data, size, offset );*/
		}

		//Write other options and getting the buffer 
		if( buffer->GetBufferDescription().type == BufferType::UNIFORM_BUFFER )
		{
			//Get the BufferHandleSM by index  to avoid dynamic_cast. 
			D3D12::D3D12Buffer* buf = m_DeviceD3D12->slotMapBuffers_.getByIndex( buffer->GetBufferID() );
			if( !buf )
			{
				IFNITY_LOG( LogCore, ERROR, "Buffer is null to write " );
				return;
			}
			upload( buf, data, size, offset );

		}



	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	void Device::Draw(DrawDescription& desc)
	{
		//Only draw if m_managedPipeline its valid
		if( !m_DeviceD3D12->actualPipeline_ )
		{
			IFNITY_LOG( LogCore, ERROR, "No pipeline is set for drawing" );
			return;
		}
		GraphicsPipelineHandle pipeline = std::shared_ptr<GraphicsPipeline>( m_DeviceD3D12->actualPipeline_, [](GraphicsPipeline*){} );
		DrawObject( pipeline, desc );
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

	//==================================================================================================//
	//  MeshObject Methods			                                                                    //
	//==================================================================================================//
	MeshObject::MeshObject( const SceneObjectHandler& data, IDevice* device ): m_Device( DCAST_DEVICE( device ) )
	{
		//Get information about scene 
		//Chec if device its valid 
		if( !m_Device )
		{
			IFNITY_LOG( LogApp, ERROR, "Device is not valid" );
			return;
		}
		//Chec if mesh data its valid
		meshStatus_ = MeshStatus::BUFFER_NOT_INITIALIZED;

		//GET data information and fill m_MeshObjectDescription
		const MeshData& meshData = data->getMeshData();
		const MeshFileHeader header = data->getHeader();

		const uint32_t* indices = meshData.indexData_.data();
		const float* vertices = meshData.vertexData_.data();

		const size_t transformsSize = data->getScene().globalTransform_.size() * sizeof( glm::mat4 );
		const void* transformsData = data->getScene().globalTransform_.data();

		m_MeshObjectDescription.meshFileHeader = header; // for now its only de data avaialbe in the future 
		// its important to move the data sceen to meshobjectDescription. to avoid lost data. 



//Get data to modify like materials in vk , this solution its not optimal but in the future
//when we have a better solution that solve vk and d3d12, probably unify that Opengl scene pipeline. 
		auto materials = data->getMaterials();
		convertToD3D12Material(materials, *m_Device, data->getTexturesFiles(),allMaterialsTextures_);

		//Create Buffers and fill the data.
		BufferDescription bufferDesc = {};
		{
			bufferDesc.SetDebugName( "Indices Data Buffer - MeshObject" );
			bufferDesc.SetBufferType( BufferType::INDEX_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( header.indexDataSize );
			bufferDesc.SetData( indices );
			bufferDesc.SetStrideSize( sizeof( uint32_t ) );
		}
		m_BufferIndex = m_Device->CreateBuffer( bufferDesc );
		IFNITY_ASSERT_MSG( m_BufferIndex, "Failed to create index buffer" );
		//VertexData
		{
			bufferDesc.SetDebugName( "Vertex Data Buffer - MeshObject" );
			bufferDesc.SetBufferType( BufferType::VERTEX_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( header.vertexDataSize );
			bufferDesc.SetData( vertices );
			bufferDesc.SetStrideSize( VertexTraits<VertexScene>::numElements * sizeof(float));
		}
		m_BufferVertex = m_Device->CreateBuffer( bufferDesc );
		IFNITY_ASSERT_MSG( m_BufferVertex, "Failed to create vertex buffer" );
		//Buffer Transformers Model Matrices
		{
			bufferDesc.SetDebugName( "TransformerBuffer  - MeshObject" );
			bufferDesc.SetBufferType( BufferType::STORAGE_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( transformsSize );
			bufferDesc.SetStrideSize( sizeof( glm::mat4 ) ); //Here is the transform size 
			bufferDesc.SetData( transformsData );
		}
		m_BufferModelMatrices = m_Device->CreateBuffer( bufferDesc );


		IFNITY_ASSERT_MSG( m_BufferModelMatrices, "Failed to create model matrices buffer" );

		//Buffer Materials 
		{
			bufferDesc.SetDebugName( "Material Buffer  - MeshObject" );
			bufferDesc.SetBufferType( BufferType::STORAGE_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( sizeof( MaterialDescription ) * materials.size() );
			bufferDesc.SetData( materials.data() );
			bufferDesc.SetStrideSize( sizeof( MaterialDescription ) ); //Set the stride size for the material buffer
		}
		m_BufferMaterials = m_Device->CreateBuffer( bufferDesc );


		//DrawCommands Buffer and DrawData 
		std::vector<DrawIndexedIndirectCommand> drawCommands;
		std::vector<DrawID> drawID;

		const uint32_t numCommands = header.meshCount;

		drawCommands.resize( numCommands );
		drawID.resize( numCommands );

		DrawIndexedIndirectCommand* cmd = drawCommands.data();
		DrawID* dd = drawID.data();

		IFNITY_ASSERT( data->getScene().meshes_.size() == numCommands );

		uint32_t ddindex = 0;
		//Fill the draw commands and draw data 
		for( auto& shape : data->getShapes() )
		{
			int32_t meshId = shape.meshIndex;
			*cmd++ = {
				.bInstanceroot = ddindex,
				.count = meshData.meshes_[ meshId ].getLODIndicesCount( 0 ),
				.instanceCount = 1,
				.firstIndex = shape.indexOffset,
				.baseVertex = ( int32_t )shape.vertexOffset,
				.baseInstance = ddindex++,
			};
			*dd++ = {
				.transformId = shape.transformIndex,
				.materialId = shape.materialIndex,
			};
		}

		// Now Build the buffer Indirect buffer and DrawID storage buffer 
		// prepare indirect commands buffer
		{
			bufferDesc.SetDebugName( "Indirect Data Buffer - MeshObject" );
			bufferDesc.SetBufferType( BufferType::INDIRECT_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( sizeof( DrawIndexedIndirectCommand ) * numCommands );
			bufferDesc.SetStrideSize( sizeof( DrawIndexedIndirectCommand ) ); //Set the stride size for the indirect buffer
			bufferDesc.SetData( drawCommands.data() );
		}
		m_BufferIndirect = m_Device->CreateBuffer( bufferDesc );
		IFNITY_ASSERT_MSG( m_BufferIndirect, "Failed to create indirect buffer" );


		//DrawID buffer
		{
			bufferDesc.SetDebugName( "DrawID Data Buffer - MeshObject" );
			bufferDesc.SetBufferType( BufferType::STORAGE_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( sizeof( DrawID ) * numCommands );
			bufferDesc.SetStrideSize( sizeof( DrawID ) ); //Set the stride size for the drawID buffer
			bufferDesc.SetData( drawID.data() );

		}
		m_BufferDrawID = m_Device->CreateBuffer( bufferDesc );
		IFNITY_ASSERT_MSG( m_BufferDrawID, "Failed to create drawID buffer" );

		//Fill al m_SM
		m_SM.vertexBuffer		 = DCAST_BUFFER( m_BufferVertex.get() )->getBufferHandleSM();
		m_SM.indexBuffer		 = DCAST_BUFFER( m_BufferIndex.get() )->getBufferHandleSM();
		m_SM.indirectBuffer		 = DCAST_BUFFER( m_BufferIndirect.get() )->getBufferHandleSM();
		m_SM.drawIDBuffer		 = DCAST_BUFFER( m_BufferDrawID.get() )->getBufferHandleSM();
		m_SM.materialBuffer		 = DCAST_BUFFER( m_BufferMaterials.get() )->getBufferHandleSM();
		m_SM.modelMatricesBuffer = DCAST_BUFFER( m_BufferModelMatrices.get() )->getBufferHandleSM();

		m_SM.srvIndex.drawId        = m_SM.drawIDBuffer.index();
		m_SM.srvIndex.materials     = m_SM.materialBuffer.index();
		m_SM.srvIndex.modelMatrices = m_SM.modelMatricesBuffer.index();


	}

	void MeshObject::convertToD3D12Material( std::vector<MaterialDescription>& mt,
											 Device& device,
											 const std::vector<std::string>& files,
											 std::vector<TextureHandle>& mtlTextures )
	{


		uint32_t id = 0;
		//For all files convert to material. 
		//Iterate for all files and build texture
		mtlTextures.resize( files.size() );
		for( auto& file : files )
		{
			//load texture from file
			TextureDescription texdesc = {};
			texdesc.debugName = file;
			texdesc.dimension = rhi::TextureType::TEXTURE2D;
			texdesc.format = Format::R8G8B8A8;
			texdesc.filepath = file;
			texdesc.usage = TextureUsageBits::SAMPLED;
			texdesc.isDepth = false;
			// upload texture and fill widht, height, 
			const void* img = LoadTextureFromFileDescription( texdesc );
			texdesc.data = img;					//fill de data. 

			//Now create the texture
			TextureHandle texHandle = device.CreateTexture( texdesc );
			//Get the texture now using dcast 
			mtlTextures[ id++ ] = texHandle;
		}

		//Now create and update the material and get the id correctly 
		auto getTextureId = []( uint32_t idx, const std::span<TextureHandle>& textures )->uint32_t
			{
				if( idx == INVALID_TEXTURE ) [[likely]]
				{
					return 0;
				}
				else
				{
					//Get the texture index. 
					const auto& texture = textures[ idx ];
					return texture ? texture->GetTextureID() : 0; // return 0 if texture is null
				}
			};

		// use for each 
		std::for_each( mt.begin(), mt.end(), [ &getTextureId, &mtlTextures ]( auto& mtl )
					   {
						   //Get the texture id. 
						   mtl.ambientOcclusionMap_  = getTextureId( mtl.ambientOcclusionMap_, mtlTextures );
						   mtl.emissiveMap_          = getTextureId( mtl.emissiveMap_, mtlTextures );
						   mtl.albedoMap_            = getTextureId( mtl.albedoMap_, mtlTextures );
						   mtl.metallicRoughnessMap_ = getTextureId( mtl.metallicRoughnessMap_, mtlTextures );
						   mtl.normalMap_            = getTextureId( mtl.normalMap_, mtlTextures );
						   mtl.opacityMap_           = getTextureId( mtl.opacityMap_, mtlTextures );
					   } );

	}



	//==================================================================================================//
	// MeshObject Methods                                                        //
	//==================================================================================================//


	void MeshObject::DrawIndirect()
	{
		//Not implemented yet
		auto& buf = m_Device->getCommandBuffer();
		const auto& pc = m_Device->pushConstants;
		//Force now to use MAT4 [TODO] REMOVE FROM THIS. ITS ONLY FOR TESTING PURPOSES.
		glm::mat4 viewmodel = *reinterpret_cast<const glm::mat4*>(pc.data);

		//Build push constants
		const struct {	
			mat4 viewProj;
			uint32_t bufferTransforms;
			uint32_t bufferDrawData;
			uint32_t bufferMaterials;
		} pushConstans = {
				.viewProj = viewmodel,
				.bufferTransforms = m_SM.srvIndex.modelMatrices,
				.bufferDrawData   = m_SM.srvIndex.drawId,
				.bufferMaterials  = m_SM.srvIndex.materials
		};

		auto* pipeline = m_Device->getActualPipeline();
		//Get rasterstate 
		CHECK_PTR(pipeline, "Pipeline is null");
		const auto& rasterState = pipeline->GetGraphicsPipelineDesc().rasterizationState;


		buf.cmdBindRenderPipeline(pipeline);
		buf.cmdBindIndexBuffer(m_SM.indexBuffer);
		buf.cmdBindVertexBuffer(m_SM.vertexBuffer);
		buf.cmdSetPrimitiveTopology( rasterState.primitiveType );
		buf.cmdPushConstants(pushConstans);
		buf.cmdDrawIndexedIndirect(m_SM.indirectBuffer, 0, m_MeshObjectDescription.meshFileHeader.meshCount);



	}






	//==================================================================================================//
	//  Scene Objects Methods			                                                        //
	//==================================================================================================//
	SceneObject::SceneObject( const char* meshFile, const char* sceneFile, const char* materialFile )
	{
		////1.First load the mesh file
		header_ = loadMeshData( meshFile, meshData_ );
		loadSceneShapes( sceneFile );
		loadMaterials( materialFile, materials_, textureFiles_ );


	}

	void SceneObject::loadSceneShapes( const char* sceneFile )
	{
		IFNITY::loadScene( sceneFile, scene_ );

		// prepare draw data buffer
		for( const auto& c : scene_.meshes_ )
		{
			auto material = scene_.materialForNode_.find( c.first );
			if( material != scene_.materialForNode_.end() )
			{
				shapes_.push_back(
					DrawData{
						.meshIndex = c.second, // c.second is the mesh index
						.materialIndex = material->second, // material->second is the material index
						.LOD = 0,
						.indexOffset = meshData_.meshes_[ c.second ].indexOffset,
						.vertexOffset = meshData_.meshes_[ c.second ].vertexOffset,
						.transformIndex = c.first // c.first is the node index
					} );
			}
		}

		// force recalculation of all global transformations
		IFNITY::markAsChanged( scene_, 0 );
		IFNITY::recalculateGlobalTransforms( scene_ );
	}








}

IFNITY_END_NAMESPACE


