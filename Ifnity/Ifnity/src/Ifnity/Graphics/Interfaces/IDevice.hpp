// IDevice.hpp
// This interface will be used by the DeviceManager to provide rendering functionality to the user.

#pragma once
#include "pch.h"

IFNITY_NAMESPACE


struct IFNITY_API Program
{
    unsigned int id{};
};

struct IFNITY_API GraphicsPipeline
{
    Program program;
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

    virtual GraphicsPipeline CreateGraphicsPipeline() = 0;

    // Virtual destructor to ensure proper destruction of derived objects
    virtual ~IDevice() = default;
};


using DeviceHandle = std::shared_ptr<IDevice>;

IFNITY_END_NAMESPACE