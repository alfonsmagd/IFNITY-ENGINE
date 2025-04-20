// IDevice.hpp
// This interface will be used by the DeviceManager to provide rendering functionality to the user.

#pragma once
#include "pch.h"
#include "IShader.hpp"
#include "IBuffer.hpp"
#include "ITexture.hpp"
#include "IGraphicsPipeline.hpp"
#include "IMeshObject.hpp"
#include "ISceneObject.hpp"


IFNITY_NAMESPACE


enum IFNITY_API DrawModeUse
{
	DRAW = 0,
	DRAW_INDEXED = 1,
	DRAW_INDIRECT = 2,
	DRAW_INDEXED_INDIRECT = 3
	
};


struct IFNITY_API DrawDescription
{
	RasterizationState rasterizationState;
	ViewPortState      viewPortState;
	DrawModeUse drawMode = DRAW;
	bool depthTest = false;
	bool enableBias = false;
	bool isIndexed = false;
	const void* indices = nullptr;
    unsigned int size;
	unsigned int instanceCount = 1;
	struct
	{
		float Constant = 0.0f;
		float Slope = 0.0f;
		float Clamp = 0.0f;
	}depthBiasValues;

	//To vulkan uses 
	bool startintRecord = false;
	bool onlyOneRender  = false;
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

	constexpr DrawDescription& StartRecording()
	{
		startintRecord = true;
		return *this;
	}
	constexpr DrawDescription& StopRecording()
	{
		startintRecord = false;
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

    virtual void Draw(DrawDescription& desc) = 0;
    virtual GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDescription& desc) = 0;
	virtual void WriteBuffer(BufferHandle& buffer, const void* data, size_t size, uint32_t offset = 0) = 0;
    virtual void BindingVertexAttributes(const VertexAttributeDescription* desc, int sizedesc, const void* data, size_t size) = 0;
	virtual void BindingVertexIndexAttributes(const VertexAttributeDescription* desc, int sizedesc, BufferHandle& bf) {}; //todo abstract
	virtual void BindingVertexAttributesBuffer(BufferHandle& bf) = 0;
	virtual void BindingIndexBuffer(BufferHandle& bf) = 0 ;
	virtual BufferHandle CreateBuffer(const BufferDescription& desc) = 0;
	virtual TextureHandle CreateTexture(TextureDescription& desc) = 0; //TODO add TextureDescripton const
	virtual MeshObjectHandle CreateMeshObject(const MeshObjectDescription& desc) = 0;
	virtual MeshObjectHandle CreateMeshObject(const MeshObjectDescription& desc, IMeshDataBuilder* meshbuilder) = 0;
	virtual SceneObjectHandler CreateSceneObject(const char* meshes, const char* scene, const char* materials) = 0;
	virtual MeshObjectHandle  CreateMeshObjectFromScene(const SceneObjectHandler& scene) = 0;
	virtual void DrawObject(GraphicsPipelineHandle& pipeline, DrawDescription& desc) = 0; //todo abstract 
	virtual void StartRecording() {};
	virtual void StopRecording() {};
	virtual void SetDepthTexture(TextureHandle texture) = 0;
    // Virtual destructor to ensure proper destruction of derived objects
    virtual ~IDevice() = default;
};


using DeviceHandle = std::shared_ptr<IDevice>;


IFNITY_END_NAMESPACE