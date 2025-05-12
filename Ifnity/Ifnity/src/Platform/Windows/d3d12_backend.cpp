

#include "d3d12_backend.hpp"
#include "DeviceD3D12.hpp"
#include "Platform/D3D12/d3d12_PipelineBuilder.hpp"
#include "Ifnity/Graphics/Utils.hpp"

#include "ShaderBuilding/ShaderBuilder.hpp"
#include "Platform/D3D12/d3d12_constants.hpp"



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





	void GraphicsPipeline::BindPipeline( IDevice* device )
	{
		Device* vkDevice = dynamic_cast< Device* >(device);
		IFNITY_ASSERT_MSG( vkDevice != nullptr, "Device is not a D3D12 Device" );

		//Create the pipeline 
		if( m_PipelineState )
		{
			IFNITY_LOG( LogApp, INFO, "Pipeline craetor" );
			return;

		}


		//Get ShaderModuleState
		ShaderModuleState* mvert = m_DeviceD3D12->slotMapShaderModules_.get( m_shaderVert );
		ShaderModuleState* mps = m_DeviceD3D12->slotMapShaderModules_.get( m_shaderPixel );

		//FOr now to simplify this. 
		auto pipeline = D3D12PipelineBuilder{}
			.setVS( mvert->bytecode->GetBufferPointer(),
					mvert->bytecode->GetBufferSize() )
			.setPS( mps->bytecode->GetBufferPointer(),
					mps->bytecode->GetBufferSize() )
			.setRootSignature( m_DeviceD3D12->m_RootSignature.Get() )
			.setInputLayout( m_rD3D12PipelineState.inputLayout_ )
			.build( m_DeviceD3D12->m_Device.Get(),
					OUT m_PipelineState.GetAddressOf() );



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

		GraphicsPipeline* pipeline = new GraphicsPipeline( std::move( desc ),m_DeviceD3D12 );

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