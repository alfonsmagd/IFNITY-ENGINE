#pragma once
#include "Ifnity/Graphics/Interfaces/IDevice.hpp"


IFNITY_NAMESPACE

namespace Vulkan
{

    class Device final: public IDevice
    {
    public:
        /**
        * @brief Constructor for the Device class.
        */
        Device();

       
        virtual ~Device();

        
        void Draw(DrawDescription& desc) override;

       
        GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDescription& desc) override;


        BufferHandle CreateBuffer(const BufferDescription& desc) override;

        void WriteBuffer(BufferHandle& buffer, const void* data, size_t size, uint32_t offset = 0) override;

        void BindingVertexAttributes(const VertexAttributeDescription* desc, int sizedesc, const void* data, size_t size)   override;
        void BindingVertexIndexAttributes(const VertexAttributeDescription* desc, int sizedesc, BufferHandle& bf) override;

        virtual TextureHandle CreateTexture(TextureDescription& desc) override;
        MeshObjectHandle CreateMeshObject(const MeshObjectDescription& desc) override;
        MeshObjectHandle CreateMeshObject(const MeshObjectDescription& desc, IMeshDataBuilder* meshbuilder)override;
        SceneObjectHandler CreateSceneObject(const char* meshes, const char* scene, const char* materials) override;
        MeshObjectHandle  CreateMeshObjectFromScene(const SceneObjectHandler& scene) override;
        void SetRenderState(const RenderState& state);


    };

    inline DeviceHandle CreateDevice()
    {
        return std::make_shared<Device>();
    }
}
IFNITY_END_NAMESPACE