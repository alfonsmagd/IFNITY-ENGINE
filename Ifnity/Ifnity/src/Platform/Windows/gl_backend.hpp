





#include "Ifnity/Graphics/Interfaces/IDevice.hpp"
#include <glad\glad.h>

IFNITY_NAMESPACE



namespace OpenGL
{

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
        GraphicsPipeline CreateGraphicsPipeline(GraphicsPipelineDescription& desc) override;


	    BufferHandle CreateBuffer( const BufferDescription& desc) override;

	    void WriteBuffer(BufferHandle& buffer, const void* data, size_t size, uint32_t offset = 0) override;

        void BindingVertexAttributes(const VertexAttributeDescription* desc, int sizedesc, const void* data, size_t size)   override;
        void BindingVertexIndexAttributes(const VertexAttributeDescription* desc, int sizedesc, BufferHandle& bf) override;


    private:

	    	Program CreateProgram(const char* vertexShader, const char* fragmentShader);
			BufferHandle CreateVertexAttAndIndexBuffer(const BufferDescription& desc);

            Program m_Program; ///< The program used by the device.

            GLuint   m_VAO; ///< The vertex array object used by the device.
			BufferHandle m_VertexBuffer; ///< The vertex buffer used by the device.
    };






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


    inline DeviceHandle CreateDevice()
    {
        return std::make_shared<Device>();
    }
};


IFNITY_END_NAMESPACE
