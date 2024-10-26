





#include "Ifnity/Graphics/Interfaces/IDevice.hpp"


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

    private:

		Program CreateProgram(const char* vertexShader, const char* fragmentShader);


        Program m_Program; ///< The program used by the device.

	    unsigned int  m_VAO; ///< The vertex array object used by the device.
    };


    inline DeviceHandle CreateDevice()
    {
        return std::make_shared<Device>();
    }
};


IFNITY_END_NAMESPACE
