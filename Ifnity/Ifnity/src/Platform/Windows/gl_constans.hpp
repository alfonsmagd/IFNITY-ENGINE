
#include "Ifnity/Graphics/ifrhi.h"
#include <glad/glad.h>


IFNITY_NAMESPACE


namespace OpenGL
{
    struct FormatMapping
    {
        rhi::Format rhiFormat;
        GLenum glFormat;
    };

    static const std::array<FormatMapping, size_t(rhi::Format::COUNT)> c_FormatMap = { {
        { rhi::Format::R8G8B8_UINT,                                         GL_RGB8UI },
        { rhi::Format::R8G8B8,                                              GL_RGB8   }
    } };


   inline void SetOpenGLRasterizationState(const RasterizationState& state)
    {
        static rhi::CullModeType currentCullMode = rhi::CullModeType::FrontAndBack;
        static rhi::FrontFaceType currentFrontFace = rhi::FrontFaceType::CounterClockwise;
        static rhi::FillModeType currentFillMode = rhi::FillModeType::Solid;

        // Configurar el modo de culling solo si es diferente al actual
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

        // Configurar la orientación de la cara frontal solo si es diferente a la actual
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

        // Configurar el modo de polígono solo si es diferente al actual
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
            return GL_TEXTURE_2D; // Valor por defecto
        }
    }

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
		   return GL_REPEAT; // Valor por defecto
	   }
   }
}
    


IFNITY_END_NAMESPACE