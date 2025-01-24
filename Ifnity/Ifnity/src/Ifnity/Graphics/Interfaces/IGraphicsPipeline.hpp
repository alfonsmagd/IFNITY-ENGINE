
#include "pch.h"
#include "IShader.hpp"


IFNITY_NAMESPACE

using GraphicsPipelineHandle = std::shared_ptr<struct IGraphicsPipeline>;


struct IFNITY_API Program
{
	unsigned int id;
};

struct IFNITY_API ViewPortState
{
	uint32_t x = 0.0f;
	uint32_t y = 0.0f;
	uint32_t width = 1200;
	uint32_t height = 720;
	uint32_t minDepth = 0;
	uint32_t maxDepth = 1;

	//Constructors
	ViewPortState() = default;
	ViewPortState(uint32_t x, uint32_t y, uint32_t width, uint32_t height): x(x), y(y), width(width), height(height) {};
};

struct IFNITY_API RasterizationState
{
	rhi::PrimitiveType primitiveType = rhi::PrimitiveType::TriangleList;
	rhi::CullModeType cullMode = rhi::CullModeType::FrontAndBack;
	rhi::FrontFaceType frontFace = rhi::FrontFaceType::CounterClockwise;
	rhi::FillModeType fillMode = rhi::FillModeType::None;
};

struct IFNITY_API BlendState
{
	//In Opengl you should use srcBlend and dstBlend to set the blend function. 
	//In Vulkan you should use srcColorBlendFactor and dstColorBlendFactor to set the blend function.
	//Todo: if the user use vkconfiguration --> pass in opengl configuration based. 
	//Todo: if the user uses opengl configuration --> pass in vkconfiguration based.
	bool        blendEnable = false;
	rhi::BlendFactor srcBlend = rhi::BlendFactor::SRC_ALPHA;
	rhi::BlendFactor dstBlend = rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
	rhi::BlendFactor srcAlphaBlendFactor = rhi::BlendFactor::SRC_ALPHA;
	rhi::BlendFactor dstAlphaBlendFactor = rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
	rhi::BlendFactor alphaBlendOp = rhi::BlendFactor::OPERATION_ADD;
	rhi::BlendFactor srcColorBlendFactor = rhi::BlendFactor::SRC_ALPHA;
	rhi::BlendFactor dstColorBlendFactor = rhi::BlendFactor::ZERO;
	rhi::BlendFactor colorBlendOp = rhi::BlendFactor::OPERATION_ADD;

	constexpr BlendState& setBlendEnable(bool enable) { blendEnable = enable; return *this; }
	constexpr BlendState& enableBlend() { blendEnable = true; return *this; }
	constexpr BlendState& disableBlend() { blendEnable = false; return *this; }
	constexpr BlendState& setSrcBlend( rhi::BlendFactor value) { srcBlend = value; return *this; }
	constexpr BlendState& setDestBlend(rhi::BlendFactor value) { dstBlend = value; return *this; }
	constexpr BlendState& setBlendOp(rhi::BlendFactor value) { colorBlendOp = value; return *this; }
	constexpr BlendState& setSrcAlphaBlendFactor(rhi::BlendFactor value) { srcAlphaBlendFactor = value; return *this; }
	constexpr BlendState& setDstAlphaBlendFactor(rhi::BlendFactor value) { dstAlphaBlendFactor = value; return *this; }
	constexpr BlendState& setAlphaBlendOp(rhi::BlendFactor value) { alphaBlendOp = value; return *this; }


};


struct IFNITY_API RenderState
{
	bool depthTest = false;
	bool depthWrite = false;
	bool stencil = false;
	BlendState blendState;
};


struct IFNITY_API GraphicsPipelineDescription
{
	RasterizationState rasterizationState;
	RenderState renderState;

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

	constexpr GraphicsPipelineDescription& SetRenderState(const RenderState& state)
	{
		renderState = state;
		return *this;
	}

	const char* debugName = "";

};

class IFNITY_API IGraphicsPipeline
{
public:
	virtual const GraphicsPipelineDescription& GetGraphicsPipelineDesc() const = 0;
	virtual void  BindPipeline(struct IDevice* device) = 0;
};

IFNITY_END_NAMESPACE