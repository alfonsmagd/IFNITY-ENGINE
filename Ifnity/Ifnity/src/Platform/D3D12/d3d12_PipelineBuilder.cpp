


#include "d3d12_PipelineBuilder.hpp"

IFNITY_NAMESPACE
namespace D3D12
{



	D3D12PipelineBuilder::D3D12PipelineBuilder()
	{}

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




}

IFNITY_END_NAMESPACE

