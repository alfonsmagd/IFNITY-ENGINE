


#include "d3d12_PipelineBuilder.hpp"
#include  "Platform/Windows/d3dx12.h"
#include "Platform/Windows/DeviceD3D12.hpp"

IFNITY_NAMESPACE
namespace D3D12
{



	D3D12PipelineBuilder::D3D12PipelineBuilder()
	{
		// Initialize the pipeline state description default initialization 
		ZeroMemory(&psoDesc_, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psoDesc_.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc_.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc_.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc_.SampleMask = UINT_MAX;
		psoDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc_.NumRenderTargets = 1;
		psoDesc_.RTVFormats[ 0 ] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc_.SampleDesc.Count = 1;
		psoDesc_.SampleDesc.Quality = 0;
	
	
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setRootSignature(ID3D12RootSignature * rootSig)
	{
		// TODO: insert return statement here
		psoDesc_.pRootSignature = rootSig;
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setVS(const void* bytecode, SIZE_T size)
	{
		// TODO: insert return statement here
		psoDesc_.VS = { bytecode, size };
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setPS(const void* bytecode, SIZE_T size)
	{
		// TODO: insert return statement here
		psoDesc_.PS = { bytecode, size };
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setGS(const void* bytecode, SIZE_T size)
	{
		// TODO: insert return statement here
		psoDesc_.GS = { bytecode, size };
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setRasterizer(const D3D12_RASTERIZER_DESC& desc)
	{
		psoDesc_.RasterizerState = desc;
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setBlend(const D3D12_BLEND_DESC& desc)
	{
		psoDesc_.BlendState = desc;
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setDepthStencil(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		psoDesc_.DepthStencilState = desc;
		return *this;
	}
	D3D12PipelineBuilder& D3D12PipelineBuilder::setInputLayout(const D3D12_INPUT_LAYOUT_DESC& layout)
	{
		inputLayout_ = layout;
		return *this;
	}
	D3D12PipelineBuilder& D3D12PipelineBuilder::setSampleDesc(DXGI_SAMPLE_DESC desc)
	{
		psoDesc_.SampleDesc = desc;
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
	{
		psoDesc_.PrimitiveTopologyType = type;
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setRenderTargetFormats(UINT numRTs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat)
	{
		psoDesc_.NumRenderTargets = numRTs;
		for( UINT i = 0; i < numRTs; ++i )
		{
			psoDesc_.RTVFormats[ i ] = rtvFormats[ i ];
		}
		psoDesc_.DSVFormat = dsvFormat;
		return *this;
		// TODO: insert return statement here
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setDepthStencilFormat(DXGI_FORMAT format)
	{
		// TODO: insert return statement here
		psoDesc_.DSVFormat = format;
		return *this;
	}

	D3D12PipelineBuilder& D3D12PipelineBuilder::setSampleDescCount(UINT count)
	{
		psoDesc_.SampleDesc.Count = count;
		return *this;
	}

	HRESULT D3D12PipelineBuilder::build(ID3D12Device* device, ID3D12PipelineState** psoOut)
	{
		psoDesc_.InputLayout = inputLayout_;
		psoDesc_.SampleMask = UINT_MAX;
		ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc_, IID_PPV_ARGS(psoOut)));
		
		return S_OK;




	}




}

IFNITY_END_NAMESPACE

