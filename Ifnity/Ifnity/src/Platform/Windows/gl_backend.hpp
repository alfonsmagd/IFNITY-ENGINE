





#include "Ifnity/Graphics/Interfaces/IDevice.hpp"
#include "Ifnity/Graphics/Features/CubeMapTextures.hpp"
#include "gl_constans.hpp"
#include <glad\glad.h>

IFNITY_NAMESPACE



namespace OpenGL
{
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

        virtual TextureHandle CreateTexture(TextureDescription& desc) override;
		MeshObjectHandle CreateMeshObject(const MeshObjectDescription& desc) override;

    private:
		
		    GLuint GetVAO() const { return m_VAO; }
			GLuint CreateVAO();
		    TextureHandle CreateTexture2DImpl(TextureDescription& desc);
			TextureHandle CreateTextureCubeMapImpl(TextureDescription& desc);
	    	Program CreateProgram(const char* vertexShader, const char* fragmentShader);
			Program CreateProgram(const char* vertexShader, const char* fragmentShader, const char* geometryShader);
			BufferHandle CreateVertexAttAndIndexBuffer(const BufferDescription& desc);
            BufferHandle CreateDefaultBuffer(int64 size, const void* data, uint32_t flags);
            void GetMeshVAO(const std::string mesh);
            void SetupVertexAttributes(GLuint vao, GLuint vertexBuffer, GLuint indexBuffer, const std::vector<VertexAttribute>& attributes);
            


            Program m_Program; ///< The program used by the device.

			GLuint       m_VAO; ///< The vertex array object used by the device DEFAULT VAO. 
			BufferHandle m_VertexBuffer; ///< The vertex buffer used by the device.

			std::unordered_map<std::string_view, GLuint> m_MeshVAOs; ///< The buffers used by save VAO by ID Mesh.
            
            //Friend class 
            friend class MeshObject;
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


    private:
		uint32_t m_BufferID; ///< The buffer ID.
		BufferDescription m_Description; ///< The buffer description.
    };


    

    //-------------------------------------------------//
   //  TEXTURE OPENGL                                  //
   //-------------------------------------------------//

    class Texture final: public ITexture
    {
    public:
        virtual TextureDescription GetTextureDescription() override { return m_TextureDescription; }
        virtual uint32_t           GetTextureID() override { return m_TextureID; }

        //Constructor 
        Texture(TextureDescription desc, uint32_t uid) : m_TextureDescription(desc), m_TextureID(uid){ }

    private:
        uint32_t m_TextureID;
        TextureDescription m_TextureDescription;
    };

    //-------------------------------------------------  //
    //  GRAPHICS PIPELINE OPENGL                               //
    //-------------------------------------------------//

    class GraphicsPipeline final: public IGraphicsPipeline
    {
		Program m_Program;
		GraphicsPipelineDescription m_Description;

    public:
        //Destructor 
        ~GraphicsPipeline();
      
		 const GraphicsPipelineDescription& GetGraphicsPipelineDesc() const override { return m_Description; }
		 void  SetProgram(Program program) { m_Program = program; }
		

    };

    //-------------------------------------------------  //
   //  MESHObject OPENGL                              //
   //----------------------------------------------------//

	class MeshObject final: public IMeshObject
	{
        public:
            //Constructor 
			MeshObject() = default;
            MeshObject(const void* indices, size_t indicesSize, const void* vertexattrib, size_t vertexattribSize, IDevice* device);
            void Draw() override;

            //Destructor 
            ~MeshObject();

				
    private:

		GLuint m_VAO; ///< The vertex array object used by the device.
        BufferHandle m_BufferVertex;
		BufferHandle m_BufferIndex;
        BufferHandle m_BufferIndirect;

		IDevice* m_Device; // avoid circular reference
	
	};

	//-------------------------------------------------  //

};


IFNITY_END_NAMESPACE
