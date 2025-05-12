#pragma once 

#include <pch.h>
#include <d3d12.h>

IFNITY_NAMESPACE

namespace D3D12
{
	class D3D12PipelineBuilder final
	{
	public:
		D3D12PipelineBuilder();
		~D3D12PipelineBuilder() = default;

		D3D12PipelineBuilder& setRootSignature(ID3D12RootSignature* rootSig);
		D3D12PipelineBuilder& setVS(const void* bytecode, SIZE_T size);
		D3D12PipelineBuilder& setPS(const void* bytecode, SIZE_T size);
		D3D12PipelineBuilder& setGS(const void* bytecode, SIZE_T size);
		D3D12PipelineBuilder& setRasterizer(const D3D12_RASTERIZER_DESC& desc);
		D3D12PipelineBuilder& setBlend(const D3D12_BLEND_DESC& desc);
		D3D12PipelineBuilder& setDepthStencil(const D3D12_DEPTH_STENCIL_DESC& desc);
		D3D12PipelineBuilder& setInputLayout(const D3D12_INPUT_LAYOUT_DESC& layout);
		D3D12PipelineBuilder& setSampleDesc(DXGI_SAMPLE_DESC desc);
		D3D12PipelineBuilder& setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		D3D12PipelineBuilder& setRenderTargetFormats(UINT numRTs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat);
		D3D12PipelineBuilder& setDepthStencilFormat(DXGI_FORMAT format);
		D3D12PipelineBuilder& setSampleDescCount(UINT count);

		HRESULT build(ID3D12Device* device, ID3D12PipelineState** psoOut);

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc_{};
		D3D12_INPUT_LAYOUT_DESC inputLayout_{};

		D3D12_RASTERIZER_DESC rasterizerDesc_{};
		D3D12_BLEND_DESC blendDesc_{};
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		const void* vsCode_ = nullptr;
		SIZE_T vsSize_ = 0;
		const void* psCode_ = nullptr;
		SIZE_T psSize_ = 0;
		const void* gsCode_ = nullptr;
		SIZE_T gsSize_ = 0;

		DXGI_FORMAT rtvFormats_[ 8 ] = {};
	};


}



IFNITY_END_NAMESPACE