




#pragma once
#include "Ifnity/Graphics/ifrhi.h"
#include "Ifnity/Graphics/Interfaces/IDevice.hpp"
#include "Ifnity/Graphics/Interfaces/IRenderPassVisitor.hpp"    
#include <Platform\Renderers\Simple.hpp>
#include <variant>
#include "Ifnity/Graphics/Features/CubeMapTextures.hpp"
#include "gl_constans.hpp"
#include "../OpenGL/gl_Classes.hpp"
#include <glad\glad.h>
#include <span>
 
IFNITY_NAMESPACE



namespace OpenGL
{
	//Forward declaration
	class SceneObject;
    class Device;

    using RenderPassVariant = std::variant<SimpleRenderer>;

    class OpenGlRenderVisitor final: public IRenderPassVisitor
    {
    public:
        void Visit( SimpleRenderer& pass) override;
		void Visit( DebugRenderer& pass ) override;


		//Delete copy constructor and assignment operator
		OpenGlRenderVisitor( Device* device ): device_( device ) {}
		OpenGlRenderVisitor( const OpenGlRenderVisitor& ) = delete;
		OpenGlRenderVisitor& operator=( const OpenGlRenderVisitor& ) = delete;
        
        void operator() (  SimpleRenderer& pass );

		//No moveVisit



    private:
		Device* device_ = nullptr; ///< Pointer to the device used by the visitor.

    };

    //Other definitions
    //-------------------------------------------------//
    //  DEVICE OPENGL                                  //
    //-------------------------------------------------//

    class Device final: public IDevice
    {
    public:
        /**
        * @brief Constructor for the Device class.
        */
        Device();

        /**
        * @brief Destructor for the Device class.
        */
        virtual ~Device();
         
        /**
        * @brief Draws using the provided description.
        * 
        * @param desc The description of the draw call.
        */
        void Draw(DrawDescription& desc) override;



		void DrawObject(GraphicsPipelineHandle& pipeline, DrawDescription& desc) override;

        /**
        * @brief Creates a graphics pipeline.
        * 
        * @return A GraphicsPipeline object.
        */
        GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDescription& desc) override;


	    BufferHandle CreateBuffer( const BufferDescription& desc) override;

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


        void AddRenderPass( IRendererPass* pass ) override;
       
    private:
		
		    GLuint GetVAO() const { return m_VAO; }
			GLuint CreateVAO();
			void  SetVAO( GLuint vao ) { m_VAO = vao; }
		    TextureHandle CreateTexture2DImpl(TextureDescription& desc);
			TextureHandle CreateTextureCubeMapImpl(TextureDescription& desc);
	    	Program CreateProgram(const char* vertexShader, const char* fragmentShader);
			Program CreateProgram(const char* vertexShader, const char* fragmentShader, const char* geometryShader);
			BufferHandle CreateVertexAttAndIndexBuffer(const BufferDescription& desc);
            BufferHandle CreateDefaultBuffer(int64 size, const void* data, uint32_t flags = 0);
			BufferHandle CreateDefaultBoundBuffer(int64 size, const void* data, uint8_t binding, uint32_t flags = 0);
            void GetMeshVAO(const std::string mesh);
            void SetupVertexAttributes( GLuint vao, GLuint vertexBuffer, GLuint indexBuffer, const std::vector<VertexAttribute>& attributes );
			void DrawInmediate( const DrawDescription& desc );


			std::unique_ptr<GLFrameBuffer> lastFramebuffer_; ///< The last framebuffer used by the device.
			std::unique_ptr<GLFrameBuffer> framebuffer_; ///< The framebuffer used by the device.
			std::vector<IRendererPass*> m_RenderPasses; ///< The render passes used by the device.
            std::vector<RenderPassVariant> m_Rpass;
           
            Program m_Program; ///< The program used by the device.

			GLuint       m_VAO; ///< The vertex array object used by the device DEFAULT VAO. 
			BufferHandle m_VertexBuffer; ///< The vertex buffer used by the device.
			OpenGlRenderVisitor* m_RenderVisitor; ///< The render visitor used by the device.
			
           
			std::unordered_map<std::string_view, GLuint> m_MeshVAOs; ///< The buffers used by save VAO by ID Mesh.
            
            //Friend class 
            friend class MeshObject;
			friend class GraphicsPipeline;
			friend class OpenGlRenderVisitor;
    };


    inline DeviceHandle CreateDevice()
    {
        return std::make_shared<Device>();
    }


    //-------------------------------------------------//
   //  BUFFER OPENGL                                  //
   //-------------------------------------------------//
    class Buffer final: public IBuffer
    {
    public:
        //Constructor 
		Buffer(GLuint bufferID, const BufferDescription& desc): 
            m_BufferID(bufferID),
            m_Description(desc) {}

    
		//Destructor
		virtual ~Buffer() = default;

        
        const void* data;

		BufferDescription& GetBufferDescription()  override { return m_Description; }
		const uint32_t GetBufferID() const override { return m_BufferID; }
		void SetData(const void* data) override { this->data = data; }
		const void* GetData() const override { return data; }
		const uint64_t GetBufferGpuAddress() override { return GetBufferID(); } //make happy IBuffer


    private:
		uint32_t m_BufferID; ///< The buffer ID.
		BufferDescription m_Description; ///< The buffer description.
    };


    

    //-------------------------------------------------//
   //  TEXTURE OPENGL                                  //
   //-------------------------------------------------//

    class IFNITY_API Texture final: public ITexture
    {
    public:
        virtual TextureDescription GetTextureDescription() override { return m_TextureDescription; }
        virtual uint32_t           GetTextureID() override { return m_TextureID; }

        //Constructor 
		Texture() = default;
        Texture(TextureDescription desc, uint32_t uid) : m_TextureDescription(desc), m_TextureID(uid){ }
        Texture(TextureDescription& desc);
        Texture(TextureDescription& desc, bool renderTarget);
        Texture(GLenum type, int width, int height, GLenum internalFormat);
        Texture(int w, int h, const void* img);

        #ifndef BUILD_SHARED_IFNITY
            Texture(Texture&& other) noexcept;
            Texture(const Texture& other) = delete;
        #endif // !BUILD_SHARED_IFNITY

		//Destructor
         ~Texture();
         
        GLuint64 getHandleBindless() const { return m_HandleBindless; }

    private:

        GLuint m_TextureID = 0 ;
        TextureDescription m_TextureDescription;
        GLuint64 m_HandleBindless = 0;
    };

    //-------------------------------------------------  //
    //  GRAPHICS PIPELINE OPENGL                               //
    //-------------------------------------------------//

    class GraphicsPipeline final: public IGraphicsPipeline
    {
		Program m_Program;
		GraphicsPipelineDescription m_Description;
		GLuint m_VAO; ///< The vertex array object used by the device.

    public:
        //Destructor 
        ~GraphicsPipeline();
      
		 const GraphicsPipelineDescription& GetGraphicsPipelineDesc() const override { return m_Description; }
		 void BindPipeline(IDevice* device) override;
		 void  SetProgram(Program program) { m_Program = program; }
		 void SetGraphicsPipelineDesc(GraphicsPipelineDescription desc) { m_Description = desc; }

    private:
        void configureVertexAttributes();

        friend Device;
    };

    //-------------------------------------------------  //
   //  MESHObject OPENGL                              //
   //----------------------------------------------------//

	class MeshObject final: public IMeshObject
	{
        public:
            //Constructor 
            MeshObject(const void* indices, size_t indicesSize, const void* vertexattrib, size_t vertexattribSize, IDevice* device);

            MeshObject(const MeshFileHeader* header, const Mesh* meshes, const void* indices, const void* vertexattrib, IDevice* device);

            // Constructor que toma un MeshObjectDescription
            MeshObject( const MeshObjectDescription&& desc, IDevice* device);
            MeshObject(const SceneObjectHandler& data, IDevice* device);
           

            void Draw() override;
			void Draw(const DrawDescription& desc) override;
			void DrawIndexed() override;
			void DrawIndirect() override;
			void DrawInstancedDirect() override;


			MeshObjectDescription& GetMeshObjectDescription() { return m_MeshObjectDescription; }
            //Destructor 
            ~MeshObject();
            

				
    private:

		GLuint m_VAO; ///< The vertex array object used by the device.
        BufferHandle m_BufferVertex;
		BufferHandle m_BufferIndex;
        BufferHandle m_BufferIndirect;
        BufferHandle m_BufferMaterials;
        BufferHandle m_BufferModelMatrices;

		MeshObjectDescription m_MeshObjectDescription;
        std::vector<GLuint> m_baseMaterialInstances;
		std::vector<GLuint> m_MeshIdx;
		std::vector<GLuint> m_MeshCount;
		std::vector<GLuint> m_MeshBaseVertex;


		IDevice* m_Device; // avoid circular reference
        const MeshFileHeader* m_header;
		const Mesh* m_meshes;
		const SceneObjectHandler m_scene;
	
	};

	//-------------------------------------------------  //

    
     //-------------------------------------------------  //
     // SceneObject OPENGL                                //
	//----------------------------------------------------//

    class IFNITY_API SceneObject final: public ISceneObject
    {
    public:
        SceneObject(
            const char* meshFile,
            const char* sceneFile,
            const char* materialFile);

        //Implement Interface
        GLFrameBuffer* glframebuffer_;
        const MeshFileHeader& getHeader() const override { return header_; }
        const MeshData& getMeshData() const override { return meshData_; }
        const Scene& getScene() const override { return scene_; }
        const std::vector<MaterialDescription>& getMaterials() const override { return materials_; }
        const std::vector<DrawData>& getShapes() const override { return shapes_; }


        //In this case device not create a specificic texture. 
        std::vector<Texture> allMaterialTextures_;

        MeshFileHeader header_;
        MeshData meshData_;

        Scene scene_;
        std::vector<MaterialDescription> materials_;
        std::vector<DrawData> shapes_;

        void loadScene(const char* sceneFile);
        uint64_t getTextureHandleBindless(uint64_t idx, const std::span<Texture>& textures);

    };


   
	




	//-------------------------------------------------  //
	using RenderPass = rhi::RenderPass;
	using Framebuffer = rhi::Framebuffer<Texture>;
	
  
};


IFNITY_END_NAMESPACE
