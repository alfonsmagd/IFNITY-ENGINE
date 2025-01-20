#pragma once
#include "Ifnity/Graphics/Interfaces/IDevice.hpp"
#include "../Vulkan/vulkan_classes.hpp"

IFNITY_NAMESPACE

namespace Vulkan
{

	//------------------------------------------------------------------------------------//
	//  DEVICE VULKAN                                                                      //
	//-------------------------------------------------------------------------------------//
    class Device final: public IDevice
    {
    public:
        /**
        * @brief Constructor for the Device class.
        */
        Device();
		Device(VkDevice vkDevice);
       
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
    
    private:
		// Add private members here
		ShaderModuleState createShaderModuleFromSpirV(const void* spirv, size_t numBytes, const char* debugName);
		VkDevice vkDevice_ = VK_NULL_HANDLE;
    };

    inline DeviceHandle CreateDevice()
    {
        return std::make_shared<Device>();
    }
	inline DeviceHandle CreateDevice(VkDevice vkDevice)
	{
		return std::make_shared<Device>(vkDevice);
	}

//------------------------------------------------------------------------------------//
//  G PIPELINE  VULKAN                                                                 //
//-------------------------------------------------------------------------------------//
    class GraphicsPipeline final: public IGraphicsPipeline
    {
        GraphicsPipelineDescription m_Description;
		ShaderModuleState m_vertex;
		ShaderModuleState m_fragment;

	public:
		//Destructor 
        ~GraphicsPipeline() {};
        const GraphicsPipelineDescription& GetGraphicsPipelineDesc() const override { return m_Description; }
        void  BindPipeline(struct IDevice* device) override;
    
    
    };
}
IFNITY_END_NAMESPACE