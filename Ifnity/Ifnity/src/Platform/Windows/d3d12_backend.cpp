

#include "d3d12_backend.hpp"
#include "DeviceD3D12.hpp"
#include "Platform/D3D12/d3d12_PipelineBuilder.hpp"
#include "Ifnity/Graphics/Utils.hpp"

#include "ShaderBuilding\ShaderBuilder.hpp"



IFNITY_NAMESPACE

namespace D3D12
{

    //--------------------------------------------------------------------//
    //  GRAPHICS PIPELINE                                   //
    //----------------------------------------------------------------------//
	GraphicsPipeline::GraphicsPipeline(GraphicsPipelineDescription&& desc): m_Description(std::move(desc))
	{}














	//--------------------------------------------------------------------//
	//  DEVICE D3D12                                                        //
	//----------------------------------------------------------------------//

	Device::Device( DeviceD3D12* ptr):  m_DeviceD3D12(ptr)
	{
		IFNITY_ASSERT_MSG(m_DeviceD3D12 != nullptr, "DeviceD3D12 is null");
		//Check if m_DeviceD31D12 has been created well 
		IFNITY_ASSERT_MSG(m_DeviceD3D12->m_Device.Get() != nullptr, "ID3D12Device is null");
	}
	








	GraphicsPipelineHandle Device::CreateGraphicsPipeline(GraphicsPipelineDescription& desc)
	{

		auto* vs = desc.vs;
		auto* fs = desc.ps;
		auto* gs = desc.gs;

        // Get the shader description
        ShaderCreateDescription vsDesc = vs->GetShaderDescription();
        ShaderCreateDescription fsDesc = fs->GetShaderDescription();

        // Resolve the shader directory path
        VFS& vfs = VFS::GetInstance();
        std::string shaderDir = vfs.ResolvePath("Shaders", "d3d12");

        // Construct the file paths for _vs.cso and _ps.cso
        std::string vsFilePath = shaderDir + "/" + vsDesc.FileName + "_vs.cso";
        std::string fsFilePath = shaderDir + "/" + fsDesc.FileName + "_ps.cso";

        // Check if the files exist
        bool vsExists = std::filesystem::exists(vsFilePath);
        bool fsExists = std::filesystem::exists(fsFilePath);

        if (!vsExists || !fsExists)
        {
            IFNITY_LOG(LogApp, ERROR, "Shader files not found: " + vsFilePath + " or " + fsFilePath);
            return GraphicsPipelineHandle{};
        }

        // Load the blobs from the files
        IDxcBlob* vsBlob = ShaderCompiler::GetBlobFromFile(vsFilePath);
        IDxcBlob* fsBlob = ShaderCompiler::GetBlobFromFile(fsFilePath);

        if (!vsBlob || !fsBlob)
        {
            IFNITY_LOG(LogApp, ERROR, "Failed to load shader blobs from files: " + vsFilePath + " or " + fsFilePath);
            return GraphicsPipelineHandle{};
        }

        // Log success
        IFNITY_LOG(LogApp, INFO, "Successfully loaded shader blobs: " + vsFilePath + " and " + fsFilePath);

		//Set the ShaderModuleState with new GraphicsPipelineState
		
		GraphicsPipeline* pipeline = new GraphicsPipeline(std::move(desc));

		// Create the StateShaderModule 
		ShaderModuleState vsState = { .bytecode = vsBlob};  
		ShaderModuleState fsState = { .bytecode = fsBlob};


        //Add the StatheShaderModule to SlotMap  
		pipeline->m_shaderVert = m_DeviceD3D12->slotMapShaderModules_.create(std::move(vsState));
		pipeline->m_shaderPixel = m_DeviceD3D12->slotMapShaderModules_.create(std::move(fsState));



		return GraphicsPipelineHandle{ pipeline };
       





		
	}











	//------------------------------------------------------------------------------------//
	//  GRAPHICS PIPELINE D3D12                                                          //
	//-------------------------------------------------------------------------------------//
	GraphicsPipeline::GraphicsPipeline(GraphicsPipelineDescription&& desc, DeviceD3D12* dev): m_Description(std::move(desc)), m_DeviceD3D12(dev)
	{}



}

IFNITY_END_NAMESPACE