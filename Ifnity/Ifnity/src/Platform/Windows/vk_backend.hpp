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
        Device(VkDevice vkDevice, DeviceVulkan* ptr);
       
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
		ShaderModuleState createShaderModuleFromSpirV(const void* spirv, size_t numBytes, const char* debugName) const;
        ShaderModuleState createShaderModule(const char* shaderCode, size_t codeSize, VkShaderStageFlagBits stage, bool isBinary, const char* debugName) const ;

		VkDevice vkDevice_ = VK_NULL_HANDLE;
		VkPhysicalDevice vkPhysicalDevice_ = VK_NULL_HANDLE;

        DeviceVulkan* m_DeviceVulkan = nullptr;
		
    };

    //Please constructor are not safe if you dont create the 
    template<typename... Args>
    inline DeviceHandle CreateDevice(Args&&... args)
    {
       
        return std::make_shared<Device>(std::forward<Args>(args)...);
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
    
    private:
        friend GraphicsPipelineHandle Device::CreateGraphicsPipeline(GraphicsPipelineDescription& desc);
    
    };
}
IFNITY_END_NAMESPACE