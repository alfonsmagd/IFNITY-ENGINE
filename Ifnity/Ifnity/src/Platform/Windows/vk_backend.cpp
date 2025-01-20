
#include "vk_backend.hpp"
#include "Ifnity\Graphics\Utils.hpp"
#include "UtilsVulkan.h"

//GLSLANG INCLUDES
#include <glslang\Public\resource_limits_c.h>
#include <spirv_cross/spirv_reflect.hpp>

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
		GraphicsPipeline* pipeline = new GraphicsPipeline();

        std::vector<uint8_t> spirv;
        compileShaderVK(VK_SHADER_STAGE_VERTEX_BIT, vShaderCode, &spirv, glslang_default_resource());

		createShaderModuleFromSpirV(spirv.data(), spirv.size(), "Vertex Shader");


       
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

    ShaderModuleState Device::createShaderModuleFromSpirV(const void* spirv, size_t numBytes, const char* debugName)
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

    

	//==================================================================================================//
	//  GraphicsPipeline METHODS                                                                      //
	//==================================================================================================//
	
	void GraphicsPipeline::BindPipeline( IDevice* device)
	{
		// Not implemented yet
		throw std::runtime_error("The function or operation is not implemented.");
	}

}

IFNITY_END_NAMESPACE