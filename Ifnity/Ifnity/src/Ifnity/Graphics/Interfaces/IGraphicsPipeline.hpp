
#include "pch.h"
#include "IShader.hpp"


IFNITY_NAMESPACE

using GraphicsPipelineHandle = std::shared_ptr<struct IGraphicsPipeline>;

struct IFNITY_API Program
{
	unsigned int id;
};


struct IFNITY_API RasterizationState
{
	rhi::PrimitiveType primitiveType = rhi::PrimitiveType::TriangleList;
	rhi::CullModeType cullMode = rhi::CullModeType::FrontAndBack;
	rhi::FrontFaceType frontFace = rhi::FrontFaceType::CounterClockwise;
	rhi::FillModeType fillMode = rhi::FillModeType::None;
};


struct IFNITY_API GraphicsPipelineDescription
{
	RasterizationState rasterizationState;

	IShader* vs = nullptr;
	IShader* ps = nullptr;
	IShader* gs = nullptr;



	GraphicsPipelineDescription& SetVertexShader(IShader* shader)
	{
		vs = shader;
		return *this;
	}
	GraphicsPipelineDescription& SetPixelShader(IShader* shader)
	{
		ps = shader;
		return *this;
	}
	GraphicsPipelineDescription& SetGeometryShader(IShader* shader)
	{
		gs = shader;
		return *this;
	}

	constexpr GraphicsPipelineDescription& SetRasterizationState(const RasterizationState& state)
	{
		rasterizationState = state;
		return *this;
	}

};

class IFNITY_API IGraphicsPipeline
{

	virtual const GraphicsPipelineDescription& GetGraphicsPipelineDesc() const = 0;
};

IFNITY_END_NAMESPACE