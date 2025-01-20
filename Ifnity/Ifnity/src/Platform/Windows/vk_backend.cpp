
#include "vk_backend.hpp"


IFNITY_NAMESPACE

namespace Vulkan
{

	//-----------------------------------------------//
    // Device METHODS                                //
	//-----------------------------------------------//


    Device::Device()
    {
        // Constructor implementation
    }

    Device::~Device()
    {
        // Destructor implementation
    }

    void Device::Draw(DrawDescription& desc)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    GraphicsPipelineHandle Device::CreateGraphicsPipeline(GraphicsPipelineDescription& desc)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    BufferHandle Device::CreateBuffer(const BufferDescription& desc)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    void Device::WriteBuffer(BufferHandle& buffer, const void* data, size_t size, uint32_t offset)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    void Device::BindingVertexAttributes(const VertexAttributeDescription* desc, int sizedesc, const void* data, size_t size)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    void Device::BindingVertexIndexAttributes(const VertexAttributeDescription* desc, int sizedesc, BufferHandle& bf)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    TextureHandle Device::CreateTexture(TextureDescription& desc)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    MeshObjectHandle Device::CreateMeshObject(const MeshObjectDescription& desc)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    MeshObjectHandle Device::CreateMeshObject(const MeshObjectDescription& desc, IMeshDataBuilder* meshbuilder)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    SceneObjectHandler Device::CreateSceneObject(const char* meshes, const char* scene, const char* materials)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    MeshObjectHandle Device::CreateMeshObjectFromScene(const SceneObjectHandler& scene)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }

    void Device::SetRenderState(const RenderState& state)
    {
        // Not implemented yet
        throw std::runtime_error("The function or operation is not implemented.");
    }



}

IFNITY_END_NAMESPACE