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
        

		//Vulkan Specific 
        VkPipeline getVkPipeline(struct GraphicsPipeline* gp) const;
		const DeviceVulkan& getDeviceContextVulkan() const { return *m_DeviceVulkan; }
        void setActualPipeline(GraphicsPipeline* pipeline);

		//Void Destroy Shader Module
		void destroyShaderModule() ;
		
    
    private:
		// Add private members here
        HolderShaderSM createShaderModuleFromSpirV(const void* spirv, size_t numBytes, const char* debugName) const;
        HolderShaderSM createShaderModule(const char* shaderCode, size_t codeSize, VkShaderStageFlagBits stage, bool isBinary, const char* debugName) const ;

        
		const VkPhysicalDeviceLimits& getPhysicalDeviceLimits() const;

		VkDevice vkDevice_ = VK_NULL_HANDLE;
		VkPhysicalDevice vkPhysicalDevice_ = VK_NULL_HANDLE;

        DeviceVulkan* m_DeviceVulkan = nullptr;

        //Shaders loading by device 
        HolderShaderSM m_vertex;
        HolderShaderSM m_fragment;
		
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
    private:
        GraphicsPipelineDescription m_Description;
		
		RenderPipelineState m_rVkPipelineState;
		VertexInput        m_vertexInput;
        SpecializationConstantDesc specInfo = {};

		rhi::Format colorFormat   = rhi::Format::UNKNOWN;
        rhi::Format depthFormat   = rhi::Format::UNKNOWN;
        rhi::Format stencilFormat = rhi::Format::UNKNOWN;


        StencilState backFaceStencil = {};
        StencilState frontFaceStencil = {};

        uint32_t samplesCount = 1u;
        float minSampleShading = 0.0f;

        ShaderModuleState* m_pvertex;
        ShaderModuleState* m_pfragment;
        bool destroy = false;

	public:
		//Destructor 
        ~GraphicsPipeline();

        GraphicsPipeline(GraphicsPipelineDescription&& desc):m_Description(std::move(desc)) {};
        GraphicsPipeline(GraphicsPipelineDescription&& desc, DeviceVulkan* dev);
        const GraphicsPipelineDescription& GetGraphicsPipelineDesc() const override { return m_Description; }
        void  BindPipeline(struct IDevice* device) override;
        ShaderModuleState* getVertexShaderModule() { if(m_pvertex) return m_pvertex;        else { return VK_NULL_HANDLE; } }
        ShaderModuleState*  getFragmentShaderModule() { if(m_pfragment) return m_pfragment; else { return VK_NULL_HANDLE; } }
        void  setSpecializationConstant(const SpecializationConstantDesc& spec);
        void  SetGraphicsPipelineDesc(GraphicsPipelineDescription desc) { m_Description = desc; }
		void  setColorFormat(rhi::Format format) { colorFormat = format; }
        RenderPipelineState& getRenderPipelineState() { return m_rVkPipelineState; }
        void DestroyPipeline(VkDevice device);
    
    private:
        DeviceVulkan* m_DeviceVulkan = nullptr;
		void   configureRenderPipelineState();
        void   passSpecializationConstantToVkFormat();
		friend class Device;
        friend GraphicsPipelineHandle Device::CreateGraphicsPipeline(GraphicsPipelineDescription& desc);

    
    };
}
IFNITY_END_NAMESPACE