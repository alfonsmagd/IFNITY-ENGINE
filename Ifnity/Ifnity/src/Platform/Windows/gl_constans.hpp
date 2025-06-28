#include "Ifnity/Graphics/ifrhi.h"
#include <glad/glad.h>

IFNITY_NAMESPACE

namespace OpenGL
{
    static constexpr GLuint kBufferIndex_PerFrameUniforms = 0;
    static constexpr GLuint kBufferIndex_ModelMatrices = 1;
    static constexpr GLuint kBufferIndex_Materials = 2;
    /**
     * @brief Structure representing a vertex attribute in OpenGL.
     */
    struct VertexAttribute
    {
        GLuint location;          /**< Index of the vertex attribute. */
        GLint size;            /**< Size of the vertex attribute. */
        GLenum type;           /**< Data type of the vertex attribute. */
        GLboolean normalized;  /**< Whether the attribute is normalized. */
        GLsizei stride;        /**< Stride between consecutive attributes. */
        size_t offset;         /**< Offset of the attribute in the buffer. */
        GLuint bindingindex;   /**< Binding index of the attribute. */
    };

    /**
     * @brief Structure representing a draw elements indirect command in OpenGL.
     */
    struct DrawElementsIndirectCommand
    {
        GLuint count_;                  /**< Number of elements to be rendered. */
        GLuint instanceCount_;          /**< Number of instances to be rendered. */
        GLuint firstIndex_;             /**< Index of the first element. */
        GLuint baseVertex_;             /**< Base vertex index. */
        GLuint baseInstance_;           /**< Base instance index. */
    };

    /**
     * @brief Structure mapping RHI format to OpenGL format.
     */
    struct FormatMapping
    {
        rhi::Format rhiFormat;  /**< RHI format. */
        GLenum glFormat;        /**< Corresponding OpenGL format. */
    };

    static const std::array<FormatMapping, size_t(rhi::Format::COUNT)> c_FormatMap = { {
    { rhi::Format::R8G8B8_UINT, GL_RGB8UI },
    { rhi::Format::R8G8B8, GL_RGB8 },
    { rhi::Format::R8G8B8A8, GL_RGBA8 },
    { rhi::Format::R32G32B32_FLOAT, GL_RGB32F },
    { rhi::Format::R8G8B8A8_UNORM, GL_RGBA8 },
    { rhi::Format::B8G8R8A8_UNORM, GL_BGRA },
    { rhi::Format::R_UNORM8, GL_R8 },
    { rhi::Format::R_UINT16, GL_R16UI },
    { rhi::Format::R_UINT32, GL_R32UI },
    { rhi::Format::R_UNORM16, GL_R16 },
    { rhi::Format::R_FLOAT16, GL_R16F },
    { rhi::Format::R_FLOAT32, GL_R32F },
    { rhi::Format::RG_UNORM8, GL_RG8 },
    { rhi::Format::RG_UINT16, GL_RG16UI },
    { rhi::Format::RG_UINT32, GL_RG32UI },
    { rhi::Format::RG_UNORM16, GL_RG16 },
    { rhi::Format::RG_FLOAT16, GL_RG16F },
    { rhi::Format::R32G32_FLOAT, GL_RG32F },
    { rhi::Format::RGBA_UNORM8, GL_RGBA8 },
    { rhi::Format::RGBA_UINT32, GL_RGBA32UI },
    { rhi::Format::RGBA_FLOAT16, GL_RGBA16F },
    { rhi::Format::RGBA_FLOAT32, GL_RGBA32F },
    { rhi::Format::RGBA_SRGB8, GL_SRGB8_ALPHA8 },
    { rhi::Format::BGRA_UNORM8, GL_BGRA },
    { rhi::Format::BGRA_SRGB8, GL_SRGB8_ALPHA8 },
    { rhi::Format::ETC2_RGB8, GL_COMPRESSED_RGB8_ETC2 },
    { rhi::Format::ETC2_SRGB8, GL_COMPRESSED_SRGB8_ETC2 },
    { rhi::Format::BC7_RGBA, GL_COMPRESSED_RGBA_BPTC_UNORM },
    { rhi::Format::Z_UNORM16, GL_DEPTH_COMPONENT16 },
    { rhi::Format::Z_UNORM24, GL_DEPTH_COMPONENT24 },
    { rhi::Format::Z_FLOAT32, GL_DEPTH_COMPONENT32F },
    { rhi::Format::Z_UNORM24_S_UINT8, GL_DEPTH24_STENCIL8 },
    { rhi::Format::Z_FLOAT32_S_UINT8, GL_DEPTH32F_STENCIL8 }




} };

    inline uint32_t getNumComponents(rhi::Format format)
    {
        using enum rhi::Format;

        switch (format)
        {
            case R_UNORM8:
            case R_UINT16:
            case R_UINT32:
            case R_UNORM16:
            case R_FLOAT16:
            case R_FLOAT32:
            case Z_UNORM16:
            case Z_UNORM24:
            case Z_FLOAT32:
                return 1;

            case RG_UNORM8:
            case RG_UINT16:
            case RG_UINT32:
            case RG_UNORM16:
            case RG_FLOAT16:
            case R32G32_FLOAT:
                return 2;

            case R8G8B8:
            case R8G8B8_UINT:
            case R32G32B32_FLOAT:
            case ETC2_RGB8:
            case ETC2_SRGB8:
                return 3;

            case R8G8B8A8:
            case R8G8B8A8_UNORM:
            case B8G8R8A8_UNORM:
            case RGBA_UNORM8:
            case RGBA_UINT32:
            case RGBA_FLOAT16:
            case RGBA_FLOAT32:
            case RGBA_SRGB8:
            case BGRA_UNORM8:
            case BGRA_SRGB8:
            case BC7_RGBA:
            case Z_UNORM24_S_UINT8:
            case Z_FLOAT32_S_UINT8:
                return 4;

            default:
                return 1; // fallback seguro
        }
    }
  



	/**
	 * @brief Get the RHI format corresponding to the given OpenGL format.
	 * @param glFormat The OpenGL format.
	 * @return The corresponding RHI format, or rhi::Format::UNKNOWN if not found.
	 */
    /*std::vector<VkFormat> getCompatibleDepthStencilFormats(lvk::Format format)
    {
        switch(format)
        {
        case lvk::Format_Z_UN16:
            return { VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT };
        case lvk::Format_Z_UN24:
            return { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM_S8_UINT };
        case lvk::Format_Z_F32:
            return { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
        case lvk::Format_Z_UN24_S_UI8:
            return { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT };
        case lvk::Format_Z_F32_S_UI8:
            return { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT };
        default:
            return { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT };
        }
        return { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT };
    }*/
    /**
     * @brief Sets the OpenGL rasterization state.
     * 
     * @param state The rasterization state to be set.
     */
    inline void SetOpenGLRasterizationState(const RasterizationState& state)
    {
        static rhi::CullModeType currentCullMode = rhi::CullModeType::FrontAndBack;
        static rhi::FrontFaceType currentFrontFace = rhi::FrontFaceType::CounterClockwise;
        static rhi::FillModeType currentFillMode = rhi::FillModeType::Solid;

        // Configure culling mode if different from current
        if(state.cullMode != currentCullMode)
        {
            switch(state.cullMode)
            {
            case rhi::CullModeType::None:
                glDisable(GL_CULL_FACE);
                break;
            case rhi::CullModeType::Front:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                break;
            case rhi::CullModeType::Back:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                break;
            case rhi::CullModeType::FrontAndBack:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT_AND_BACK);
                break;
            }
            currentCullMode = state.cullMode;
        }

        // Configure front face orientation if different from current
        if(state.frontFace != currentFrontFace)
        {
            switch(state.frontFace)
            {
            case rhi::FrontFaceType::Clockwise:
                glFrontFace(GL_CW);
                break;
            case rhi::FrontFaceType::CounterClockwise:
                glFrontFace(GL_CCW);
                break;
            }
            currentFrontFace = state.frontFace;
        }

        // Configure polygon mode if different from current
        if(state.fillMode != currentFillMode)
        {
            switch(state.fillMode)
            {
            case rhi::FillModeType::Point:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
            case rhi::FillModeType::Wireframe:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case rhi::FillModeType::Solid:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            }
            currentFillMode = state.fillMode;
        }
    }

    /**
     * @brief Converts RHI texture dimension to OpenGL texture target.
     * 
     * @param dimension The RHI texture dimension.
     * @return The corresponding OpenGL texture target.
     */
    inline GLenum ConvertToOpenGLTextureTarget(rhi::TextureType dimension)
    {
        switch(dimension)
        {
        case rhi::TextureType::TEXTURE1D:
            return GL_TEXTURE_1D;
        case rhi::TextureType::TEXTURE1DARRAY:
            return GL_TEXTURE_1D_ARRAY;
        case rhi::TextureType::TEXTURE2D:
            return GL_TEXTURE_2D;
        case rhi::TextureType::TEXTURE2DARRAY:
            return GL_TEXTURE_2D_ARRAY;
        case rhi::TextureType::TEXTURECUBE:
            return GL_TEXTURE_CUBE_MAP;
        case rhi::TextureType::TEXTURECUBEARRAY:
            return GL_TEXTURE_CUBE_MAP_ARRAY;
        case rhi::TextureType::TEXTURE2DMS:
            return GL_TEXTURE_2D_MULTISAMPLE;
        case rhi::TextureType::TEXTURE2DMSARRAY:
            return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        case rhi::TextureType::TEXTURE3D:
            return GL_TEXTURE_3D;
        default:
            return GL_TEXTURE_2D; // Default value
        }
    }

    /**
     * @brief Converts RHI texture wrapping to OpenGL texture wrapping.
     * 
     * @param wrapping The RHI texture wrapping.
     * @return The corresponding OpenGL texture wrapping.
     */
    inline GLenum ConvertToOpenGLTextureWrapping(rhi::TextureWrapping wrapping)
    {
        switch(wrapping)
        {
        case rhi::TextureWrapping::REPEAT:
            return GL_REPEAT;
        case rhi::TextureWrapping::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case rhi::TextureWrapping::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case rhi::TextureWrapping::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        default:
            return GL_REPEAT; // Default value
        }
    }

    /**
     * @brief Converts blend factor to OpenGL blend factor.
     * 
     * @param factor The blend factor.
     * @return The corresponding OpenGL blend factor.
     */
    inline GLenum ConvertToOpenGLBlendFactor(BlendFactor factor)
    {
        switch(factor)
        {
        case BlendFactor::ZERO:
            return GL_ZERO;
        case BlendFactor::ONE:
            return GL_ONE;
        case BlendFactor::SRC_COLOR:
            return GL_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SRC_ALPHA:
            return GL_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        default:
            return GL_ONE; // Default value
        }
    }
}

IFNITY_END_NAMESPACE