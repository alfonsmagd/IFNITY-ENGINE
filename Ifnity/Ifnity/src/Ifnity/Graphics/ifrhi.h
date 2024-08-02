#pragma once

#include "pch.h"

//Remember to simplfy the acces namespace RHI = Ifnity::rhi;


IFNITY_NAMESPACE namespace rhi {

    enum class GraphicsAPI : uint8_t
    {
        OPENGL,
        D3D11,
        D3D12,
        VULKAN,

        MAX_GRAPHICS_API
    };






}
IFNITY_END_NAMESPACE