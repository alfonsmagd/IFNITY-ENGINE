

#include "d3d12_backend.hpp"
#include "DeviceD3D12.hpp"
#include "Platform/D3D12/d3d12_PipelineBuilder.hpp"
#include "Ifnity/Graphics/Utils.hpp"



IFNITY_NAMESPACE

namespace D3D12
{
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

		//Get if files are binary, other solution its get the extension. 
		bool vsbinary = desc.vs->GetShaderDescription().APIflag & ShaderAPIflag::SPIRV_BIN;
		bool fsbinary = desc.ps->GetShaderDescription().APIflag & ShaderAPIflag::SPIRV_BIN;
		bool gsbinary = desc.gs ? desc.gs->GetShaderDescription().APIflag & ShaderAPIflag::SPIRV_BIN : false;

		if( !vs || !fs )
		{
			IFNITY_LOG(LogApp, WARNING, "Load GetPixelShader or VertexShader");
			return GraphicsPipelineHandle{};
		}
		// 1. retrieve the vertex/fragment/geometry source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			std::tie(vertexCode, fragmentCode, geometryCode) = Utils::readShaderFilesByAPI(rhi::GraphicsAPI::D3D12, vs, fs, gs);
		}
		catch( const std::runtime_error& e )
		{
			IFNITY_LOG(LogApp, ERROR, e.what());
			return GraphicsPipelineHandle{};
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = gs ? geometryCode.c_str() : nullptr;



		return {};





		
	}











	//------------------------------------------------------------------------------------//
	//  GRAPHICS PIPELINE D3D12                                                          //
	//-------------------------------------------------------------------------------------//
	GraphicsPipeline::GraphicsPipeline(GraphicsPipelineDescription&& desc, DeviceD3D12* dev): m_Description(std::move(desc)), m_DeviceD3D12(dev)
	{}



}

IFNITY_END_NAMESPACE