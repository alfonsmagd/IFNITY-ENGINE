
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
	//-------------------------------------------------//
	//  UTILS METHODS INSIDE FILE IMPLEMENTATION       //
	//-------------------------------------------------//

	bool validateImageLimits(VkImageType imageType,
							 VkSampleCountFlagBits samples,
							 const VkExtent3D& extent,
							 const VkPhysicalDeviceLimits& limits
	)
	{

		if( samples != VK_SAMPLE_COUNT_1_BIT && !IFNITY_VERIFY(imageType == VK_IMAGE_TYPE_2D) )
		{
			IFNITY_LOG(LogCore, ERROR, "Sample count > 1 only supported for 2D images");
			return false;
		}

		if( imageType == VK_IMAGE_TYPE_2D &&
		   !IFNITY_VERIFY(extent.width <= limits.maxImageDimension2D &&
						  extent.height <= limits.maxImageDimension2D) )
		{

			IFNITY_LOG(LogCore, ERROR, "2D texture size exceeded");
			return false;
		}
		if( imageType == VK_IMAGE_TYPE_3D &&
		   !IFNITY_VERIFY(extent.width <= limits.maxImageDimension3D &&
						  extent.height <= limits.maxImageDimension3D &&
						  extent.depth <= limits.maxImageDimension3D) )
		{
			IFNITY_LOG(LogCore, ERROR, "3D texture size exceeded");
			return false;
		}

		return true;
	}

	/**
	* @brief Validates the range of a texture.
	*
	* This function checks if the specified range of a texture is valid based on the provided texture extent and number of mip levels.
	*
	* @param ext The extent (dimensions) of the texture.
	* @param numLevels The number of mip levels in the texture.
	* @param range The range description to validate.
	* @return true if the range is valid, false otherwise.
	*/
	bool validateRange(const VkExtent3D& ext, uint32_t numLevels, const TextureRangeDesc& range)
	{
		bool isValidate = true;

		if( !IFNITY_VERIFY(range.dimensions.width > 0 && range.dimensions.height > 0 || range.dimensions.depth > 0 || range.numLayers > 0 ||
						   range.numMipLevels > 0) )
		{
			IFNITY_LOG(LogCore, ERROR, "Invalid range dimensions");
			return false;
		}
		if( range.mipLevel > numLevels )
		{
			IFNITY_LOG(LogCore, ERROR, "Mip level out of range");
			return false;

		}

		const uint32_t texWidth = std::max(ext.width >> range.mipLevel, 1u);
		const uint32_t texHeight = std::max(ext.height >> range.mipLevel, 1u);
		const uint32_t texDepth = std::max(ext.depth >> range.mipLevel, 1u);

		if( range.dimensions.width > texWidth ||
		   range.dimensions.height > texHeight ||
		   range.dimensions.depth > texDepth )
		{
			IFNITY_LOG(LogCore, ERROR, "range dimensions exceed texture dimensions");
			return false;
		}
		if( range.offset.x > texWidth - range.dimensions.width ||
		   range.offset.y > texHeight - range.dimensions.height ||
		   range.offset.z > texDepth - range.dimensions.depth )
		{
			IFNITY_LOG(LogCore, ERROR, "range offset exceeds texture dimensions");
			return false;
		}

		return isValidate;
	}


	//-----------------------------------------------//
	// Device METHODS                                //
	//-----------------------------------------------//
	Device::Device(VkDevice vkDevice, DeviceVulkan* ptr): vkDevice_(vkDevice), m_DeviceVulkan(ptr)
	{

		IFNITY_ASSERT_MSG(vkDevice_ != VK_NULL_HANDLE, "VkDevice is null");
		IFNITY_ASSERT_MSG(m_DeviceVulkan != nullptr, "DeviceVulkan is null");

		//set the stagindevice

		vkPhysicalDevice_ = m_DeviceVulkan->getPhysicalDevice();

		IFNITY_ASSERT_MSG(vkPhysicalDevice_ != VK_NULL_HANDLE, "VkPhysicalDevice is null, creatin device");



	}



	Device::~Device()
	{
		destroyShaderModule();
		//Destroy the dummy texture


	}

	void Device::Draw(DrawDescription& desc)
	{






	}

	//TODO: Implement this function texdesc information
	void Device::DrawObject(GraphicsPipelineHandle& pipeline, DrawDescription& desc)
	{
		DepthState depthState = { .compareOp = rhi::CompareOp::CompareOp_Less, .isDepthWriteEnabled = desc.depthTest };


		cmdBuffer.cmdBindDepthState(depthState);
		cmdBuffer.cmdSetDepthBiasEnable(desc.enableBias);
		cmdBuffer.cmdSetDepthBias(desc.depthBiasValues.Constant,
								  desc.depthBiasValues.Clamp,
								  desc.depthBiasValues.Clamp);

		pipeline->BindPipeline(this);//This set pipeline like actualpilenine in VK.



		cmdBuffer.cmdBindRenderPipeline(*m_DeviceVulkan->actualPipeline_);

		//for each vertex buffer bind 
		for( auto& vb : m_vertexBuffer )
		{

			cmdBuffer.cmdBindVertexBuffer(0, vb);
		}
		for( auto& ib : m_indexBuffer )
		{
			if( !ib.valid() ) continue;
			cmdBuffer.cmdBindIndexBuffer(ib, rhi::IndexFormat::IndexFormat_UINT32);
		}


		cmdBuffer.cmdBindDepthState(depthState);
		cmdBuffer.cmdPushConstants(pushConstants.data,
								   pushConstants.size,
								   pushConstants.offset);


		cmdBuffer.cmdDraw(desc.drawMode, desc.size, desc.instanceCount);

	}

	void Device::StartRecording()
	{
		DepthState depthState;




		Vulkan::CommandBuffer& cmdb = m_DeviceVulkan->acquireCommandBuffer();

		cmdBuffer = std::move(cmdb);

		//Get handler current texture 
		currentTexture_ = m_DeviceVulkan->getCurrentSwapChainTexture();

		//Default RenderPass and FrameBuffer by default, this will be move 
		Vulkan::RenderPass renderPass = {
		.color = { {.loadOp = Vulkan::LoadOp_Clear, .clearColor = { 1.0f, 1.0f, 1.0f, 1.0f } } } };

		Vulkan::Framebuffer framebuffer = { .color = { {.texture = currentTexture_ } } };
		//Get if we have internal depth texture
		if( depthTexture_.valid() )
		{
			framebuffer.depthStencil = { .texture = depthTexture_ };
			renderPass.depth = { .loadOp = Vulkan::LoadOp_Clear, .clearDepth = 1.0f };
			depthState = { .compareOp = rhi::CompareOp::CompareOp_Less, .isDepthWriteEnabled = true };


		}


		//Start Rendering
		cmdBuffer.cmdBeginRendering(renderPass, framebuffer);



	}

	void Device::StopRecording()
	{

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer.wrapper_->cmdBuf_);
		cmdBuffer.cmdEndRendering();
		m_DeviceVulkan->submit(cmdBuffer, currentTexture_);

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

		if( !vs || !fs )
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
		catch( const std::runtime_error& e )
		{
			IFNITY_LOG(LogApp, ERROR, e.what());
			return GraphicsPipelineHandle{};
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = gs ? geometryCode.c_str() : nullptr;

		// 2. compile shaders
		GraphicsPipeline* pipeline = new GraphicsPipeline(std::move(desc), m_DeviceVulkan);


		auto& vert = m_shaderVert.emplace_back(createShaderModule(vShaderCode, vertexCode.size(), VK_SHADER_STAGE_VERTEX_BIT, vsbinary, "Vertex Shader"));
		auto& frag = m_shaderFragment.emplace_back(createShaderModule(fShaderCode, fragmentCode.size(), VK_SHADER_STAGE_FRAGMENT_BIT, fsbinary, "Fragment Shader"));


		//3. Create the pipeline and configure colorFormat,
		const DeviceVulkan& ctx = getDeviceContextVulkan();

		pipeline->setColorFormat(getRHIFormat(ctx.GetSwapChainFormat())); //Get the SwapChain Color Format 
		pipeline->setDepthFormat(pipeline->GetGraphicsPipelineDesc().renderState.depthFormat);
		pipeline->passSpecializationConstantToVkFormat();
		pipeline->configureVertexAttributes();
		pipeline->m_shaderFragment = *frag.get();
		pipeline->m_shaderVert = *vert.get();



		pipeline->ownerHandle_ = m_DeviceVulkan->slotMapRenderPipelines_.create(std::move(*pipeline));

		return GraphicsPipelineHandle(pipeline);


	}

	BufferHandle Device::CreateBuffer(const BufferDescription& desc)
	{
		//auxiliar storagetype config 
		StorageType storage = desc.storage;

		//This is a constant buffer and Vulkan Constant Buffer is inside like a PushConstant and manage	internal 
		if( desc.type == BufferType::CONSTANT_BUFFER )
		{
			IFNITY_LOG(LogCore, INFO, "Constant Buffer its managed in vulkan like push constants inside ");
			Buffer* buff = new Buffer(desc);
			return BufferHandle(buff);

		}

		if( desc.type == BufferType::NO_DEFINE_BUFFER )
		{
			IFNITY_LOG(LogCore, WARNING, "No define buffer "); return{};
		}


		if( !m_DeviceVulkan->useStaging_ && (desc.storage == StorageType::DEVICE) )
		{
			storage = StorageType::HOST_VISIBLE;
		}

		// Use staging device to transfer data into the buffer when the storage is private to the device
		VkBufferUsageFlags usageFlags = (desc.storage == StorageType::DEVICE) ?
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0;


		//Generate Flags and MemFlags using BufferDescription 
		usageFlags |= Vulkan::getVkBufferUsageFlags(desc.type);
		VkMemoryPropertyFlags memFlags = storageTypeToVkMemoryPropertyFlags(storage);

		//Create the buffer holder 
		HolderBufferSM buffer = CreateInternalVkBuffer(desc.byteSize, usageFlags, memFlags, desc.debugName.c_str());

		//Check if the buffer its created
		IFNITY_ASSERT_MSG(buffer, "Buffer its not created internal ");

		//Upload data if the bufferdesc have it; 
		if( desc.data )
		{
			upload(*buffer, desc.data, desc.byteSize, desc.offset);
		}


		//Check if is a vertex buffer or index buffer 
		Buffer* handle = new Buffer(desc, std::move(buffer));
		//Try to set the buffer gpu address
		handle->getBufferGpuAddress(*m_DeviceVulkan);

		IFNITY_LOG(LogCore, INFO, STRMESSAGE("BufferCreation: ", desc.debugName));

		return BufferHandle(handle);

	}

	HolderBufferSM Device::CreateInternalVkBuffer(VkDeviceSize bufferSize,
												  VkBufferUsageFlags usageFlags,
												  VkMemoryPropertyFlags memFlags,
												  const char* debugName)
	{
		//Check the buffersize has valid value
		IFNITY_ASSERT_MSG(bufferSize > 0, "Buffer size is invalid");

		//Get limits to verify the buffer size its 
		const VkPhysicalDeviceLimits& limits = m_DeviceVulkan->GetPhysicalDeviceLimits();

		if( usageFlags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT )
		{
			if( !(bufferSize <= limits.maxUniformBufferRange) )
			{
				IFNITY_LOG(LogCore, ERROR, "Buffer size exceeded VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT");
				return {};
			}
		}

		if( usageFlags & VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM )
		{
			if( !(bufferSize <= limits.maxStorageBufferRange) )
			{
				IFNITY_LOG(LogCore, ERROR, "Buffer size exceeded VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM");
				return {};
			}
		}

		VulkanBuffer buf =
		{
		 .bufferSize_ = bufferSize,
		 .vkUsageFlags_ = usageFlags,
		 .vkMemFlags_ = memFlags,
		};

		const VkBufferCreateInfo ci =
		{
			 .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			 .pNext = nullptr,
			 .flags = 0,
			 .size = bufferSize,
			 .usage = usageFlags,
			 .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			 .queueFamilyIndexCount = 0,
			 .pQueueFamilyIndices = nullptr,
		};

		if( VMA_ALLOCATOR_VK )
		{
			VmaAllocationCreateInfo vmaAllocInfo = {};

			// Initialize VmaAllocation Info
			if( memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT )
			{
				vmaAllocInfo = {
					 .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
					 .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
					 .preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
				};
			}

			if( memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT )
			{
				// Check if coherent buffer is available.
				VK_ASSERT(vkCreateBuffer(vkDevice_, &ci, nullptr, &buf.vkBuffer_));
				VkMemoryRequirements requirements = {};
				vkGetBufferMemoryRequirements(vkDevice_, buf.vkBuffer_, &requirements);
				vkDestroyBuffer(vkDevice_, buf.vkBuffer_, nullptr);
				buf.vkBuffer_ = VK_NULL_HANDLE;

				if( requirements.memoryTypeBits & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT )
				{
					vmaAllocInfo.requiredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
					buf.isCoherentMemory_ = true;
				}
			}

			vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

			vmaCreateBuffer((VmaAllocator)m_DeviceVulkan->getVmaAllocator(),
							&ci,
							&vmaAllocInfo,
							&buf.vkBuffer_,
							&buf.vmaAllocation_,
							nullptr);

			// handle memory-mapped buffers
			if( memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT )
			{
				vmaMapMemory((VmaAllocator)m_DeviceVulkan->getVmaAllocator(),
							 buf.vmaAllocation_,
							 &buf.mappedPtr_);
			}
		}
		//Not using VMA Allocator
		else
		{
			// create buffer
			IFNITY_ASSERT(vkCreateBuffer(vkDevice_, &ci, nullptr, &buf.vkBuffer_));

			// back the buffer with some memory
			{
				VkMemoryRequirements requirements = {};
				vkGetBufferMemoryRequirements(vkDevice_, buf.vkBuffer_, &requirements);
				if( requirements.memoryTypeBits & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT )
				{
					buf.isCoherentMemory_ = true;
				}

				IFNITY_ASSERT(allocateMemory(vkPhysicalDevice_, vkDevice_, &requirements, memFlags, &buf.vkMemory_));
				IFNITY_ASSERT(vkBindBufferMemory(vkDevice_, buf.vkBuffer_, buf.vkMemory_, 0));
			}

			// handle memory-mapped buffers
			if( memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT )
			{
				IFNITY_ASSERT(vkMapMemory(vkDevice_, buf.vkMemory_, 0, buf.bufferSize_, 0, &buf.mappedPtr_));
			}

		}//end of else createbuffer. 

		IFNITY_ASSERT(buf.vkBuffer_ != VK_NULL_HANDLE);

		// set debug name
		VK_ASSERT(setDebugObjectName(vkDevice_, VK_OBJECT_TYPE_BUFFER, (uint64_t)buf.vkBuffer_, debugName));

		// handle shader access
		if( usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT )
		{
			const VkBufferDeviceAddressInfo ai = {
				 .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				 .buffer = buf.vkBuffer_,
			};
			buf.vkDeviceAddress_ = vkGetBufferDeviceAddress(vkDevice_, &ai);
			IFNITY_ASSERT(buf.vkDeviceAddress_);
		}

		BufferHandleSM buffhandle = m_DeviceVulkan->slotMapBuffers_.create(std::move(buf));

		return makeHolder(m_DeviceVulkan, buffhandle);

	}





	void Device::upload(BufferHandleSM& buffer, const void* data, size_t size, uint32_t offset)
	{

		//Previos check if the buffer is null and check it 
		if( !data )
		{
			IFNITY_LOG(LogCore, ERROR, "Data is null to upload ");
			return;
		}

		IFNITY_ASSERT_MSG(size, "Data size should be non-zero");

		VulkanBuffer* buf = m_DeviceVulkan->slotMapBuffers_.get(buffer);

		if( !buf )
		{
			IFNITY_LOG(LogCore, ERROR, "Buffer is null to upload ");
			return;
		}

		if( !IFNITY_VERIFY(offset + size <= buf->bufferSize_) )
		{
			return;
		}

		//Lets to staginDevice to upload data 
		m_DeviceVulkan->m_StagingDevice->bufferSubData(*buf, offset, size, data);



	}

	void Device::upload(Vulkan::VulkanBuffer* buffer, const void* data, size_t size, uint32_t offset)
	{

		//Previos check if the buffer is null and check it 
		if( !data )
		{
			IFNITY_LOG(LogCore, ERROR, "Data is null to upload ");
			return;
		}

		IFNITY_ASSERT_MSG(size, "Data size should be non-zero");


		if( !buffer )
		{
			IFNITY_LOG(LogCore, ERROR, "Buffer is null to upload ");
			return;
		}

		if( !IFNITY_VERIFY(offset + size <= buffer->bufferSize_) )
		{
			IFNITY_LOG(LogCore, ERROR, "Buffer is enough size ");
			return;
		}

		//Lets to staginDevice to upload data 
		m_DeviceVulkan->m_StagingDevice->bufferSubData(*buffer, offset, size, data);






	}

	void Device::upload(TextureHandleSM handle, const TextureRangeDesc& range, const void* data)
	{
		if( !data )
		{
			IFNITY_LOG(LogCore, ERROR, "Data is null to upload creatin texture vulkan ");
			return;
		}

		VulkanImage* texture = m_DeviceVulkan->slootMapTextures_.get(handle);

		if( !texture )
		{
			IFNITY_LOG(LogCore, ERROR, "Texture not create,error at get in slotmap");
			return;
		}


		if( !validateRange(texture->vkExtent_, texture->numLevels_, range) )
		{
			IFNITY_LOG(LogCore, ERROR, "Error at validate range");

		}

		const uint32_t numLayers = std::max(range.numLayers, 1u);


		//Get the format of the texture
		VkFormat vkFormat = texture->vkImageFormat_;

		//For now we only use 2D textures
		IFNITY_ASSERT(texture->vkType_ == VK_IMAGE_TYPE_2D);

		//Get the image region for build image. 
		const VkRect2D imageRegion = {
			.offset = {.x = range.offset.x, .y = range.offset.y},
			.extent = {.width = range.dimensions.width, .height = range.dimensions.height},
		};
		m_DeviceVulkan->m_StagingDevice->imageData2D(*texture, imageRegion, range.mipLevel, range.numMipLevels, range.layer, range.numLayers, vkFormat, data);







	}

	void Device::WriteBuffer(BufferHandle& buffer, const void* data, size_t size, uint32_t offset)
	{
		if( buffer->GetBufferDescription().type == BufferType::CONSTANT_BUFFER )
		{
			//Internal cache buffer 
			pushConstants.data = data;
			pushConstants.size = size;
			pushConstants.offset = offset;
		}
		if( buffer->GetBufferDescription().type == BufferType::UNIFORM_BUFFER )
		{
			//Get the BufferHandleSM by index  to avoid dynamic_cast. 
			Vulkan::VulkanBuffer* buf = m_DeviceVulkan->slotMapBuffers_.getByIndex(buffer->GetBufferID());
			if( !buf )
			{
				IFNITY_LOG(LogCore, ERROR, "Buffer is null to write ");
				return;
			}
			upload(buf, data, size, offset);

			if( buf->vkDeviceAddress_ )
			{
				//Update push constant
				pushConstants.data = &buf->vkDeviceAddress_;
				pushConstants.size = sizeof(uint64_t);
				pushConstants.offset = 0; // force to offset 0 in vulkan 

			}
		}

	}


	bool Device::validateTextureDescription(TextureDescription& texdesc)
	{
		const rhi::TextureType type = texdesc.dimension;
		if( !(type == TextureType::TEXTURE2D || type == TextureType::TEXTURECUBE || type == TextureType::TEXTURE3D) )
		{
			IFNITY_ASSERT(false, "Only 2D, 3D and Cube textures are supported");
			return false;
		}

		if( texdesc.mipLevels == 0 )
		{
			IFNITY_LOG(LogCore, WARNING, "The number of mip-levels is 0. Setting it to 1.");
			texdesc.mipLevels = 1;
		}

		if( texdesc.sampleCount > 1 && texdesc.mipLevels != 1 )
		{
			IFNITY_LOG(LogCore, WARNING, "Multisampled textures must have only one mip-level. Setting it to 1.");
			return false;
		}

		if( texdesc.sampleCount > 1 && type == rhi::TextureType::TEXTURE3D )
		{
			IFNITY_LOG(LogCore, WARNING, "Multisampled 3D textures are not supported. Setting it to 1.");
			texdesc.sampleCount = 1;
			return false;
		}

		if( !(texdesc.mipLevels <= Utils::getNumMipMapLevels2D(texdesc.dimensions.width, texdesc.dimensions.height)) )
		{
			IFNITY_LOG(LogCore, WARNING, "The number of mip-levels is too high. Setting it to the maximum possible value.");
			texdesc.mipLevels = Utils::getNumMipMapLevels2D(texdesc.dimensions.width, texdesc.dimensions.height);
		}

		if( texdesc.usage == rhi::TextureUsageBits::UNKNOW )
		{
			IFNITY_LOG(LogCore, WARNING, "Texture usage is not set. Setting it to sampled.");
			texdesc.usage = rhi::TextureUsageBits::SAMPLED;
		}

		return true;
	}

	VkImageUsageFlags Device::getImageUsageFlags(const TextureDescription& texdesc)
	{
		VkImageUsageFlags usageFlags = (texdesc.storage == StorageType::DEVICE) ? VK_IMAGE_USAGE_TRANSFER_DST_BIT : 0;

		if( static_cast<uint8_t>(texdesc.usage) & static_cast<uint8_t>(rhi::TextureUsageBits::SAMPLED) )
		{
			usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		if( static_cast<uint8_t>(texdesc.usage) & static_cast<uint8_t>(rhi::TextureUsageBits::STORAGE) )
		{
			IFNITY_ASSERT_MSG(texdesc.sampleCount <= 1, "Storage images cannot be multisampled");
			usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		}
		if( static_cast<uint8_t>(texdesc.usage) & static_cast<uint8_t>(rhi::TextureUsageBits::ATTACHMENT) )
		{
			usageFlags |= isDepthFormat(texdesc.format) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				: VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			if( texdesc.storage == StorageType::MEMORYLESS )
			{
				usageFlags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
			}
		}

		if( texdesc.storage != StorageType::MEMORYLESS )
		{
			usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		return usageFlags;
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

	void Device::BindingVertexAttributesBuffer(BufferHandle& bf)
	{
		//Dynamic cast to VkDevice 
		// 
		Buffer* vkBuffer = dynamic_cast<Buffer*>(bf.get());
		if( !vkBuffer )
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to get VulkanBuffar dynamic cast");
			return;
		}

		//Get the holderbuffer and push back 
		m_vertexBuffer.push_back(vkBuffer->getBufferHandleSM());

		//m_indexBuffer.push_back(bf);
	}

	void Device::BindingIndexBuffer(BufferHandle& bf)
	{

		//Dynamic cast to VkDevice 
		// 
		Buffer* vkBuffer = dynamic_cast<Buffer*>(bf.get());
		if( !vkBuffer )
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to get VulkanBuffar dynamic cast");
			return;
		}

		//Get the holderbuffer and push back 
		m_indexBuffer.push_back(vkBuffer->getBufferHandleSM());
	}

	TextureHandle Device::CreateTexture(TextureDescription& desc)
	{
		using namespace rhi;

		TextureDescription texdesc(desc);
		const auto& dvk = *m_DeviceVulkan;

		//Validate and asure the texture description is valid , if some values are invalid, the function force
		//by default some values but not all. 
		if( !validateTextureDescription(texdesc) )
		{
			return {};
		}

		//Get the format value. 
		const VkFormat vkFormat = isDepthFormat(desc.format) ? getClosestDepthStencilFormat(desc.format)
			: formatToVkFormat(desc.format);


		IFNITY_ASSERT_MSG(vkFormat != VK_FORMAT_UNDEFINED, "Invalid VkFormat value");

		VkImageUsageFlags usageFlags = getImageUsageFlags(texdesc);

		const VkMemoryPropertyFlags memFlags = storageTypeToVkMemoryPropertyFlags(texdesc.storage);

		const bool hasDebugName = texdesc.debugName.size() > 0;

		char debugNameImage[ 256 ] = { 0 };
		char debugNameImageView[ 256 ] = { 0 };

		if( hasDebugName )
		{
			snprintf(debugNameImage, sizeof(debugNameImage) - 1, "Image: %s", texdesc.debugName);
			snprintf(debugNameImageView, sizeof(debugNameImageView) - 1, "Image View: %s", texdesc.debugName);
		}

		//Get the correct VkImageCreateFlags and properties to build texture desc.
		VkImageCreateFlags vkCreateFlags = 0;
		VkImageViewType vkImageViewType = VK_IMAGE_VIEW_TYPE_2D; //by default, for now
		VkImageType vkImageType = VK_IMAGE_TYPE_2D;
		VkSampleCountFlagBits vkSamples = VK_SAMPLE_COUNT_1_BIT;
		uint32_t numLayers = 1;

		switch( texdesc.dimension )
		{
			case TextureType::TEXTURE2D:
				vkImageViewType = VK_IMAGE_VIEW_TYPE_2D;
				vkImageType = VK_IMAGE_TYPE_2D;
				vkSamples = getVulkanSampleCountFlags(texdesc.sampleCount, m_DeviceVulkan->getFramebufferMSAABitMask());
				break;
			case TextureType::TEXTURECUBE:
				IFNITY_LOG(LogCore, ERROR, "Cubemaps are not supported yet");
				break;
			case TextureType::TEXTURE3D:
				IFNITY_LOG(LogCore, ERROR, "3D textures are not supported yet");
				break;


			default:
				IFNITY_LOG(LogCore, ERROR, "Unsupported texture type");

				return {};
		}

		const VkExtent3D vkExtent{ texdesc.dimensions.width, texdesc.dimensions.height, texdesc.dimensions.depth };
		const uint32_t numLevels = texdesc.mipLevels;

		//TODO: Complete 
		if( !IFNITY_VERIFY(validateImageLimits(vkImageType, vkSamples, vkExtent, m_DeviceVulkan->GetPhysicalDeviceLimits())) )
		{
			return {};
		}

		IFNITY_ASSERT_MSG(numLevels > 0, "The image must contain at least one mip-level");
		IFNITY_ASSERT_MSG(numLayers > 0, "The image must contain at least one layer");
		IFNITY_ASSERT_MSG(vkSamples > 0, "The image must contain at least one sample");
		IFNITY_ASSERT(vkExtent.width > 0);
		IFNITY_ASSERT(vkExtent.height > 0);
		IFNITY_ASSERT(vkExtent.depth > 0);

		VulkanImage image = {
			 .vkUsageFlags_ = usageFlags,
			 .vkExtent_ = vkExtent,
			 .vkType_ = vkImageType,
			 .vkImageFormat_ = vkFormat,
			 .vkSamples_ = vkSamples,
			 .numLevels_ = numLevels,
			 .numLayers_ = numLayers,
			 .isDepthFormat_ = VulkanImage::isDepthFormat(vkFormat),
			 .isStencilFormat_ = VulkanImage::isStencilFormat(vkFormat),
		};

		const uint32_t numPlanes = getNumImagePlanes(desc.format);

		const VkImageCreateInfo ci = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = vkCreateFlags,
			.imageType = vkImageType,
			.format = vkFormat,
			.extent = vkExtent,
			.mipLevels = numLevels,
			.arrayLayers = numLayers,
			.samples = vkSamples,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = usageFlags,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};

		if( VMA_ALLOCATOR_VK )
		{

			VmaAllocationCreateInfo vmaAllocInfo = {

				.usage = memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ? VMA_MEMORY_USAGE_CPU_TO_GPU : VMA_MEMORY_USAGE_AUTO,
			};


			VkResult result = vmaCreateImage((VmaAllocator)dvk.getVmaAllocator(),
											 &ci,
											 &vmaAllocInfo,
											 &image.vkImage_,
											 &image.vmaAllocation_,
											 nullptr);
			if( !(result == VK_SUCCESS) )
			{
				IFNITY_LOG(LogCore, ERROR, "Failed to create image in format correctly ");
				return {};
			}
			// handle memory-mapped buffers
			if( memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT )
			{
				vmaMapMemory((VmaAllocator)dvk.getVmaAllocator(), image.vmaAllocation_, &image.mappedPtr_);
			}
		}
		else
		{
			//Force creating depthsetncil flow 
		}
		// Creation debug name

		VK_ASSERT(setDebugObjectName(vkDevice_, VK_OBJECT_TYPE_IMAGE, (uint64_t)image.vkImage_, debugNameImage));

		//Get physical device's properties for the image's format
		vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice_, image.vkImageFormat_, &image.vkFormatProperties_);

		VkImageAspectFlags aspect = 0;
		if( image.isDepthFormat_ || image.isStencilFormat_ )
		{
			if( image.isDepthFormat_ || texdesc.isDepth )
			{
				aspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
			}
			if( image.isStencilFormat_ || texdesc.isStencil )
			{
				aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		image.imageView_ = image.createImageView(vkDevice_,
												 vkImageViewType,
												 vkFormat,
												 aspect,
												 0,
												 VK_REMAINING_MIP_LEVELS,
												 0,
												 numLayers,
												 {},
												 nullptr,
												 debugNameImageView);
		if( !IFNITY_VERIFY(image.imageView_ != VK_NULL_HANDLE) )
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to create image view");
			return {};
		}

		//Creating all 

		TextureHandleSM handle = m_DeviceVulkan->slootMapTextures_.create(std::move(image));

		HolderTextureSM holder = makeHolder(m_DeviceVulkan, handle);

		Texture* tex;

		if( holder.get()->valid() )
		{
			tex = new Texture(texdesc, std::move(holder));

		}
		else
		{
			delete tex;
			IFNITY_LOG(LogCore, ERROR, "Failed to create texture in vulkan , not error reporter before ");
			return {};
		}


		//Chek if texture has a valid value to use 
		if( desc.data )
		{
			//upload process to texture 
			upload(handle, { .dimensions = desc.dimensions, .numLayers = numLayers, .numMipLevels = 1 }, desc.data);

		}



		return TextureHandle(tex);


	}









	MeshObjectHandle Device::CreateMeshObject(const MeshObjectDescription& desc)
	{// Not implemented yet//Check if MeshData its valid ? 
		if(desc.meshData.indexData_.empty() || desc.meshData.vertexData_.empty())
		{
			IFNITY_LOG(LogApp, ERROR, "MeshData its invalid, are you sure that mesh Object Desc has data? , you have to build, or use a IMeshObjectBuilder");
			return nullptr;
		}
		// Check if ist not a large mesh 


		//For now its a largeMesh 
		else if(desc.isLargeMesh)
		{
			//Create a MeshObject with the data. 

			/*MeshObject* mesh = new MeshObject(&desc.meshFileHeader, desc.meshData.meshes_.data(), desc.meshData.indexData_.data(), desc.meshData.vertexData_.data(), this);*/
			MeshObject* mesh = new MeshObject(std::move(const_cast<MeshObjectDescription&>(desc)), this);

			return MeshObjectHandle(mesh);
		}


		return nullptr;
		
	}

	MeshObjectHandle Device::CreateMeshObject(const MeshObjectDescription& desc, IMeshDataBuilder* meshbuilder)
	{
		if(meshbuilder)
		{
			meshbuilder->buildMeshData(const_cast<MeshObjectDescription&>(desc));
			return CreateMeshObject(desc);
		}
		else
		{
			IFNITY_LOG(LogApp, ERROR, "MeshDataBuilder its invalid");
			return nullptr;
		}
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

	void Device::SetDepthTexture(TextureHandle texture)
	{
		Texture* vkdepth = dynamic_cast<Texture*>(texture.get());
		if( !vkdepth )
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to get VulkanTexture depht dynamic cast");
			return;
		}

		depthTexture_ = vkdepth->getTextureHandleSM();
		if( !depthTexture_.valid() )
		{
			IFNITY_LOG(LogCore, ERROR, "Failed to get VulkanTexture depht dynamic cast");
			depthTexture_ = {}; //invalid texture
		}


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

		IFNITY_ASSERT_MSG(handle.valid(), "GraphicsPipeline is null");

		GraphicsPipeline* gp = m_DeviceVulkan->slotMapRenderPipelines_.get(handle);



		RenderPipelineState* rps = gp->getRenderPipelineStatePtr();

		if( rps->pipeline_ != VK_NULL_HANDLE )
		{
			return rps->pipeline_;
		}

		// build a new Vulkan pipeline

		enum ShaderStage { VSHADER = 0, FSHADER = 1, GSHADER = 2, MAX_SHADER_STAGE = 3 };

		VkPipelineLayout layout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;

		//Initialize and get information to build diferent stages of the pipeline
		const GraphicsPipelineDescription& desc = gp->m_Description;
		const BlendState& blendstate = desc.renderState.blendState;
		SpecializationConstantDesc& specInfo = gp->specInfo;

		//Todo: get the shader modules from the pipeline description TESSELATION, MESH, GEOM
		const ShaderModuleState* vertModule = gp->getVertexShaderModule();
		const ShaderModuleState* fragModule = gp->getFragmentShaderModule();
		const ShaderModuleState* geomModule = gp->getGeometryShaderModule();

		const uint32_t samplesCount = gp->samplesCount;
		const float minSampleShading = gp->minSampleShading;

		StencilState& backFaceStencil = gp->backFaceStencil;
		StencilState& frontFaceStencil = gp->frontFaceStencil;


		const uint32_t numColorAttachments = 1;

		// Not all attachments are valid. We need to create color blend attachments only for active attachments
		VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[ MAX_COLOR_ATTACHMENTS ] = {};
		VkFormat colorAttachmentFormats[ MAX_COLOR_ATTACHMENTS ] = {};

		// 4. Create the Color Blend Attachment States

		const auto& attachment = gp->colorFormat;
		IFNITY_ASSERT_MSG(attachment != rhi::Format::UNKNOWN, "format invalid");
		colorAttachmentFormats[ 0 ] = VK_FORMAT_B8G8R8A8_UNORM;



		if( !blendstate.blendEnable )
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

		VkPipelineShaderStageCreateInfo shaderStages[ MAX_SHADER_STAGE ] = {};
		shaderStages[ VSHADER ] = getPipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertModule->sm, "main", &si);
		shaderStages[ FSHADER ] = getPipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragModule->sm, "main", &si);
		shaderStages[ GSHADER ] = geomModule ? getPipelineShaderStageCreateInfo(VK_SHADER_STAGE_GEOMETRY_BIT, geomModule->sm, "main", &si) : VkPipelineShaderStageCreateInfo{ .module = VK_NULL_HANDLE };


		#define UPDATE_PUSH_CONSTANT_SIZE(sm, bit)                                  \
			if (sm) {                                                                 \
				pushConstantsSize = std::max(pushConstantsSize, sm->pushConstantsSize); \
				rps->shaderStageFlags_ |= bit;                                          \
		}
		rps->shaderStageFlags_ = 0;
		uint32_t pushConstantsSize = 0;
		UPDATE_PUSH_CONSTANT_SIZE(vertModule, VK_SHADER_STAGE_VERTEX_BIT);
		UPDATE_PUSH_CONSTANT_SIZE(fragModule, VK_SHADER_STAGE_FRAGMENT_BIT);
		UPDATE_PUSH_CONSTANT_SIZE(geomModule, VK_SHADER_STAGE_GEOMETRY_BIT);

		#undef UPDATE_PUSH_CONSTANT_SIZE

		//maxPushConstantsSize is guaranteed to be at least 128 bytes, now in Vulkan 1.4 is 256 bytes
		// https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#features-limits
		// Table 32. Required Limits
		const VkPhysicalDeviceLimits& limits = getPhysicalDeviceLimits();
		if( !(pushConstantsSize <= limits.maxPushConstantsSize) )
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
		if( desc.debugName )
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
			.rasterizationSamples(getVulkanSampleCountFlags(samplesCount, m_DeviceVulkan->getFramebufferMSAABitMask()), minSampleShading)
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
			.stencilMasks(VK_STENCIL_FACE_FRONT_BIT, 0xFF, frontFaceStencil.writeMask, frontFaceStencil.readMask)
			.stencilMasks(VK_STENCIL_FACE_BACK_BIT, 0xFF, backFaceStencil.writeMask, backFaceStencil.readMask)
			.shaderStage(shaderStages[ VSHADER ])
			.shaderStage(shaderStages[ FSHADER ])
			.shaderStage(shaderStages[ GSHADER ])
			.cullMode(cullModeToVkCullMode(desc.rasterizationState.cullMode))
			.frontFace(windingModeToVkFrontFace(desc.rasterizationState.frontFace))
			.vertexInputState(ciVertexInputState)
			.colorAttachments(colorBlendAttachmentStates, colorAttachmentFormats, 1)
			.depthAttachmentFormat(formatToVkFormat(gp->depthFormat))
			.stencilAttachmentFormat(formatToVkFormat(gp->stencilFormat))
			.build(vkDevice_, m_DeviceVulkan->pipelineCache_, layout, &pipeline, desc.debugName);


		rps->pipeline_ = pipeline;
		rps->pipelineLayout_ = layout;



		m_DeviceVulkan->addGraphicsPipeline(gp);


		return pipeline;


	}

	void Device::setActualPipeline(GraphicsPipeline* pipeline)
	{

		m_DeviceVulkan->actualPipeline_ = pipeline;
	}

	void Device::destroyShaderModule()
	{

		auto resetShaderModule = [](auto& shaderModule, const char* shaderType)
			{
				if( shaderModule )
				{
					IFNITY_LOG(LogApp, INFO, STRMESSAGE("Destroy ", shaderType, " Shader Module"));
					shaderModule.reset();
				}
			};

		for( auto& vertex : m_shaderVert )
		{
			resetShaderModule(vertex, "Vertex");
		}

		for( auto& fragment : m_shaderFragment )
		{
			resetShaderModule(fragment, "Fragment");
		}

		for( auto& geometry : m_shaderGeometry )
		{
			resetShaderModule(geometry, "Geometry");
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


			if( result != VK_SUCCESS )
			{
				return {};
			}

			size_t numElements = numBytes / sizeof(uint32_t);

			//todo: move to heap compilerRefelction to stack 

			spirv_cross::CompilerReflection* compiler = new  spirv_cross::CompilerReflection((const uint32_t*)spirv, numElements);

			// Refleja los recursos del shader
			spirv_cross::ShaderResources resources = compiler->get_shader_resources();

			uint32_t pushConstantSize = 0;

			// Itera sobre las push constants y calcula el tamaño total
			for( const auto& pushConstant : resources.push_constant_buffers )
			{
				const spirv_cross::SPIRType& type = compiler->get_type(pushConstant.base_type_id);
				pushConstantSize += compiler->get_declared_struct_size(type);
			}


			ShaderModuleState smstate = { .sm = vkShaderModule, .pushConstantsSize = pushConstantSize };

			//Create the handle 
			ShaderModuleHandleSM handle = m_DeviceVulkan->slotMapShaderModules_.create(std::move(smstate));

			//Make holder 
			return  makeHolder(m_DeviceVulkan, handle);

		}
	}

	HolderShaderSM Device::createShaderModule(const char* shaderCode, size_t codeSize, VkShaderStageFlagBits stage, bool isBinary, const char* debugName) const
	{
		std::vector<uint8_t> spirv;
		if( isBinary )
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

	VkFormat Device::getClosestDepthStencilFormat(rhi::Format desiredFormat) const
	{

		// get a list of compatible depth formats for a given desired format
		// The list will contain depth format that are ordered from most to least closest
		const std::vector<VkFormat> compatibleDepthStencilFormatList = getCompatibleDepthStencilFormats(desiredFormat);

		const auto deviceDepthFormats_ = m_DeviceVulkan->depthFormats_;

		// Generate a set of device supported formats
		std::set<VkFormat> availableFormats;
		for( auto format : deviceDepthFormats_ )
		{
			availableFormats.insert(format);
		}

		// check if any of the format in compatible list is supported
		for( auto depthStencilFormat : compatibleDepthStencilFormatList )
		{
			if( availableFormats.count(depthStencilFormat) != 0 )
			{
				return depthStencilFormat;
			}
		}

		// no matching found, choose the first supported format
		return !deviceDepthFormats_.empty() ? deviceDepthFormats_[ 0 ] : VK_FORMAT_D24_UNORM_S8_UINT;
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


		if( ownerHandle_.valid() )
		{
			auto vkpipeline = vkDevice->getVkPipeline(ownerHandle_);
		}



		vkDevice->setActualPipeline(this);


	}

	ShaderModuleState* GraphicsPipeline::getVertexShaderModule()
	{
		if( m_shaderVert.valid() )
		{
			ShaderModuleState* mvert = m_DeviceVulkan->slotMapShaderModules_.get(m_shaderVert);
			return mvert;
		}
		IFNITY_LOG(LogApp, ERROR, "Vertex Shader Module State getHandle   its not valid");
		return nullptr;
	}

	ShaderModuleState* GraphicsPipeline::getFragmentShaderModule()
	{
		if( m_shaderVert.valid() )
		{
			ShaderModuleState* frag = m_DeviceVulkan->slotMapShaderModules_.get(m_shaderFragment);
			return frag;
		}
		IFNITY_LOG(LogApp, ERROR, "Vertex frag Module State getHandle   its not valid");

		return nullptr;
	}

	ShaderModuleState* GraphicsPipeline::getGeometryShaderModule()
	{
		if( m_shaderVert.valid() )
		{
			ShaderModuleState* geom = m_DeviceVulkan->slotMapShaderModules_.get(m_shaderGeometry);
			return geom;
		}
		IFNITY_LOG(LogApp, ERROR, "Geometry  Module State getHandle   its not valid");

		return nullptr;
	}

	void GraphicsPipeline::setSpecializationConstant(const SpecializationConstantDesc& spec)
	{
		if( spec.data && spec.dataSize )
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

		if( !destroy )
		{

			if( m_rVkPipelineState.pipeline_ != VK_NULL_HANDLE )
			{
				vkDestroyPipeline(device, m_rVkPipelineState.pipeline_, nullptr);

			}

			if( m_rVkPipelineState.pipelineLayout_ != VK_NULL_HANDLE )
			{
				vkDestroyPipelineLayout(device, m_rVkPipelineState.pipelineLayout_, nullptr);
			}
		}

		destroy = true;

	}

	void GraphicsPipeline::configureVertexAttributes()
	{


		//Not implemented yet BUT UPDATE THE RENDER PIPELINE STATE and configure inside 

		m_rVkPipelineState.numAttributes_ = m_Description.vertexInput.getNumAttributes();

		const auto& vertexInput = m_Description.vertexInput;

		bool bufferAlreadyBound[ VertexInput::VERTEX_BUFFER_MAX ] = {};

		for( uint32_t i = 0; i != m_rVkPipelineState.numAttributes_; i++ )
		{
			const auto& attr = vertexInput.attributes[ i ];

			m_rVkPipelineState.vkAttributes_[ i ] =
			{
				 .location = attr.location,
				 .binding = attr.binding,
				 .format = formatToVkFormat(attr.format),
				 .offset = (uint32_t)attr.offset
			};

			if( !bufferAlreadyBound[ attr.binding ] )
			{
				bufferAlreadyBound[ attr.binding ] = true;
				m_rVkPipelineState.vkBindings_[ m_rVkPipelineState.numBindings_++ ] =
				{
					 .binding = attr.binding,
					 .stride = vertexInput.inputBindings[ attr.binding ].stride,
					 .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
				};
			}
		}//end for

		//Set the specialization constant
		setSpecializationConstant(specInfo);


	}

	void GraphicsPipeline::passSpecializationConstantToVkFormat()
	{

		for( const auto& spec : m_Description.specInfo )
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


	VkDeviceAddress Buffer::getBufferGpuAddress(const DeviceVulkan& device) const
	{
		const Vulkan::VulkanBuffer* vkBuffer = device.slotMapBuffers_.get(*m_holdBuffer);

		if( vkBuffer->vkDeviceAddress_ )
		{
			IFNITY_LOG(LogCore, TRACE, "Buffer has a valid VkDeviceAddress");
			if( m_BufferGpuAddress == 0 )
			{
				m_BufferGpuAddress = vkBuffer->vkDeviceAddress_;
			}
			return vkBuffer->vkDeviceAddress_;
		}

		return 0;
	}
	//==================================================================================================//
	//  MeshObject Methods			                                                                    //
	//==================================================================================================//

	//Constructor 
	MeshObject::MeshObject(const MeshObjectDescription&& desc, IDevice* device)
		: m_MeshObjectDescription(std::move(desc))
	{

		m_Device = dynamic_cast<Device*>(device);

		//Chec if device its valid 
		if( !m_Device )
		{
			IFNITY_LOG(LogApp, ERROR, "Device is not valid");
			return;
		}
		//Chec if mesh data its valid
		meshStatus_ = MeshStatus::BUFFER_NOT_INITIALIZED;

		//Get vertex data and indices
		const uint32_t* indices = desc.meshData.indexData_.data();
		const float* vertices = desc.meshData.vertexData_.data();

		//Create drawcommands 
		std::vector<uint8_t> drawCommands;
		const uint32_t numCommands = desc.meshFileHeader.meshCount;

		struct DrawIndexedIndirectCommand 
		{
			uint32_t count;
			uint32_t instanceCount;
			uint32_t firstIndex;
			int32_t baseVertex;
			uint32_t baseInstance;
		};

	

		drawCommands.resize(sizeof(DrawIndexedIndirectCommand) * numCommands + sizeof(uint32_t));

		// store the number of draw commands in the very beginning of the buffer
		memcpy(drawCommands.data(), &numCommands, sizeof(numCommands));

		//avoid aliassing issues. 
		DrawIndexedIndirectCommand* cmd = std::launder(reinterpret_cast<DrawIndexedIndirectCommand*>(drawCommands.data() + sizeof(uint32_t)));

		// prepare indirect commands buffer
		for (uint32_t i = 0; i != numCommands; i++) {
			*cmd++ = {
				.count         = desc.meshData.meshes_[i].getLODIndicesCount(0),
				.instanceCount = 1,
				.firstIndex    = desc.meshData.meshes_[i].indexOffset,
				.baseVertex    = (int32_t)desc.meshData.meshes_[i].vertexOffset,
				.baseInstance  = 0,
			};
		}
		//Now drawcommands are ready and filled with the cmd data.
		// 
		BufferDescription bufferDesc = {};
		{
			bufferDesc.SetDebugName("Indices Data Buffer - MeshObject");
			bufferDesc.SetBufferType(BufferType::INDEX_BUFFER);
			bufferDesc.SetStorageType(StorageType::HOST_VISIBLE);
			bufferDesc.SetByteSize(desc.meshFileHeader.indexDataSize);
			bufferDesc.SetData(indices);
		}
		m_BufferIndex = m_Device->CreateBuffer(bufferDesc);
		IFNITY_ASSERT_MSG(m_BufferIndex, "Failed to create index buffer");

		//VertexData
		{
			bufferDesc.SetDebugName("Vertex Data Buffer - MeshObject");
			bufferDesc.SetBufferType(BufferType::VERTEX_BUFFER);
			bufferDesc.SetStorageType(StorageType::HOST_VISIBLE);
			bufferDesc.SetByteSize(desc.meshFileHeader.vertexDataSize);
			bufferDesc.SetData(vertices);
		}

		m_BufferVertex = m_Device->CreateBuffer(bufferDesc);
		IFNITY_ASSERT_MSG(m_BufferVertex, "Failed to create vertex buffer");

		//Indrect buffer
		{
			bufferDesc.SetDebugName("Indirect Data Buffer - MeshObject");
			bufferDesc.SetBufferType(BufferType::INDIRECT_BUFFER);
			bufferDesc.SetStorageType(StorageType::HOST_VISIBLE);
			bufferDesc.SetByteSize(sizeof(DrawIndexedIndirectCommand) * numCommands + sizeof(uint32_t));
			bufferDesc.SetData(drawCommands.data());
		}

		m_BufferIndirect = m_Device->CreateBuffer(bufferDesc);
		IFNITY_ASSERT_MSG(m_BufferIndirect, "Failed to create indirect buffer");

		meshStatus_ = MeshStatus::READY_TO_DRAW;
			
		
	}

	void MeshObject::Draw()
	{
		//Not implemented yet
		throw std::runtime_error("The function or operation is not implemented.");
	}

	void MeshObject::DrawIndexed()
	{
		//Not implemented yet
		throw std::runtime_error("The function or operation is not implemented.");
	}

	void MeshObject::DrawIndirect()
	{
		//Not implemented yet
		throw std::runtime_error("The function or operation is not implemented.");
	}
	void MeshObject:: Draw(const DrawDescription& desc)
	{
		//Not implemented yet
		throw std::runtime_error("The function or operation is not implemented.");
	}

	MeshObjectDescription& MeshObject::GetMeshObjectDescription()
	{
		return m_MeshObjectDescription;
	}

	void MeshObject::DrawInstancedDirect()
	{
		//Not implemented yet
		throw std::runtime_error("The function or operation is not implemented.");
	}


}

IFNITY_END_NAMESPACE