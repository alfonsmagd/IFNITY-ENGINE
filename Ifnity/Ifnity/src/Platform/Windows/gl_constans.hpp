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
        GLuint index;          /**< Index of the vertex attribute. */
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
        GLuint count_;          /**< Number of elements to be rendered. */
        GLuint instanceCount_;  /**< Number of instances to be rendered. */
        GLuint firstIndex_;     /**< Index of the first element. */
        GLuint baseVertex_;     /**< Base vertex index. */
        GLuint baseInstance_;   /**< Base instance index. */
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
        { rhi::Format::R32G32B32_FLOAT, GL_RGB32F }
    } };

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
    inline GLenum ConvertToOpenGLTextureTarget(rhi::TextureDimension dimension)
    {
        switch(dimension)
        {
        case rhi::TextureDimension::TEXTURE1D:
            return GL_TEXTURE_1D;
        case rhi::TextureDimension::TEXTURE1DARRAY:
            return GL_TEXTURE_1D_ARRAY;
        case rhi::TextureDimension::TEXTURE2D:
            return GL_TEXTURE_2D;
        case rhi::TextureDimension::TEXTURE2DARRAY:
            return GL_TEXTURE_2D_ARRAY;
        case rhi::TextureDimension::TEXTURECUBE:
            return GL_TEXTURE_CUBE_MAP;
        case rhi::TextureDimension::TEXTURECUBEARRAY:
            return GL_TEXTURE_CUBE_MAP_ARRAY;
        case rhi::TextureDimension::TEXTURE2DMS:
            return GL_TEXTURE_2D_MULTISAMPLE;
        case rhi::TextureDimension::TEXTURE2DMSARRAY:
            return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        case rhi::TextureDimension::TEXTURE3D:
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