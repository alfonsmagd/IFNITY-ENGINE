//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-24 by alfonsmagd


//


//
//




#pragma once

#include "Ifnity\Graphics\ifrhi.h"
#include "Ifnity\Graphics\Interfaces\IDevice.hpp"
#include "Ifnity\Graphics\Interfaces\IBuffer.hpp"
#include "Ifnity\Graphics\Interfaces\ITexture.hpp"
#include "Ifnity\Graphics\Interfaces\IGraphicsPipeline.hpp"
#include "Ifnity\Graphics\Interfaces\IMeshObject.hpp"

#include "Platform\D3D12\d3d12_CommandBuffer.hpp"
#include "Platform\D3D12\d3d12_classes.hpp"

IFNITY_NAMESPACE


class DeviceD3D12;

namespace D3D12
{


	class IFNITY_API Texture final: public ITexture
	{
	public:
		virtual ~Texture() = default;

		Texture() = default;
		Texture( const TextureDescription& desc, HolderTextureSM&& texture ): m_TextureDescription( desc ), m_holdTexture( std::move( texture ) )
		{
			m_TextureID = *m_holdTexture + offset;
		}
		TextureDescription GetTextureDescription() override { return m_TextureDescription; }
		//Todo: GetTextureFormatProperties
		uint32_t GetTextureID() override { return m_TextureID; }
		void   setOffset( uint32_t off = 10 ) { offset = off; }


		TextureHandleSM getTextureHandleSM() const { return *m_holdTexture; }

	private:
		uint32_t m_TextureID = 0; ///< The texture ID.
		uint32_t offset = 10; ///< The offset for the SRV descriptor. start_srv_binddless TODO
		TextureDescription m_TextureDescription; ///< The texture description.
		HolderTextureSM m_holdTexture; ///< The texture handle.
	};

	//------------------------------------------------------------------------------------//
	//  DEVICE D3D12                                                                     //
	//-------------------------------------------------------------------------------------//
	class IFNITY_API Device final: public IDevice
	{
	public:
		Device( DeviceD3D12* ptr );
		virtual ~Device() = default;


		void Draw( DrawDescription& desc )override;
		GraphicsPipelineHandle CreateGraphicsPipeline( GraphicsPipelineDescription& desc ) override;
		void WriteBuffer( BufferHandle& buffer, const void* data, size_t size, uint32_t offset = 0 ) override;
		void BindingVertexAttributes( const VertexAttributeDescription* desc, int sizedesc, const void* data, size_t size ) override {};
		void BindingVertexIndexAttributes( const VertexAttributeDescription* desc, int sizedesc, BufferHandle& bf )override {}; //todo abstract
		void BindingVertexAttributesBuffer( BufferHandle& bf )override;
		void BindingIndexBuffer( BufferHandle& bf )override;
		BufferHandle CreateBuffer( const BufferDescription& desc ) override;
		TextureHandle CreateTexture( TextureDescription& desc ) override; //TODO add TextureDescripton const
		MeshObjectHandle CreateMeshObject( const MeshObjectDescription& desc ) override ;
		MeshObjectHandle CreateMeshObject( const MeshObjectDescription& desc, IMeshDataBuilder* meshbuilder ) override;
		SceneObjectHandler CreateSceneObject( const char* meshes, const char* scene, const char* materials ) override ;
		MeshObjectHandle  CreateMeshObjectFromScene( const SceneObjectHandler& scene ) override;
		void DrawObject( GraphicsPipelineHandle& pipeline, DrawDescription& desc )override; //todo abstract 
		void StartRecording() override;
		void StopRecording() override;
		void SetDepthTexture( TextureHandle texture )override {};
		// Virtual destructor to ensure proper destruction of derived objects
		void setActualPipeline( GraphicsPipeline* pipeline );
		D3D12::GraphicsPipeline* getActualPipeline() const ;
		void upload( BufferHandleSM& buffer, const void* data, size_t size, uint32_t offset = 0 );
		void upload( D3D12::D3D12Buffer* buffer, const void* data, size_t size, uint32_t offset = 0 );
		void upload( TextureHandleSM handle, uint32_t miplevel, const void* data );

		void DrawObjectIndirect( GraphicsPipelineHandle& pipeline, DrawDescription& desc, BufferHandle& bf ) override;

		CommandBuffer& getCommandBuffer() { return cmdBuffer; }


	private:
		bool validateTextureDescription( TextureDescription& texdesc );
		D3D12_RESOURCE_FLAGS getImageUsageFlags( const TextureDescription& texdesc );

		HolderBufferSM CreateInternalD3D12Buffer( const BufferDescription& desc,
												  D3D12_RESOURCE_FLAGS resourceFlags,
												  D3D12_RESOURCE_STATES initialState,
												  D3D12_HEAP_TYPE heapType,
												  const char* debugName );

		struct
		{
			const void* data = nullptr;
			size_t size = 0;
			size_t offset = 0;
		}pushConstants;

		DeviceD3D12* m_DeviceD3D12 = nullptr;       ///< Pointer to the DeviceD3D12 instance.
		GraphicsPipelineHandleSM m_PipelineHandle;	///< Handle to the graphics pipeline.
		CommandBuffer cmdBuffer;					///< Command buffer for recording commands.

		//handles
		TextureHandleSM currentTexture_;		  ///< Handle to the current texture.
		BufferHandleSM currentVertexBuffer_;	  ///< Handle to the current vertex buffer.
		BufferHandleSM currentIndexBuffer_;	      ///< Handle to the current index buffer.


		friend class MeshObject; ///< Allow MeshObject to access private members.
	};


	class IFNITY_API Buffer final: public IBuffer
	{
	public:
		Buffer( const BufferDescription& desc ): m_Description( desc ) {}
		Buffer( const BufferDescription& desc, HolderBufferSM&& handle ): m_Description( desc ),
			m_holdBuffer( std::move( handle ) )
		{
			m_BufferID = m_holdBuffer.get()->index();
		}
		BufferDescription& GetBufferDescription() override { return m_Description; }
		const uint32_t GetBufferID() const override { return m_BufferID; }
		const BufferHandleSM& getBufferHandleSM() const { return *m_holdBuffer; }
		BufferHandleSM& getBufferHandleSM() { return *m_holdBuffer; }
		const uint64_t GetBufferGpuAddress() override { return 0; IFNITY_LOG( LogCore, WARNING, "GETBUFFERADDRES D3D12 NOT IMPLEMENTED" ); };
		void SetData( const void* data ) override {};
		const void* GetData() const { return nullptr; IFNITY_LOG( LogCore, WARNING, "GET DATA BUFFER D3D12 NOT IMPLEMENTED" ); };

	private:
		uint32_t m_BufferID = 0; ///< The buffer ID.
		mutable uint64_t m_BufferGpuAddress = 0; ///< The buffer GPU address.
		BufferDescription m_Description; ///< The buffer description  m_holdBuffer;
		HolderBufferSM m_holdBuffer; ///< The buffer handle.
	};


	class IFNITY_API GraphicsPipeline final: public IGraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline();

		GraphicsPipeline( GraphicsPipelineDescription&& desc, DeviceD3D12* dev );

		const GraphicsPipelineDescription& GetGraphicsPipelineDesc() const override { return m_Description; }


		ID3D12PipelineState* getPipelineState() const { return m_PipelineState.Get(); }
		void  BindPipeline( struct IDevice* device ) override;

		void setColorFormat( rhi::Format format ) { colorFormat = format; }
		void setDepthFormat( rhi::Format format ) { depthFormat = format; }

		void SetGraphicsPipelineDesc( GraphicsPipelineDescription desc ) { m_Description = std::move( desc ); }

	private:
		void configureVertexAttributes();


	private:
		GraphicsPipelineDescription m_Description;
		RenderPipelineState m_rD3D12PipelineState;

		//Pipeline State Description.
		ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;


		// Estado de rasterizacin, profundidad, etc.
		rhi::Format colorFormat = rhi::Format::UNKNOWN;
		rhi::Format depthFormat = rhi::Format::UNKNOWN;
		rhi::Format stencilFormat = rhi::Format::UNKNOWN;

		StencilState backFaceStencil = {};
		StencilState frontFaceStencil = {};

		uint32_t samplesCount = 1u;

		ShaderModuleHandleSM m_shaderVert;
		ShaderModuleHandleSM m_shaderPixel;
		ShaderModuleHandleSM m_shaderGeometry;

		bool destroy = false;

		// Handle de slotmap (para manager de pipelines)

		DeviceD3D12* m_DeviceD3D12 = nullptr;


		friend class Device;
		friend class DeviceD3D12;
		friend class CommandBuffer;
	};

	//Please constructor are not safe if you dont create the construct function 
	template<typename... Args>
	inline DeviceHandle CreateDevice( Args&&... args )
	{
		return std::make_shared<Device>( std::forward<Args>( args )... );
	}


	//------------------------------------------------------------------------------------//
	//  MESH OBJECT D3D12                                                                 //
	//-------------------------------------------------------------------------------------//
		class IFNITY_API MeshObject final: public IMeshObject
	{
	public:

		enum class MeshStatus
		{
			READY_TO_DRAW,
			IDEVICE_NOT_VALID,
			BUFFER_NOT_INITIALIZED,
			// Agrega más estados según sea necesario
		}meshStatus_ = MeshStatus::IDEVICE_NOT_VALID;

		struct DrawIndexedIndirectCommand
		{
			uint32_t bInstanceroot;
			uint32_t count;
			uint32_t instanceCount;
			uint32_t firstIndex;
			int32_t baseVertex;
			uint32_t baseInstance;
		};

		MeshObject( const MeshObjectDescription&& desc, IDevice* device ) {};
		MeshObject( const SceneObjectHandler& data, IDevice* device);


		void Draw() override {};
		void Draw( const DrawDescription& desc ) override {};
		void DrawIndexed() override {};

		MeshObjectDescription& GetMeshObjectDescription() override { return  m_MeshObjectDescription; };
		void DrawIndirect() override;
		void DrawInstancedDirect() override {};



	private :
		Device* m_Device = nullptr; // avoid circular reference 
		MeshObjectDescription m_MeshObjectDescription;
		uint32_t numIndices_ = 0;

		BufferHandle m_BufferVertex;
		BufferHandle m_BufferIndex;
		BufferHandle m_BufferIndirect;
		BufferHandle m_BufferMaterials;
		BufferHandle m_BufferDrawID;
		BufferHandle m_BufferModelMatrices;

		struct BufferGpuIndex
		{
			uint32_t drawId;
			uint32_t materials;
			uint32_t modelMatrices;
		};
		struct SMBuffers
		{
			BufferHandleSM vertexBuffer;
			BufferHandleSM indexBuffer;
			BufferHandleSM indirectBuffer;		
			BufferHandleSM drawIDBuffer;
			BufferHandleSM materialBuffer;
			BufferHandleSM modelMatricesBuffer;
			BufferGpuIndex srvIndex;
		}m_SM;

		std::vector<TextureHandle> allMaterialsTextures_; //In this case textures will be building because we use device to create textures, Opengl can create textures itself.

	private: 
		void convertToD3D12Material( std::vector<MaterialDescription>& mt,
									 Device& device,
									 const std::vector<std::string>& files,
									 std::vector<TextureHandle>& mtlTextures );
	};

	//------------------------------------------------------------------------------------//
	//  SCENE OBJECT VULKAN                                                               //
	//-------------------------------------------------------------------------------------//

	class IFNITY_API SceneObject final: public ISceneObject
	{
	public:
		SceneObject(
			const char* meshFile,
			const char* sceneFile,
			const char* materialFile);

		//Implement Interface

		const MeshFileHeader& getHeader() const override { return header_; }
		const MeshData& getMeshData() const override { return meshData_; }
		const Scene& getScene() const override { return scene_; }
		const std::vector<MaterialDescription>& getMaterials() const override { return materials_; }

		//Shapes its not overload here 
		const std::vector<DrawData>& getShapes() const override { return shapes_; }
		const std::vector<std::string>& getTexturesFiles() override { return textureFiles_; }
		void loadSceneShapes(const char* sceneFile);

		MeshFileHeader header_;
		MeshData meshData_;
	private:
		Scene scene_;
		std::vector<std::string> textureFiles_;
		mutable std::vector<MaterialDescription> materials_;
		std::vector<DrawData> shapes_;



	};

}

IFNITY_END_NAMESPACE
