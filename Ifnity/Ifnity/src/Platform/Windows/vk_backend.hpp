#pragma once


#include "Ifnity/Graphics/Interfaces/IDevice.hpp"
#include "../Vulkan/vulkan_classes.hpp"
#include "../Vulkan/vulkan_CommandBuffer.hpp"
IFNITY_NAMESPACE

namespace Vulkan
{
    //------------------------------------------------------------------------------------//
    // BUFFER VULKAN                                                                       //
    //-------------------------------------------------------------------------------------//
    
    class Buffer final: public IBuffer
    {
    public:
		Buffer(const BufferDescription& desc): m_Description(desc) {}
		Buffer(uint32_t bufferID, const BufferDescription& desc): m_BufferID(bufferID), m_Description(desc) {}
		Buffer(const BufferDescription& desc, HolderBufferSM&& buffer): m_Description(desc), m_holdBuffer(std::move(buffer)) {}
        BufferDescription& GetBufferDescription() override { return m_Description; }
        const uint32_t GetBufferID()  const override { return m_holdBuffer.get()->index(); }
        void SetData(const void* data) {}
		const void* GetData() const { return nullptr; }

    private:
        uint32_t m_BufferID = 0; ///< The buffer ID.
		BufferDescription m_Description; ///< The buffer description.
		HolderBufferSM  m_holdBuffer; ///< The buffer handle.
        
    };




	//------------------------------------------------------------------------------------//
	//  DEVICE VULKAN                                                                      //
	//-------------------------------------------------------------------------------------//
    class Device final: public IDevice
    {
       
    public:
        /**
        * @brief Constructor for the Device class.
        */
        
        Device(VkDevice vkDevice, DeviceVulkan* ptr);
       
        virtual ~Device();

        
        void Draw(DrawDescription& desc) override;
        void DrawObject(GraphicsPipelineHandle& pipeline, DrawDescription& desc) override;
		void StartRecording() override;
		void StopRecording() override;

       
        GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDescription& desc) override;


        BufferHandle CreateBuffer(const BufferDescription& desc) override;
        
		void upload(BufferHandleSM& buffer, const void* data, size_t size, uint32_t offset = 0);

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
        VkPipeline getVkPipeline(GraphicsPipelineHandleSM gp) const;
		const DeviceVulkan& getDeviceContextVulkan() const { return *m_DeviceVulkan; }
        void setActualPipeline(GraphicsPipeline* pipeline);

		//Void Destroy Shader Module
		void destroyShaderModule() ;
		
    
    private:
		// Add private members here
        HolderBufferSM CreateInternalVkBuffer(VkDeviceSize bufferSize,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memFlags,
            const char* debugName);
        HolderShaderSM createShaderModuleFromSpirV(const void* spirv, size_t numBytes, const char* debugName) const;
        HolderShaderSM createShaderModule(const char* shaderCode, size_t codeSize, VkShaderStageFlagBits stage, bool isBinary, const char* debugName) const ;

        
		const VkPhysicalDeviceLimits& getPhysicalDeviceLimits() const;

		VkDevice vkDevice_ = VK_NULL_HANDLE;
		VkPhysicalDevice vkPhysicalDevice_ = VK_NULL_HANDLE;
		CommandBuffer cmdBuffer;
        DeviceVulkan* m_DeviceVulkan = nullptr;
        TextureHandleSM currentTexture_;

        #define MAX_SHADER_STAGES 4
        //Shaders loading by device 
        std::vector<HolderShaderSM> m_vertex;
        std::vector<HolderShaderSM> m_fragment;


        struct
        {
            const void* data;
			size_t size;
			size_t offset;
        }pushConstants;
		
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

        ShaderModuleHandleSM m_vertex;
        ShaderModuleHandleSM m_fragment;
        bool destroy = false;


        GraphicsPipelineHandleSM ownerHandle_;
	public:
		//Destructor 
        ~GraphicsPipeline();

        GraphicsPipeline(GraphicsPipelineDescription&& desc):m_Description(std::move(desc)) {};
        GraphicsPipeline(GraphicsPipelineDescription&& desc, DeviceVulkan* dev);
        const GraphicsPipelineDescription& GetGraphicsPipelineDesc() const override { return m_Description; }
        void  BindPipeline(struct IDevice* device) override;
        ShaderModuleState* getVertexShaderModule();
        ShaderModuleState* getFragmentShaderModule();
        void  setSpecializationConstant(const SpecializationConstantDesc& spec);
        void  SetGraphicsPipelineDesc(GraphicsPipelineDescription desc) { m_Description = desc; }
		void  setColorFormat(rhi::Format format) { colorFormat = format; }
        const RenderPipelineState& getRenderPipelineState() const { return m_rVkPipelineState; }
		RenderPipelineState& getRenderPipelineState() { return m_rVkPipelineState; }
		RenderPipelineState* getRenderPipelineStatePtr() { return &m_rVkPipelineState; }
        void DestroyPipeline(VkDevice device);
		operator GraphicsPipelineHandleSM() const { return ownerHandle_; }
    
    private:
        DeviceVulkan* m_DeviceVulkan = nullptr;
		void   configureRenderPipelineState();
        void   passSpecializationConstantToVkFormat();
		friend class Device;
        friend GraphicsPipelineHandle Device::CreateGraphicsPipeline(GraphicsPipelineDescription& desc);

    
    };
}
IFNITY_END_NAMESPACE