// IDevice.hpp
// This interface will be used by the DeviceManager to provide rendering functionality to the user.

#pragma once
#include "pch.h"
#include "../IShader.hpp"

IFNITY_NAMESPACE


struct IFNITY_API Program
{
     unsigned int id;
};

struct IFNITY_API GraphicsPipeline
{
    Program program;
    // Constructor por defecto
	 GraphicsPipeline() = default;
};


struct IFNITY_API GraphicsPipelineDescription
{
 
	IShader* vs  = nullptr;
	IShader* ps = nullptr;

	 GraphicsPipelineDescription& SetVertexShader(IShader* shader)
	{
		vs = shader;
		return *this;
	}
     GraphicsPipelineDescription& SetPixelShader(IShader* shader)
    {
		ps = shader;
		return *this;
    }

};


struct IFNITY_API DrawDescription
{
    unsigned int size;
};

// Definition of the IDevice interface
class IFNITY_API IDevice {
public:

    /**
     * @brief Se va usar para dibujar.
     * 
     * @param desc Descripción del dibujo.
     */
    virtual void Draw(DrawDescription& desc) = 0;

    virtual GraphicsPipeline CreateGraphicsPipeline(GraphicsPipelineDescription& desc) = 0;

    // Virtual destructor to ensure proper destruction of derived objects
    virtual ~IDevice() = default;
};


using DeviceHandle = std::shared_ptr<IDevice>;

IFNITY_END_NAMESPACE