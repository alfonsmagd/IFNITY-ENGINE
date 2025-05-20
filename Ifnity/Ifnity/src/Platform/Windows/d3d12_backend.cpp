//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-18 by alfonsmagd





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

	bool validateImageLimits( D3D12_RESOURCE_DIMENSION dimension,
							  UINT sampleCount,
							  UINT width,
							  UINT height,
							  UINT depth )
	{
		// MSAA solo válido en 2D
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
			IFNITY_LOG( LogApp, INFO, "Pipeline creator" );
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

		cmdBuffer.cmdDraw( desc.drawMode, desc.size, 1 );

	}


	TextureHandle Device::CreateTexture( TextureDescription& desc )
	{

		using namespace rhi;

		//For simplify 
		TextureDescription texdesc( desc );
		const auto& dD312 = *m_DeviceD3D12;

		//Get the format value in this case will check only if the format is valid or depthformat 
		if( !validateTextureDescription( texdesc ) )
		{
			IFNITY_LOG( LogCore, ERROR, "Texture description is invalid" );
			return {};
		}

		DXGI_FORMAT format = formatToDxgiFormat( texdesc.format );

		D3D12_RESOURCE_FLAGS usageFlags = getImageUsageFlags( texdesc );


		const bool hasDebugName = texdesc.debugName.size() > 0;

		char debugNameImage[ 256 ] = { 0 };
		char debugNameImageView[ 256 ] = { 0 };

		if( hasDebugName )
		{
			snprintf( debugNameImage, sizeof( debugNameImage ) - 1, "Image: %s", texdesc.debugName );
			snprintf( debugNameImageView, sizeof( debugNameImageView ) - 1, "Image View: %s", texdesc.debugName );
		}

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




		return {};
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
		IFNITY_LOG( LogCore, INFO, "Buffer created: " + std::to_string( bufferHandle.index() ) );

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////





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


