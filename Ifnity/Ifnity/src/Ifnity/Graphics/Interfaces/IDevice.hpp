// IDevice.hpp
// This interface will be used by the DeviceManager to provide rendering functionality to the user.

#pragma once
#include "pch.h"
#include "IShader.hpp"
#include "IBuffer.hpp"
#include "ITexture.hpp"

IFNITY_NAMESPACE


struct IFNITY_API Program
{
     unsigned int id;
};

struct IFNITY_API GraphicsPipeline
{
    Program program;
    // Constructor por defecto
	 GraphicsPipeline() = default;
};

struct IFNITY_API RasterizationState
{
	rhi::PrimitiveType primitiveType = rhi::PrimitiveType::TriangleList;
	rhi::CullModeType cullMode =       rhi::CullModeType::FrontAndBack;
	rhi::FrontFaceType frontFace =     rhi::FrontFaceType::CounterClockwise;
	rhi::FillModeType fillMode   =     rhi::FillModeType::None;
};

struct IFNITY_API GraphicsPipelineDescription
{
	RasterizationState rasterizationState;

	IShader* vs  = nullptr;
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


struct IFNITY_API DrawDescription
{
	RasterizationState rasterizationState;

	bool isIndexed = false;
	const void* indices = nullptr;
    unsigned int size;
};

// Definition of the IDevice interface
class IFNITY_API IDevice {
public:

    /**
     * @brief Se va usar para dibujar.
     * 
     * @param desc Descripción del dibujo.
     */
    virtual void Draw(DrawDescription& desc) = 0;
    virtual GraphicsPipeline CreateGraphicsPipeline(GraphicsPipelineDescription& desc) = 0;
	virtual void WriteBuffer(BufferHandle& buffer, const void* data, size_t size, uint32_t offset = 0) = 0;
    virtual void BindingVertexAttributes(const VertexAttributeDescription* desc, int sizedesc, const void* data, size_t size) = 0;
	virtual void BindingVertexIndexAttributes(const VertexAttributeDescription* desc, int sizedesc, BufferHandle& bf) {}; //todo abstract
	virtual BufferHandle CreateBuffer(const BufferDescription& desc) = 0;

	virtual TextureHandle CreateTexture(TextureDescription& desc) = 0;

    // Virtual destructor to ensure proper destruction of derived objects
    virtual ~IDevice() = default;
};


using DeviceHandle = std::shared_ptr<IDevice>;


IFNITY_END_NAMESPACE