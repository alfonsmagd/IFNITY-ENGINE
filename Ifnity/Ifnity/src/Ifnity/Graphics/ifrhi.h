#pragma once

#include "pch.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
 // TODO: All colors move in rhi namespace
//Remember to simplfy the acces namespace RHI = Ifnity::rhi;
#include <DirectXColors.h>

IFNITY_NAMESPACE 

typedef signed long long int int64;





//BASIC TYPES COLOR// 
struct Color
{
    float r, g, b, a;

    Color() : r(0.f), g(0.f), b(0.f), a(0.f) {}
    Color(float c) : r(c), g(c), b(c), a(c) {}
    Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
    Color(const float* color) : r(color[0]), g(color[1]), b(color[2]), a(color[3]) {}

    bool operator ==(const Color& _b) const { return r == _b.r && g == _b.g && b == _b.b && a == _b.a; }
    bool operator !=(const Color& _b) const { return !(*this == _b); }
   
    Color operator +(const Color& _b) const { return Color(r + _b.r, g + _b.g, b + _b.b, a + _b.a); }

    
    Color operator -(const Color& _b) const { return Color(r - _b.r, g - _b.g, b - _b.b, a - _b.a); }

    
    Color operator *(float scalar) const { return Color(r * scalar, g * scalar, b * scalar, a * scalar); }

    Color operator /(float scalar) const
    {
		return Color(r / scalar, g / scalar, b / scalar, a / scalar);
    }

	// Cast to float* when needed in d3d12
    operator const float* () const
    {
        return &r;
    }
};









namespace rhi {



    struct IFNITY_API VertexScene
    {
        
    };

    struct IFNITY_API VertexBasic
    {
    };

    template <typename T>
    struct IFNITY_API VertexTraits;

    template <>
    struct VertexTraits<VertexScene>
    {
        static constexpr uint16_t numElements = 8;
    };

    template <>
    struct VertexTraits<VertexBasic>
    {
        static constexpr uint16_t numElements = 3;
    };


    //TYPES 

    /**
     * @enum eBitmapType
     * @brief Enum representing the type of bitmap.
     */
    enum eBitmapType : uint8_t
    {
        eBitmapType_2D,   /**< 2D bitmap type */
        eBitmapType_Cube  /**< Cube bitmap type */
    };

    /**
     * @enum eBitmapFormat
     * @brief Enum representing the format of bitmap.
     */
    enum eBitmapFormat : uint8_t
    {
        eBitmapFormat_UnsignedByte, /**< Unsigned byte format */
        eBitmapFormat_Float         /**< Float format */
    };

    /**
     * @enum VerxtexAttributeType
     * @brief Enum representing the type of vertex attribute.
     */
    enum class VerxtexAttributeType : uint8_t
    {
        POSITION, /**< Position attribute */
        COLOR,    /**< Color attribute */
        NORMAL,   /**< Normal attribute */
        TANGENT,  /**< Tangent attribute */
        INDEX,    /**< Index attribute */
        NONE      /**< No attribute */
    };

    /**
     * @enum PrimitiveType
     * @brief Enum representing the type of primitive.
     */
    enum class PrimitiveType : uint8_t
    {
        PointList,                    /**< Point list */
        LineList,                     /**< Line list */
        TriangleList,                 /**< Triangle list */
        TriangleStrip,                /**< Triangle strip */
        TriangleFan,                  /**< Triangle fan */
        TriangleListWithAdjacency,    /**< Triangle list with adjacency */
        TriangleStripWithAdjacency,   /**< Triangle strip with adjacency */
        PatchList                     /**< Patch list */
    };

    /**
     * @enum CullModeType
     * @brief Enum representing the type of cull mode.
     */
    enum class CullModeType : uint8_t
    {
        None,          /**< No culling */
        Front,         /**< Front face culling */
        Back,          /**< Back face culling */
        FrontAndBack   /**< Front and back face culling */
    };

    /**
     * @enum FillModeType
     * @brief Enum representing the type of fill mode.
     */
    enum class FillModeType : uint8_t
    {
        None,       /**< No fill */
        Solid,      /**< Solid fill */
        Point,      /**< Point fill */
        Wireframe   /**< Wireframe fill */
    };

    /**
     * @enum FrontFaceType
     * @brief Enum representing the type of front face.
     */
    enum class FrontFaceType : uint8_t
    {
        Clockwise,         /**< Clockwise front face */
        CounterClockwise   /**< Counter-clockwise front face */
    };

    /**
     * @enum Format
     * @brief Enum representing the format type.
     */
    enum class Format : uint8_t
    {
        R8G8B8_UINT, /**< R8G8B8 unsigned integer format */
        R8G8B8,      /**< R8G8B8 format */
		R8G8B8A8,    /**< R8G8B8A8 format */
		R32G32B32_FLOAT, /**< R32G32B32 float format */
        R8G8B8A8_UNORM, /**< R8G8B8A8 unorm format */
		B8G8R8A8_UNORM, /**< B8G8R8A8 unorm format */
        UNKNOWN,     /**< Unknown format */
        COUNT        /**< Format count */
    };

    /**
     * @enum TextureWrapping
     * @brief Enum representing the type of texture wrapping.
     */
    enum class TextureWrapping : uint8_t
    {
        REPEAT,            /**< Repeat wrapping */
        MIRRORED_REPEAT,   /**< Mirrored repeat wrapping */
        CLAMP_TO_EDGE,     /**< Clamp to edge wrapping */
        CLAMP_TO_BORDER,   /**< Clamp to border wrapping */
        COUNT              /**< Wrapping count */
    };

    /**
     * @enum TextureDimension
     * @brief Enum representing the dimension of texture.
     */
    enum class TextureDimension : uint8_t
    {
        UNKNOWN,            /**< Unknown dimension */
        TEXTURE1D,          /**< 1D texture */
        TEXTURE1DARRAY,     /**< 1D texture array */
        TEXTURE2D,          /**< 2D texture */
        TEXTURE2DARRAY,     /**< 2D texture array */
        TEXTURECUBE,        /**< Cube texture */
        TEXTURECUBEARRAY,   /**< Cube texture array */
        TEXTURE2DMS,        /**< 2D multisample texture */
        TEXTURE2DMSARRAY,   /**< 2D multisample texture array */
        TEXTURE3D,          /**< 3D texture */
        COUNT               /**< Dimension count */
    };

    /**
     * @enum BlendFactor
     * @brief Enum representing the blend factors used in rendering.
     */
    enum class BlendFactor: uint8_t
    {
        ZERO,               /**< Blend factor zero */
        ONE,                /**< Blend factor one */
        SRC_COLOR,          /**< Blend factor source color */
        ONE_MINUS_SRC_ALPHA,/**< Blend factor one minus source alpha */
		ONE_MINUS_SRC_COLOR,/**< Blend factor one minus source color */
        SRC_ALPHA     ,      /**< Blend factor source alpha */
		OPERATION_ADD,     /**< Blend factor operation add */

		COUNT			   /**< Blend factor count */
    };

   
    



       // Definición de los errores de D3D12
   #define D3D12_ERROR_INVALID_DEVICE 0x887A0001
   #define D3D12_ERROR_INVALID_PARAMETER 0x887A0002
   #define D3D12_ERROR_OUT_OF_MEMORY 0x887A0003
   #define D3D12_ERROR_UNSUPPORTED 0x887A0004
   #define D3D12_ERROR_DEVICE_REMOVED 0x887A0005
   #define D3D12_ERROR_DEVICE_HUNG 0x887A0006
   #define D3D12_ERROR_DEVICE_RESET 0x887A0007
   #define D3D12_ERROR_DRIVER_INTERNAL_ERROR 0x887A0008
   #define D3D12_ERROR_INVALID_CALL 0x887A0009
   #define D3D12_ERROR_MORE_DATA 0x887A000A
   #define D3D12_ERROR_NOT_FOUND 0x887A000B
   #define D3D12_ERROR_NOT_CURRENTLY_AVAILABLE 0x887A000C
   #define D3D12_ERROR_ACCESS_DENIED 0x887A000D
   #define D3D12_ERROR_SESSION_DISCONNECTED 0x887A000E
   #define D3D12_ERROR_RESTRICT_TO_OUTPUT_STALE 0x887A000F
   #define D3D12_ERROR_CANNOT_PROTECT_CONTENT 0x887A0010
   #define D3D12_ERROR_ACCESS_LOST 0x887A0011
   #define D3D12_ERROR_WAIT_TIMEOUT 0x887A0012

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


	
    static const std::unordered_map<HRESULT, const char*> hrErrorMap = {
    { S_OK, "S_OK: Operation successful" },
    { E_FAIL, "E_FAIL: Unspecified failure" },
    { E_INVALIDARG, "E_INVALIDARG: One or more arguments are invalid" },
    { E_OUTOFMEMORY, "E_OUTOFMEMORY: Ran out of memory" },
    { E_NOTIMPL, "E_NOTIMPL: Not implemented" },
    { E_NOINTERFACE, "E_NOINTERFACE: No such interface supported" },
    { DXGI_ERROR_DEVICE_HUNG, "DXGI_ERROR_DEVICE_HUNG: The application's device failed due to badly formed commands sent by the application. This is a design-time issue that should be investigated and fixed." },
    { DXGI_ERROR_DEVICE_REMOVED, "DXGI_ERROR_DEVICE_REMOVED: The video card has been physically removed from the system, or a driver upgrade for the video card has occurred." },
    { DXGI_ERROR_DEVICE_RESET, "DXGI_ERROR_DEVICE_RESET: The device failed due to a badly formed command. This is a run-time issue; The application should destroy and recreate the device." },
    { DXGI_ERROR_DRIVER_INTERNAL_ERROR, "DXGI_ERROR_DRIVER_INTERNAL_ERROR: The driver encountered a problem and was put into the device removed state." },
    { DXGI_ERROR_FRAME_STATISTICS_DISJOINT, "DXGI_ERROR_FRAME_STATISTICS_DISJOINT: An event (for example, a power cycle) interrupted the gathering of presentation statistics." },
    { DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE, "DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE: The application attempted to acquire exclusive ownership of an output, but failed because some other application (or device within the application) is already using it." },
    { DXGI_ERROR_INVALID_CALL, "DXGI_ERROR_INVALID_CALL: The application provided invalid parameter data; this must be debugged and fixed before the application is released." },
    { DXGI_ERROR_MORE_DATA, "DXGI_ERROR_MORE_DATA: The buffer supplied by the application is not big enough to hold the requested data." },
    { DXGI_ERROR_NAME_ALREADY_EXISTS, "DXGI_ERROR_NAME_ALREADY_EXISTS: The supplied name of a resource in a call to IDXGIResource1::CreateSharedHandle is already associated with some other resource." },
    { DXGI_ERROR_NONEXCLUSIVE, "DXGI_ERROR_NONEXCLUSIVE: A global counter resource is in use, and the Direct3D device can't currently use the counter resource." },
    { DXGI_ERROR_NOT_CURRENTLY_AVAILABLE, "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: The resource or request is not currently available, but it might become available later." },
    { DXGI_ERROR_NOT_FOUND, "DXGI_ERROR_NOT_FOUND: The specified item was not found." },
    { DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED, "DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED: The remote device has been removed due to disconnection." },
    { DXGI_ERROR_REMOTE_OUTOFMEMORY, "DXGI_ERROR_REMOTE_OUTOFMEMORY: The remote device has run out of memory." },
    { DXGI_ERROR_WAS_STILL_DRAWING, "DXGI_ERROR_WAS_STILL_DRAWING: The GPU was busy at the moment when a call was made to perform an operation, and did not execute or schedule the operation." },
    { DXGI_ERROR_UNSUPPORTED, "DXGI_ERROR_UNSUPPORTED: The requested functionality is not supported by the device or the driver." },
    { DXGI_ERROR_ACCESS_LOST, "DXGI_ERROR_ACCESS_LOST: The desktop duplication interface is invalid. The desktop duplication interface typically becomes invalid when a different type of image is displayed on the desktop." },
    { DXGI_ERROR_WAIT_TIMEOUT, "DXGI_ERROR_WAIT_TIMEOUT: The time-out interval elapsed before the next desktop frame was available." },
    { DXGI_ERROR_SESSION_DISCONNECTED, "DXGI_ERROR_SESSION_DISCONNECTED: The Remote Desktop Services session is currently disconnected." },
    { DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE, "DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE: The DXGI output (monitor) to which the swap chain content was restricted is now disconnected or changed." },
    { DXGI_ERROR_CANNOT_PROTECT_CONTENT, "DXGI_ERROR_CANNOT_PROTECT_CONTENT: DXGI can't provide content protection on the swap chain. This is typically caused by an older driver, or by the application using a swap chain that is incompatible with content protection." },
    { DXGI_ERROR_ACCESS_DENIED, "DXGI_ERROR_ACCESS_DENIED: You tried to use a resource to which you did not have the required access privileges." },
   #ifdef DXGI_ERROR_MPO_UNPINNED
        { DXGI_ERROR_MPO_UNPINNED, "DXGI_ERROR_MPO_UNPINNED: The application is trying to use a resource that is no longer available." },
    #endif
    { DXGI_ERROR_NOT_CURRENT, "DXGI_ERROR_NOT_CURRENT: The application is trying to use a resource that is no longer available." },
    { DXGI_ERROR_HW_PROTECTION_OUTOFMEMORY, "DXGI_ERROR_HW_PROTECTION_OUTOFMEMORY: The application is trying to use a resource that is no longer available." },
    { DXGI_ERROR_DYNAMIC_CODE_POLICY_VIOLATION, "DXGI_ERROR_DYNAMIC_CODE_POLICY_VIOLATION: The application is trying to use a resource that is no longer available." },
    { DXGI_ERROR_NON_COMPOSITED_UI, "DXGI_ERROR_NON_COMPOSITED_UI: The application is trying to use a resource that is no longer available." },
    { D3D12_ERROR_ADAPTER_NOT_FOUND, "D3D12_ERROR_ADAPTER_NOT_FOUND: The specified adapter was not found." },
    { D3D12_ERROR_DRIVER_VERSION_MISMATCH, "D3D12_ERROR_DRIVER_VERSION_MISMATCH: The specified driver version does not  match the required version." },
    { D3D12_ERROR_INVALID_REDIST, "D3D12_ERROR_INVALID_REDIST: The specified redistributable is invalid." },
    { D3D12_ERROR_INVALID_DEVICE, "D3D12_ERROR_INVALID_DEVICE: The specified device is invalid." },
    { D3D12_ERROR_INVALID_PARAMETER, "D3D12_ERROR_INVALID_PARAMETER: The specified parameter is invalid." },
    { D3D12_ERROR_OUT_OF_MEMORY, "D3D12_ERROR_OUT_OF_MEMORY: The operation ran out of memory." },
    { D3D12_ERROR_UNSUPPORTED, "D3D12_ERROR_UNSUPPORTED: The requested functionality is not supported by the device or the  driver." },
    { D3D12_ERROR_DEVICE_REMOVED, "D3D12_ERROR_DEVICE_REMOVED: The device has been removed." },
    { D3D12_ERROR_DEVICE_HUNG, "D3D12_ERROR_DEVICE_HUNG: The device is hung and not responding." },
    { D3D12_ERROR_DEVICE_RESET, "D3D12_ERROR_DEVICE_RESET: The device has been reset." },
    { D3D12_ERROR_DRIVER_INTERNAL_ERROR, "D3D12_ERROR_DRIVER_INTERNAL_ERROR: The driver encountered an internal error." },
    { D3D12_ERROR_INVALID_CALL, "D3D12_ERROR_INVALID_CALL: The method call is invalid." },
    { D3D12_ERROR_MORE_DATA, "D3D12_ERROR_MORE_DATA: More data is available." },
    { D3D12_ERROR_NOT_FOUND, "D3D12_ERROR_NOT_FOUND: The specified item was not found." },
    { D3D12_ERROR_NOT_CURRENTLY_AVAILABLE, "D3D12_ERROR_NOT_CURRENTLY_AVAILABLE: The resource or request is not currently   available." },
    { D3D12_ERROR_ACCESS_DENIED, "D3D12_ERROR_ACCESS_DENIED: Access to the resource is denied." },
    { D3D12_ERROR_SESSION_DISCONNECTED, "D3D12_ERROR_SESSION_DISCONNECTED: The session has been disconnected." },
    { D3D12_ERROR_RESTRICT_TO_OUTPUT_STALE, "D3D12_ERROR_RESTRICT_TO_OUTPUT_STALE: The output is stale." },
    { D3D12_ERROR_CANNOT_PROTECT_CONTENT, "D3D12_ERROR_CANNOT_PROTECT_CONTENT: Cannot protect content." },
    { D3D12_ERROR_ACCESS_LOST, "D3D12_ERROR_ACCESS_LOST: Access to the resource has been lost." },
    { D3D12_ERROR_WAIT_TIMEOUT, "D3D12_ERROR_WAIT_TIMEOUT: The wait operation timed out." },
    { D3D12_ERROR_SESSION_DISCONNECTED, "D3D12_ERROR_SESSION_DISCONNECTED: The session has been disconnected." },
    { D3D12_ERROR_RESTRICT_TO_OUTPUT_STALE, "D3D12_ERROR_RESTRICT_TO_OUTPUT_STALE: The output is stale." },
    { D3D12_ERROR_CANNOT_PROTECT_CONTENT, "D3D12_ERROR_CANNOT_PROTECT_CONTENT: Cannot protect content." },
    { D3D12_ERROR_ACCESS_LOST, "D3D12_ERROR_ACCESS_LOST: Access to the resource has been lost." },
    { D3D12_ERROR_WAIT_TIMEOUT, "D3D12_ERROR_WAIT_TIMEOUT: The wait operation timed out." }

	};



  /*  struct DrawDescription
    {
        uint32_t vertexCount = 0;
        uint32_t instanceCount = 1;
        uint32_t startIndexLocation = 0;
        uint32_t startVertexLocation = 0;
        uint32_t startInstanceLocation = 0;

        constexpr DrawDescription& setVertexCount(uint32_t value) { vertexCount = value; return *this; }
        constexpr DrawDescription& setInstanceCount(uint32_t value) { instanceCount = value; return *this; }
        constexpr DrawDescription& setStartIndexLocation(uint32_t value) { startIndexLocation = value; return *this; }
        constexpr DrawDescription& setStartVertexLocation(uint32_t value) { startVertexLocation = value; return *this; }
        constexpr DrawDescription& setStartInstanceLocation(uint32_t value) { startInstanceLocation = value; return *this; }
    };*/


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
    DECLARE_RENDERER_FUNCTION(void, ImguiRenderDrawDataD3D12, ImDrawData* draw_data, ID3D12GraphicsCommandList* ctx)
	DECLARE_RENDERER_FUNCTION(void, ImGuiOnDetach);





}
IFNITY_END_NAMESPACE