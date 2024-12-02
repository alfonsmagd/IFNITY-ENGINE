// IDevice.hpp
// This interface will be used by the DeviceManager to provide rendering functionality to the user.

#pragma once
#include "pch.h"
#include "IShader.hpp"
#include "IBuffer.hpp"
#include "ITexture.hpp"
#include "IGraphicsPipeline.hpp"
#include "IMeshObject.hpp"


IFNITY_NAMESPACE





struct IFNITY_API DrawDescription
{
	RasterizationState rasterizationState;
	ViewPortState      viewPortState;

	bool isIndexed = false;
	const void* indices = nullptr;
    unsigned int size;

	constexpr DrawDescription& SetRasterizationState(const RasterizationState& state)
	{
		rasterizationState = state;
		return *this;
	}
	constexpr DrawDescription& SetViewPortState(const ViewPortState& state)
	{
		viewPortState = state;
		return *this;
	}

	//Constructors 
	DrawDescription() = default;
	DrawDescription(const RasterizationState& rasterizationState, const ViewPortState& viewPortState): 
		rasterizationState(rasterizationState), viewPortState(viewPortState) {}
	DrawDescription& operator=(const DrawDescription& desc)
	{
		rasterizationState = desc.rasterizationState;
		viewPortState = desc.viewPortState;
		isIndexed = desc.isIndexed;
		indices = desc.indices;
		size = desc.size;
		return *this;
	}
	const DrawDescription& operator=(DrawDescription&& desc) noexcept
	{
		rasterizationState = std::move(desc.rasterizationState);
		viewPortState = std::move(desc.viewPortState);
		isIndexed = std::move(desc.isIndexed);
		indices = std::move(desc.indices);
		size = std::move(desc.size);
		return *this;
	}
	~DrawDescription()
	{
		delete indices;
	}

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
    virtual GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDescription& desc) = 0;
	virtual void WriteBuffer(BufferHandle& buffer, const void* data, size_t size, uint32_t offset = 0) = 0;
    virtual void BindingVertexAttributes(const VertexAttributeDescription* desc, int sizedesc, const void* data, size_t size) = 0;
	virtual void BindingVertexIndexAttributes(const VertexAttributeDescription* desc, int sizedesc, BufferHandle& bf) {}; //todo abstract
	virtual BufferHandle CreateBuffer(const BufferDescription& desc) = 0;

	virtual TextureHandle CreateTexture(TextureDescription& desc) = 0;
	virtual MeshObjectHandle CreateMeshObject(const MeshObjectDescription& desc) = 0;

    // Virtual destructor to ensure proper destruction of derived objects
    virtual ~IDevice() = default;
};


using DeviceHandle = std::shared_ptr<IDevice>;


IFNITY_END_NAMESPACE