
#include "vk_backend.hpp"
#include "Ifnity\Graphics\Utils.hpp"
#include "UtilsVulkan.h"

//GLSLANG INCLUDES
#include <glslang\Public\resource_limits_c.h>
#include <spirv_cross/spirv_reflect.hpp>
#include "DeviceVulkan.h"

IFNITY_NAMESPACE

namespace Vulkan
{

	//-----------------------------------------------//
    // Device METHODS                                //
	//-----------------------------------------------//


	Device::Device(): vkDevice_(VK_NULL_HANDLE)
    {
        // Constructor implementation
    }

	Device::Device(VkDevice vkDevice): vkDevice_(vkDevice)
	{
		// Constructor implementation
	}

	Device::Device(VkDevice vkDevice, DeviceVulkan* ptr): vkDevice_(vkDevice), m_DeviceVulkan(ptr)
    {
    
		IFNITY_ASSERT_MSG(vkDevice_ != VK_NULL_HANDLE, "VkDevice is null");
		IFNITY_ASSERT_MSG(m_DeviceVulkan != nullptr, "DeviceVulkan is null");
		
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
        auto* vs = desc.vs;
        auto* fs = desc.ps;
        auto* gs = desc.gs;

        //Get if files are binary, other solution its get the extension. 
        bool vsbinary = desc.vs->GetShaderDescription().APIflag & ShaderAPIflag::SPIRV_BIN;
		bool fsbinary = desc.ps->GetShaderDescription().APIflag & ShaderAPIflag::SPIRV_BIN;
		bool gsbinary = desc.gs ? desc.gs->GetShaderDescription().APIflag & ShaderAPIflag::SPIRV_BIN : false;

        if(!vs || !fs)
        {
            IFNITY_LOG(LogApp, WARNING, "Load GetPixelShader or VertexShader");
            return GraphicsPipelineHandle{};
        }
        // 1. retrieve the vertex/fragment/geometry source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            std::tie(vertexCode, fragmentCode, geometryCode) = Utils::readShaderFilesByAPI(rhi::GraphicsAPI::VULKAN, vs, fs, gs);
        }
        catch(const std::runtime_error& e)
        {
            IFNITY_LOG(LogApp, ERROR, e.what());
            return GraphicsPipelineHandle{};
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* gShaderCode = gs ? geometryCode.c_str() : nullptr;

		// 2. compile shaders
		GraphicsPipeline* pipeline = new GraphicsPipeline(std::move(desc));

		pipeline->m_vertex   = createShaderModule(vShaderCode, vertexCode.size(), VK_SHADER_STAGE_VERTEX_BIT, vsbinary, "Vertex Shader");
		pipeline->m_fragment = createShaderModule(fShaderCode, fragmentCode.size(), VK_SHADER_STAGE_FRAGMENT_BIT, fsbinary, "Fragment Shader");

		//3. Create the pipeline
		pipeline->configureRenderPipelineState();


        return GraphicsPipelineHandle(pipeline);

       
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

	//--------------------------------------------------------------------------------------------------//
	//                  Device Specific Methods                                                         //
	//--------------------------------------------------------------------------------------------------//

    VkPipeline Device::getVkPipeline(GraphicsPipeline* gp) const
    {
		IFNITY_ASSERT_MSG(gp != nullptr, "GraphicsPipeline is null");

		RenderPipelineState* rps = &gp->m_rVkPipelineState;

        if(rps->pipeline_ != VK_NULL_HANDLE)
        {
            return rps->pipeline_;
        }
        
        // build a new Vulkan pipeline

        VkPipelineLayout layout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;

        const GraphicsPipelineDescription& desc = gp->m_Description;

		//const uint32_t numColorAttachments = &gp->m_rVkPipelineState.numColorAttachments_; only one color attachment format 

        // Not all attachments are valid. We need to create color blend attachments only for active attachments
        VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[ MAX_COLOR_ATTACHMENTS ] = {};
        VkFormat colorAttachmentFormats[ MAX_COLOR_ATTACHMENTS ] = {};

		return VK_NULL_HANDLE;
    }

    ShaderModuleState Device::createShaderModuleFromSpirV(const void* spirv, size_t numBytes, const char* debugName) const
    {
        VkShaderModule vkShaderModule = VK_NULL_HANDLE;

        const VkShaderModuleCreateInfo ci = {
             .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
             .codeSize = numBytes,
             .pCode = (const uint32_t*)spirv
        };

        {
            const VkResult result = vkCreateShaderModule(vkDevice_, &ci, nullptr, &vkShaderModule);


            if(result != VK_SUCCESS)
            {
                return { .sm = VK_NULL_HANDLE };
            }

            size_t numElements = numBytes / sizeof(uint32_t);

			//todo: move to heap compilerRefelction to stack 

            spirv_cross::CompilerReflection compiler((const uint32_t*)spirv,numElements);

            // Refleja los recursos del shader
            spirv_cross::ShaderResources resources = compiler.get_shader_resources();

            uint32_t pushConstantSize = 0;

            // Itera sobre las push constants y calcula el tamaño total
            for(const auto& pushConstant : resources.push_constant_buffers)
            {
                const spirv_cross::SPIRType& type = compiler.get_type(pushConstant.base_type_id);
                pushConstantSize += compiler.get_declared_struct_size(type);
            }

			return { .sm = vkShaderModule, .pushConstantsSize = pushConstantSize };
        }
    }

    ShaderModuleState Device::createShaderModule(const char* shaderCode, size_t codeSize, VkShaderStageFlagBits stage, bool isBinary, const char* debugName) const 
    {
        std::vector<uint8_t> spirv;
        if(isBinary)
        {
            return createShaderModuleFromSpirV(shaderCode, codeSize, debugName);
        }
        else
        {

			const glslang_resource_t resource = getGlslangResource(m_DeviceVulkan->properties2.properties.limits);
            
            compileShaderVK(stage, shaderCode, &spirv, &resource);
            return createShaderModuleFromSpirV(spirv.data(), spirv.size(), debugName);
        }
    }

    

	//==================================================================================================//
	//  GraphicsPipeline METHODS                                                                      //
	//==================================================================================================//
	
	void GraphicsPipeline::BindPipeline( IDevice* device)
	{
		Device* vkDevice = dynamic_cast<Device*>(device);
		IFNITY_ASSERT_MSG(vkDevice != nullptr, "Device is not a Vulkan Device");

        
		auto vkpipeline = vkDevice->getVkPipeline(this);


		
	}

    void GraphicsPipeline::setSpecializationConstant(const SpecializationConstantDesc& spec)
    {
        if(spec.data && spec.dataSize)
        {
			// copy into a local storage //First Reserve the memory,
            m_rVkPipelineState.specConstantDataStorage_ = malloc(spec.dataSize);
			memcpy(m_rVkPipelineState.specConstantDataStorage_, spec.data, spec.dataSize); //Copy the data in the memory VkPipeline 
			specInfo.data = m_rVkPipelineState.specConstantDataStorage_; // GetInformation from the data
        }
    
    
    }

    void GraphicsPipeline::configureRenderPipelineState()
    {
    
		//Not implemented yet BUT UPDATE THE RENDER PIPELINE STATE and configure inside 
		m_rVkPipelineState.numBindings_ = m_vertexInput.getNumInputBindings();
		m_rVkPipelineState.numAttributes_ = m_vertexInput.getNumAttributes();

        bool bufferAlreadyBound[ VertexInput::VERTEX_BUFFER_MAX ] = {};
	
        for(uint32_t i = 0; i != m_rVkPipelineState.numAttributes_; i++)
        {
            const auto& attr = m_vertexInput.attributes[ i ];

            m_rVkPipelineState.vkAttributes_[ i ] = {
                 .location = attr.location, .binding = attr.binding,  .offset = (uint32_t)attr.offset };

            if(!bufferAlreadyBound[ attr.binding ])
            {
                bufferAlreadyBound[ attr.binding ] = true;
                m_rVkPipelineState.vkBindings_[ m_rVkPipelineState.numBindings_++ ] = {
                     .binding = attr.binding, .stride = m_vertexInput.inputBindings[ attr.binding ].stride, .inputRate = VK_VERTEX_INPUT_RATE_VERTEX };
            }
        }
    }

}

IFNITY_END_NAMESPACE