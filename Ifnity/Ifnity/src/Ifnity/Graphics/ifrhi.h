#pragma once

#include "pch.h"

//Remember to simplfy the acces namespace RHI = Ifnity::rhi;


IFNITY_NAMESPACE namespace rhi {


    static bool IsNvDeviceID(UINT id)
    {
        return id == 0x10DE;
    }


    enum class GraphicsAPI : uint8_t
    {
        OPENGL,
        D3D11,
        D3D12,
        VULKAN,

        MAX_GRAPHICS_API
    };

    
    // MacroDefinition to  IntelliSense Style based in the Forge Engine;
#ifdef __INTELLISENSE__
 // IntelliSense is the code completion engine in Visual Studio. When it parses the source files, __INTELLISENSE__ macro is defined.
// Here we trick IntelliSense into thinking that the renderer functions are not function pointers, but just regular functions.
// What this achieves is filtering out duplicated function names from code completion results and improving the code completion for function
// parameters. This dramatically improves the quality of life for Visual Studio users. 
    //@see https://github.com/ConfettiFX/The-Forge IGraphics.h
#define DECLARE_RENDERER_FUNCTION(ret, name, ...) ret name(__VA_ARGS__);
#else
#define DECLARE_RENDERER_FUNCTION(ret, name, ...)       \
    typedef ret(__cdecl* name##Fn)(__VA_ARGS__); \
    extern name##Fn name;
#endif

   
    DECLARE_RENDERER_FUNCTION(void, ImGuiRenderDrawData, ImDrawData* draw_data);
    DECLARE_RENDERER_FUNCTION(void, ImGuiOnDetach);



}
IFNITY_END_NAMESPACE