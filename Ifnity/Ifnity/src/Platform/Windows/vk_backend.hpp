#pragma once


#include "Ifnity/Graphics/Interfaces/IDevice.hpp"
#include "../Vulkan/vulkan_classes.hpp"
#include "../Vulkan/vulkan_CommandBuffer.hpp"
#include "../Vulkan/vulkan_StaginDevice.hpp"
IFNITY_NAMESPACE

namespace Vulkan
{
    //------------------------------------------------------------------------------------//
    // BUFFER VULKAN                                                                       //
    //-------------------------------------------------------------------------------------//
    
    class IFNITY_API Buffer final: public IBuffer
    {
    public:
		Buffer(const BufferDescription& desc): m_Description(desc) {}
		Buffer(uint32_t bufferID, const BufferDescription& desc): m_BufferID(bufferID), m_Description(desc) {}
		Buffer(const BufferDescription& desc, HolderBufferSM&& buffer): m_Description(desc), m_holdBuffer(std::move(buffer)) {}
        BufferDescription& GetBufferDescription() override { return m_Description; }
        const uint32_t GetBufferID()  const override { return m_holdBuffer.get()->index(); }
        void SetData(const void* data) {}
		const void* GetData() const { return nullptr; }
        BufferHandleSM getBufferHandleSM() const { return *m_holdBuffer; }

    private:
        uint32_t m_BufferID = 0; ///< The buffer ID.
		BufferDescription m_Description; ///< The buffer description.
		HolderBufferSM  m_holdBuffer; ///< The buffer handle.
        
    };

    //------------------------------------------------------------------------------------//
    // TEXTURE VULKAN                                                                       //
    //-------------------------------------------------------------------------------------//

    class IFNITY_API Texture final: public ITexture
    {
    public:
        virtual TextureDescription GetTextureDescription() override { return m_TextureDescription; }
        virtual uint32_t           GetTextureID() override { return *m_holdTexture; }

        //Constructor 
		Texture() = default;
		Texture(const TextureDescription & desc, uint32_t uid): m_TextureDescription(desc), m_TextureID(uid)
		{}
        Texture(const TextureDescription & desc, HolderTextureSM && texture): m_TextureDescription(desc), m_holdTexture(std::move(texture)){}
        Texture(TextureDescription& desc);
        Texture(int w, int h, const void* img);

        TextureHandleSM getTextureHandleSM() const { return *m_holdTexture; }

    private:
		UINT32 m_TextureID = 0;
        TextureDescription m_TextureDescription;
		HolderTextureSM m_holdTexture;

	};

	//------------------------------------------------------------------------------------//
	//  DEVICE VULKAN                                                                      //
	//-------------------------------------------------------------------------------------//
    class IFNITY_API Device final: public IDevice
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
        void BindingVertexAttributesBuffer(BufferHandle& bf) override;
		void BindingIndexBuffer(BufferHandle& bf) override;

        virtual TextureHandle CreateTexture(TextureDescription& desc) override;
        MeshObjectHandle CreateMeshObject(const MeshObjectDescription& desc) override;
        MeshObjectHandle CreateMeshObject(const MeshObjectDescription& desc, IMeshDataBuilder* meshbuilder)override;
        SceneObjectHandler CreateSceneObject(const char* meshes, const char* scene, const char* materials) override;
        MeshObjectHandle  CreateMeshObjectFromScene(const SceneObjectHandler& scene) override;
        void SetRenderState(const RenderState& state);
        
		//Set Depth Texture
		void SetDepthTexture(TextureHandle texture) override;

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
        VkFormat getClosestDepthStencilFormat(rhi::Format desiredFormat) const;
        bool validateTextureDescription(TextureDescription& texdesc);
        VkImageUsageFlags getImageUsageFlags(const TextureDescription& texdesc);

        
		//DepthTexture
		TextureHandleSM depthTexture_;
        //DepthFormat
		VkFormat depthFormat_;

		const VkPhysicalDeviceLimits& getPhysicalDeviceLimits() const;

		VkDevice vkDevice_ = VK_NULL_HANDLE;
		VkPhysicalDevice vkPhysicalDevice_ = VK_NULL_HANDLE;
		CommandBuffer cmdBuffer;
        DeviceVulkan* m_DeviceVulkan = nullptr;
		std::unique_ptr<VulkanStagingDevice> m_StagingDevice;
        TextureHandleSM currentTexture_;


        #define MAX_SHADER_STAGES 4
        //Shaders loading by device 
        std::vector<HolderShaderSM> m_shaderVert;
        std::vector<HolderShaderSM> m_shaderFragment;

        //TODO:Remove in the future its owner the mesh or something, now its for test 
        #define MAX_VERTEX_BUFFERS 1
		#define MAX_INDEX_BUFFERS 1
        //Now i only uses one 
        std::vector<BufferHandleSM> m_vertexBuffer;
        std::vector<BufferHandleSM> m_indexBuffer;

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
    class IFNITY_API GraphicsPipeline final: public IGraphicsPipeline
    {
    private:
        GraphicsPipelineDescription m_Description;
		
		RenderPipelineState m_rVkPipelineState;
        SpecializationConstantDesc specInfo = {};

		rhi::Format colorFormat   = rhi::Format::UNKNOWN;
        rhi::Format depthFormat   = rhi::Format::UNKNOWN;
        rhi::Format stencilFormat = rhi::Format::UNKNOWN;


        StencilState backFaceStencil = {};
        StencilState frontFaceStencil = {};

        uint32_t samplesCount = 1u;
        float minSampleShading = 0.0f;

        ShaderModuleHandleSM m_shaderVert;
        ShaderModuleHandleSM m_shaderFragment;
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
		void  setDepthFormat(rhi::Format format) { depthFormat = format; }
        const RenderPipelineState& getRenderPipelineState() const { return m_rVkPipelineState; }
		RenderPipelineState& getRenderPipelineState() { return m_rVkPipelineState; }
		RenderPipelineState* getRenderPipelineStatePtr() { return &m_rVkPipelineState; }
        void DestroyPipeline(VkDevice device);
		operator GraphicsPipelineHandleSM() const { return ownerHandle_; }
    
    private:
        DeviceVulkan* m_DeviceVulkan = nullptr;
		void   configureVertexAttributes();
        void   passSpecializationConstantToVkFormat();
		friend class Device;
        friend GraphicsPipelineHandle Device::CreateGraphicsPipeline(GraphicsPipelineDescription& desc);

    
    };
}
IFNITY_END_NAMESPACE