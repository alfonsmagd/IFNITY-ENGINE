
#include "vk_backend.hpp"
#include "Ifnity\Graphics\Utils.hpp"
#include "UtilsVulkan.h"

//GLSLANG INCLUDES
#include <glslang\Public\resource_limits_c.h>
#include <spirv_cross/spirv_reflect.hpp>
#include "DeviceVulkan.h"
#include "../Vulkan/vulkan_PipelineBuilder.hpp"

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
		destroyShaderModule();
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
		GraphicsPipeline* pipeline = new GraphicsPipeline(std::move(desc), m_DeviceVulkan);


		auto& vert = m_vertex.emplace_back( createShaderModule(vShaderCode, vertexCode.size(), VK_SHADER_STAGE_VERTEX_BIT, vsbinary, "Vertex Shader"));
		auto& frag = m_fragment.emplace_back( createShaderModule(fShaderCode, fragmentCode.size(), VK_SHADER_STAGE_FRAGMENT_BIT, fsbinary, "Fragment Shader"));

		//3. Create the pipeline and configure colorFormat,
		const DeviceVulkan& ctx = getDeviceContextVulkan();

		pipeline->setColorFormat(GetRHIFormat(ctx.GetSwapChainFormat())); //Get the SwapChain Color Format 
		pipeline->passSpecializationConstantToVkFormat();
		pipeline->configureRenderPipelineState();
		pipeline->m_fragment = *frag.get();
		pipeline->m_vertex =   *vert.get();
		
		
		pipeline->ownerHandle_ = m_DeviceVulkan->slotMapRenderPipelines_.create(std::move(*pipeline));

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

	VkPipeline Device::getVkPipeline(GraphicsPipelineHandleSM handle) const
	{
		// Steps to follow:
		// 1. Validate the GraphicsPipeline pointer.
		// 2. Check if the pipeline is already created.
		// 3. Initialize the necessary variables for pipeline creation.
		// 4. Create the color blend states.
		// 5. Create VkPipelineVertexInputStateCreateInfo.
		// 6. Configure specialization constants and push constant ranges.
		// 7. Create VkPipelineShaderStageCreateInfo for each shader module.
		// 8. Create VkPipelineLayout.
		// 9. Build the pipeline using VulkanPipelineBuilder.
		#define VSHADER 0
		#define FSHADER 1

		IFNITY_ASSERT_MSG(handle.valid(), "GraphicsPipeline is null");

		GraphicsPipeline* gp = m_DeviceVulkan->slotMapRenderPipelines_.get(handle);



		RenderPipelineState* rps = gp->getRenderPipelineStatePtr();

		if(rps->pipeline_ != VK_NULL_HANDLE)
		{
			return rps->pipeline_;
		}

		// build a new Vulkan pipeline

		VkPipelineLayout layout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;

		//Initialize and get information to build diferent stages of the pipeline
		const GraphicsPipelineDescription& desc = gp->m_Description;
		const BlendState& blendstate = desc.renderState.blendState;
		SpecializationConstantDesc& specInfo = gp->specInfo;

		//Todo: get the shader modules from the pipeline description TESSELATION, MESH, GEOM
		const ShaderModuleState* vertModule = gp->getVertexShaderModule();
		const ShaderModuleState* fragModule = gp->getFragmentShaderModule();

		const uint32_t samplesCount = gp->samplesCount;
		const float minSampleShading = gp->minSampleShading;

		StencilState& backFaceStencil = gp->backFaceStencil;
		StencilState& frontFaceStencil = gp->frontFaceStencil;


		//const uint32_t numColorAttachments = &gp->m_rVkPipelineState.numColorAttachments_; only one color attachment format 

		// Not all attachments are valid. We need to create color blend attachments only for active attachments
		VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[ MAX_COLOR_ATTACHMENTS ] = {};
		VkFormat colorAttachmentFormats[ MAX_COLOR_ATTACHMENTS ] = {};

		// 4. Create the Color Blend Attachment States

		const auto& attachment = gp->colorFormat;
		IFNITY_ASSERT_MSG(attachment != rhi::Format::UNKNOWN, "format invalid");
		colorAttachmentFormats[ 0 ] = VK_FORMAT_B8G8R8A8_UNORM;



		if(!blendstate.blendEnable)
		{
			colorBlendAttachmentStates[ 0 ] = VkPipelineColorBlendAttachmentState
			{
				 .blendEnable = VK_FALSE,
				 .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
				 .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
				 .colorBlendOp = VK_BLEND_OP_ADD,
				 .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
				 .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
				 .alphaBlendOp = VK_BLEND_OP_ADD,
				 .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			};
		}
		else
		{
			colorBlendAttachmentStates[ 0 ] = VkPipelineColorBlendAttachmentState
			{
				 .blendEnable = VK_TRUE,
				 .srcColorBlendFactor = ConvertToVkBlendFactor(blendstate.srcColorBlendFactor),
				 .dstColorBlendFactor = ConvertToVkBlendFactor(blendstate.dstColorBlendFactor),
				 .colorBlendOp = ConvertToVkBlendOp(blendstate.colorBlendOp),
				 .srcAlphaBlendFactor = ConvertToVkBlendFactor(blendstate.srcAlphaBlendFactor),
				 .dstAlphaBlendFactor = ConvertToVkBlendFactor(blendstate.dstAlphaBlendFactor),
				 .alphaBlendOp = ConvertToVkBlendOp(blendstate.alphaBlendOp),
				 .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			};
		} //end if blendEnable

		//5. Create VkPipelineVertexInputStateCreateInfo

		const VkPipelineVertexInputStateCreateInfo ciVertexInputState =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = rps->numBindings_,
			.pVertexBindingDescriptions = rps->numBindings_ ? rps->vkBindings_ : nullptr,
			.vertexAttributeDescriptionCount = rps->numAttributes_,
			.pVertexAttributeDescriptions = rps->numAttributes_ ? rps->vkAttributes_ : nullptr,
		};

		//6. PushConstant Vulkan Range and Specialization Info
		VkSpecializationMapEntry entries[ SpecializationConstantDesc::SPECIALIZATION_CONSTANTS_MAX ] = {};
		const VkSpecializationInfo si = getPipelineShaderStageSpecializationInfo(specInfo, entries);


		//7. Create VkPipelineShaderStageCreateInfo for each shader module

		VkPipelineShaderStageCreateInfo shaderStages[ 2 ] = {};
		shaderStages[ VSHADER ] = getPipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertModule->sm, "main", &si);
		shaderStages[ FSHADER ] = getPipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragModule->sm, "main", &si);

		#define UPDATE_PUSH_CONSTANT_SIZE(sm, bit)                                  \
			if (sm) {                                                                 \
				pushConstantsSize = std::max(pushConstantsSize, sm->pushConstantsSize); \
				rps->shaderStageFlags_ |= bit;                                          \
		}
		rps->shaderStageFlags_ = 0;
		uint32_t pushConstantsSize = 0;
		UPDATE_PUSH_CONSTANT_SIZE(vertModule, VK_SHADER_STAGE_VERTEX_BIT);
		UPDATE_PUSH_CONSTANT_SIZE(fragModule, VK_SHADER_STAGE_FRAGMENT_BIT);

		#undef UPDATE_PUSH_CONSTANT_SIZE

		//maxPushConstantsSize is guaranteed to be at least 128 bytes, now in Vulkan 1.4 is 256 bytes
		// https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#features-limits
		// Table 32. Required Limits
		const VkPhysicalDeviceLimits& limits = getPhysicalDeviceLimits();
		if(!(pushConstantsSize <= limits.maxPushConstantsSize))
		{
			//Fill and complete with IFNITY_LOG 
			IFNITY_LOG(LogApp, ERROR,
				STRMESSAGE("Push constants size exceeded ", std::to_string(pushConstantsSize))
				+ STRMESSAGE(" (max ", std::to_string(limits.maxPushConstantsSize)) + " bytes)");
		}

		//7. Create VkPipelineLayout
		auto& vkDsl = m_DeviceVulkan->vkDSL_;  // get the descriptor set layout from the deviceVulkan context.

		const VkDescriptorSetLayout dsls[] = { vkDsl ,vkDsl };
		const VkPushConstantRange range =
		{
			 .stageFlags = rps->shaderStageFlags_,
			 .offset = 0,
			 .size = pushConstantsSize,
		};
		const VkPipelineLayoutCreateInfo ci =
		{
			 .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			 .setLayoutCount = (uint32_t)ARRAY_NUM_ELEMENTS(dsls),
			 .pSetLayouts = dsls,
			 .pushConstantRangeCount = pushConstantsSize ? 1u : 0u,
			 .pPushConstantRanges = pushConstantsSize ? &range : nullptr,
		};
		VK_ASSERT(vkCreatePipelineLayout(vkDevice_, &ci, nullptr, &layout), "Fail CREATE PIPELINE LAYOUT IN getVkPipeline");
		char pipelineLayoutName[ 256 ] = { 0 };
		if(desc.debugName)
		{
			snprintf(pipelineLayoutName, sizeof(pipelineLayoutName) - 1, "Pipeline Layout: %s", desc.debugName);
		}
		VK_ASSERT(setDebugObjectName(vkDevice_, VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)layout, pipelineLayoutName));



		//8. Create BuilderPipeline 
		VulkanPipelineBuilder()
			// from Vulkan 1.0
			.dynamicState(VK_DYNAMIC_STATE_VIEWPORT)
			.dynamicState(VK_DYNAMIC_STATE_SCISSOR)
			.dynamicState(VK_DYNAMIC_STATE_DEPTH_BIAS)
			.dynamicState(VK_DYNAMIC_STATE_BLEND_CONSTANTS)
			// from Vulkan 1.3 or VK_EXT_extended_dynamic_state
			.dynamicState(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE)
			.dynamicState(VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE)
			.dynamicState(VK_DYNAMIC_STATE_DEPTH_COMPARE_OP)
			// from Vulkan 1.3 or VK_EXT_extended_dynamic_state2
			.dynamicState(VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE)
			.primitiveTopology(ConvertToVkPrimitiveTopology(desc.rasterizationState.primitiveType))
			.polygonMode(ConverToVkPolygonMode(desc.rasterizationState.polygonMode))
			.stencilStateOps(VK_STENCIL_FACE_FRONT_BIT,
				ConvertstencilOpToVkStencilOp(frontFaceStencil.stencilFailureOp),
				ConvertstencilOpToVkStencilOp(frontFaceStencil.depthStencilPassOp),
				ConvertstencilOpToVkStencilOp(frontFaceStencil.depthFailureOp),
				compareOpToVkCompareOp(frontFaceStencil.stencilCompareOp))
			.stencilStateOps(VK_STENCIL_FACE_BACK_BIT,
				ConvertstencilOpToVkStencilOp(backFaceStencil.stencilFailureOp),
				ConvertstencilOpToVkStencilOp(backFaceStencil.depthStencilPassOp),
				ConvertstencilOpToVkStencilOp(backFaceStencil.depthFailureOp),
				compareOpToVkCompareOp(backFaceStencil.stencilCompareOp))
			.rasterizationSamples(getVulkanSampleCountFlags(samplesCount,
				m_DeviceVulkan->getFramebufferMSAABitMask()),
				minSampleShading)
			.stencilMasks(VK_STENCIL_FACE_FRONT_BIT, 0xFF, frontFaceStencil.writeMask, frontFaceStencil.readMask)
			.stencilMasks(VK_STENCIL_FACE_BACK_BIT, 0xFF, backFaceStencil.writeMask, backFaceStencil.readMask)
			.shaderStage(shaderStages[ VSHADER ])
			.shaderStage(shaderStages[ FSHADER ])
			.cullMode(cullModeToVkCullMode(desc.rasterizationState.cullMode))
			.frontFace(windingModeToVkFrontFace(desc.rasterizationState.frontFace))
			.vertexInputState(ciVertexInputState)
			.colorAttachments(colorBlendAttachmentStates, colorAttachmentFormats, 1)
			.depthAttachmentFormat(formatToVkFormat(gp->depthFormat))
			.stencilAttachmentFormat(formatToVkFormat(gp->stencilFormat))
			.build(vkDevice_, m_DeviceVulkan->pipelineCache_, layout, &pipeline, desc.debugName);


		rps->pipeline_ = pipeline;
		rps->pipelineLayout_ = layout;

		//9. 

		#undef VSHADER
		#undef FSHADER

		m_DeviceVulkan->addGraphicsPipeline(gp);


		return pipeline;


	}

	void Device::setActualPipeline(GraphicsPipeline* pipeline)
	{

		m_DeviceVulkan->actualPipeline_ = pipeline;
	}

	void Device::destroyShaderModule()
	{

		for(auto& vertex : m_vertex)
		{
			if(vertex)
			{
				IFNITY_LOG(LogApp, INFO, "Destroy Vertex Shader Module");
				vertex.reset();
				//Trace 
			}
		}
		
		for(auto& fragment : m_fragment)
		{
			if(fragment)
			{
				IFNITY_LOG(LogApp, INFO, "Destroy Fragment Shader Module");
				fragment.reset();
				//Trace 
			}
		}

	}



	HolderShaderSM Device::createShaderModuleFromSpirV(const void* spirv, size_t numBytes, const char* debugName) const
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
				return {};
			}

			size_t numElements = numBytes / sizeof(uint32_t);

			//todo: move to heap compilerRefelction to stack 

			spirv_cross::CompilerReflection compiler((const uint32_t*)spirv, numElements);

			// Refleja los recursos del shader
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();

			uint32_t pushConstantSize = 0;

			// Itera sobre las push constants y calcula el tamaño total
			for(const auto& pushConstant : resources.push_constant_buffers)
			{
				const spirv_cross::SPIRType& type = compiler.get_type(pushConstant.base_type_id);
				pushConstantSize += compiler.get_declared_struct_size(type);
			}

			
			ShaderModuleState smstate = { .sm = vkShaderModule, .pushConstantsSize = pushConstantSize };

			//Create the handle 
			ShaderModuleHandleSM handle = m_DeviceVulkan->slotMapShaderModules_.create(std::move(smstate));

			//Make holder 
			return  makeHolder(m_DeviceVulkan, handle, m_DeviceVulkan->slotMapShaderModules_);

		}
	}

	HolderShaderSM Device::createShaderModule(const char* shaderCode, size_t codeSize, VkShaderStageFlagBits stage, bool isBinary, const char* debugName) const
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

	const VkPhysicalDeviceLimits& Device::getPhysicalDeviceLimits() const
	{
		// TODO: Insertar una instrucción "return" aquí
		IFNITY_ASSERT_MSG(m_DeviceVulkan != VK_NULL_HANDLE, "VkPhysicalDevice is null");
		return m_DeviceVulkan->properties2.properties.limits;

	}



	//==================================================================================================//
	//  GraphicsPipeline METHODS                                                                      //
	//==================================================================================================//

	GraphicsPipeline::~GraphicsPipeline()
	{
		DestroyPipeline(m_DeviceVulkan->device_);
	}
	GraphicsPipeline::GraphicsPipeline(GraphicsPipelineDescription&& desc, DeviceVulkan* dev): m_Description(std::move(desc)), m_DeviceVulkan(dev)
	{}

	void GraphicsPipeline::BindPipeline(IDevice* device)
	{
		Device* vkDevice = dynamic_cast<Device*>(device);
		IFNITY_ASSERT_MSG(vkDevice != nullptr, "Device is not a Vulkan Device");


		if(ownerHandle_.valid())
		{
			auto vkpipeline = vkDevice->getVkPipeline(ownerHandle_);
		}
		


		vkDevice->setActualPipeline(this);


	}

	ShaderModuleState* GraphicsPipeline::getVertexShaderModule()
	{
		if(m_vertex.valid())
		{
			   ShaderModuleState* mvert = m_DeviceVulkan->slotMapShaderModules_.get(m_vertex);
			   return mvert;
		}
		IFNITY_LOG(LogApp, ERROR, "Vertex Shader Module State getHandle   its not valid");
		return nullptr;
	}

	ShaderModuleState* GraphicsPipeline::getFragmentShaderModule()
	{
		if(m_vertex.valid())
		{
			ShaderModuleState* frag = m_DeviceVulkan->slotMapShaderModules_.get(m_fragment);
			return frag;
		}
		IFNITY_LOG(LogApp, ERROR, "Vertex frag Module State getHandle   its not valid");
		
		return nullptr;
	}

	void GraphicsPipeline::setSpecializationConstant(const SpecializationConstantDesc& spec)
	{
		if(spec.data && spec.dataSize)
		{
			// copy into a local storage //First Reserve the memory,
			m_rVkPipelineState.specConstantDataStorage_ = malloc(spec.dataSize);
			memcpy(m_rVkPipelineState.specConstantDataStorage_, spec.data, spec.dataSize);
			//Copy the data in the memory VkPipeline 
			specInfo.data = m_rVkPipelineState.specConstantDataStorage_; // GetInformation from the data
		}


	}

	void GraphicsPipeline::DestroyPipeline(VkDevice device)
	{

		if(!destroy)
		{

			if(m_rVkPipelineState.pipeline_ != VK_NULL_HANDLE)
			{
				vkDestroyPipeline(device, m_rVkPipelineState.pipeline_, nullptr);

			}

			if(m_rVkPipelineState.pipelineLayout_ != VK_NULL_HANDLE)
			{
				vkDestroyPipelineLayout(device, m_rVkPipelineState.pipelineLayout_, nullptr);
			}
		}

		destroy = true;

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

			m_rVkPipelineState.vkAttributes_[ i ] =
			{
				 .location = attr.location, .binding = attr.binding,  .offset = (uint32_t)attr.offset
			};

			if(!bufferAlreadyBound[ attr.binding ])
			{
				bufferAlreadyBound[ attr.binding ] = true;
				m_rVkPipelineState.vkBindings_[ m_rVkPipelineState.numBindings_++ ] =
				{
					 .binding = attr.binding, .stride = m_vertexInput.inputBindings[ attr.binding ].stride, .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
				};
			}
		}//end for

		//Set the specialization constant
		setSpecializationConstant(specInfo);


	}

	void GraphicsPipeline::passSpecializationConstantToVkFormat()
	{

		for(const auto& spec : m_Description.specInfo)
		{
			specInfo.entries[ spec.id ] = {
			.constantId = spec.id,
			.offset = spec.offset,
			.size = spec.size
			};
			specInfo.data = spec.data;
			specInfo.dataSize += spec.dataSize;
		}


	}

}

IFNITY_END_NAMESPACE