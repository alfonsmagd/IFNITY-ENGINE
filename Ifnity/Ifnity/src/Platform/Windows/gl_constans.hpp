
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
        { rhi::Format::R32G32B32A32_TYPELESS,       GL_RGBA32UI },
        { rhi::Format::R32G32B32A32_FLOAT,          GL_RGBA32F  },
        { rhi::Format::R32G32B32A32_UINT,           GL_RGBA32UI },
        { rhi::Format::R32G32B32A32_SINT,           GL_RGBA32I  },
        { rhi::Format::R32G32B32_TYPELESS,          GL_RGB32UI  },
        { rhi::Format::R32G32B32_FLOAT,             GL_RGB32F   },
        { rhi::Format::R32G32B32_UINT,              GL_RGB32UI  },
        { rhi::Format::R32G32B32_SINT,              GL_RGB32I   },
        { rhi::Format::R16G16B16A16_TYPELESS,       GL_RGBA16UI },
        { rhi::Format::R16G16B16A16_FLOAT,          GL_RGBA16F  },
        { rhi::Format::R16G16B16A16_UNORM,          GL_RGBA16   },
        { rhi::Format::R16G16B16A16_UINT,           GL_RGBA16UI },
        { rhi::Format::R16G16B16A16_SNORM,          GL_RGBA16_SNORM },
        { rhi::Format::R16G16B16A16_SINT,           GL_RGBA16I  },
        { rhi::Format::R32G32_TYPELESS,             GL_RG32UI   },
        { rhi::Format::R32G32_FLOAT,                GL_RG32F    },
        { rhi::Format::R32G32_UINT,                 GL_RG32UI   },
        { rhi::Format::R32G32_SINT,                 GL_RG32I    },
        { rhi::Format::R32G8X24_TYPELESS,           GL_DEPTH32F_STENCIL8 },
        { rhi::Format::D32_FLOAT_S8X24_UINT,        GL_DEPTH32F_STENCIL8 },
        { rhi::Format::R32_FLOAT_X8X24_TYPELESS  ,  GL_DEPTH32F_STENCIL8 },
        { rhi::Format::X32_TYPELESS_G8X24_UINT,     GL_DEPTH32F_STENCIL8 },
        { rhi::Format::R10G10B10A2_TYPELESS,        GL_RGB10_A2UI },
        { rhi::Format::R10G10B10A2_UNORM,           GL_RGB10_A2  },
        { rhi::Format::R10G10B10A2_UINT,             GL_RGB10_A2UI },
        { rhi::Format::R11G11B10_FLOAT,              GL_R11F_G11F_B10F },
        { rhi::Format::R8G8B8A8_TYPELESS,            GL_RGBA8UI   },
        { rhi::Format::R8G8B8A8_UNORM,               GL_RGBA8     },
        { rhi::Format::R8G8B8A8_UNORM_SRGB,          GL_SRGB8_ALPHA8 },
        { rhi::Format::R8G8B8A8_UINT,                GL_RGBA8UI   },
        { rhi::Format::R8G8B8A8_SNORM,               GL_RGBA8_SNORM },
        { rhi::Format::R8G8B8A8_SINT,                GL_RGBA8I    },
        { rhi::Format::UNKNOWN,                      GL_NONE                      },
        { rhi::Format::R16G16_TYPELESS,              GL_RG16UI                    },
        { rhi::Format::R16G16_FLOAT,                 GL_RG16F                     },
        { rhi::Format::R16G16_UNORM,                 GL_RG16                      },
        { rhi::Format::R16G16_UINT,                  GL_RG16UI                    },
        { rhi::Format::R16G16_SNORM,                 GL_RG16_SNORM                },
        { rhi::Format::R16G16_SINT,                  GL_RG16I                     },
        { rhi::Format::R32_TYPELESS,                 GL_R32UI                     },
        { rhi::Format::D32_FLOAT,                    GL_DEPTH_COMPONENT32         },
        { rhi::Format::R32_FLOAT,                    GL_R32F                      },
        { rhi::Format::R32_UINT,                     GL_R32UI                     },
        { rhi::Format::R32_SINT,                     GL_R32I                      },
        { rhi::Format::R24G8_TYPELESS,               GL_DEPTH24_STENCIL8          },
        { rhi::Format::D24_UNORM_S8_UINT,            GL_DEPTH24_STENCIL8          },
        { rhi::Format::R24_UNORM_X8_TYPELESS,        GL_DEPTH24_STENCIL8          },
        { rhi::Format::X24_TYPELESS_G8_UINT,         GL_DEPTH24_STENCIL8          },
        { rhi::Format::R8G8_TYPELESS,                GL_RG8UI                     },
        { rhi::Format::R8G8_UNORM,                   GL_RG8                       },
        { rhi::Format::R8G8_UINT,                    GL_RG8UI                     },
        { rhi::Format::R8G8_SNORM,                   GL_RG8_SNORM                 },
        { rhi::Format::R8G8_SINT,                    GL_RG8I                      },
        { rhi::Format::R16_TYPELESS,                 GL_R16UI                     },
        { rhi::Format::R16_FLOAT,                    GL_R16F                      },
        { rhi::Format::D16_UNORM,                    GL_DEPTH_COMPONENT16         },
        { rhi::Format::R16_UNORM,                    GL_R16                       },
        { rhi::Format::R16_UINT,                     GL_R                          },
        { rhi::Format::R8_UNORM,                     GL_R8                        },
        { rhi::Format::R8_SNORM,                     GL_R8_SNORM                  },
        { rhi::Format::R16_UINT,                     GL_R16UI                     },
        { rhi::Format::R16_SINT,                     GL_R16I                      },
        { rhi::Format::R16_UNORM,                    GL_R16                       },
        { rhi::Format::R16_SNORM,                    GL_R16_SNORM                 },
        { rhi::Format::R16_FLOAT,                    GL_R16F                      },
        { rhi::Format::B5G6R5_UNORM,                 GL_RGB565                    },
        { rhi::Format::B5G5R5A1_UNORM,               GL_RGB5_A1                   },
        { rhi::Format::R10G10B10A2_UNORM,            GL_RGB10_A2                  },
        { rhi::Format::R11G11B10_FLOAT,              GL_R11F_G11F_B10F            },
        { rhi::Format::R32_UINT,                     GL_R32UI                     },
        { rhi::Format::R32_SINT,                     GL_R32I                      },
        { rhi::Format::R32_FLOAT,                    GL_R32F                      },
        { rhi::Format::BC4_UNORM,                    GL_COMPRESSED_RED_RGTC1      },
        { rhi::Format::BC4_SNORM,                    GL_COMPRESSED_SIGNED_RED_RGTC1 },
        { rhi::Format::BC5_UNORM,                    GL_COMPRESSED_RG_RGTC2       },
        { rhi::Format::BC5_SNORM,                    GL_COMPRESSED_SIGNED_RG_RGTC2 },
        { rhi::Format::BC7_UNORM,                    GL_COMPRESSED_RGBA_BPTC_UNORM },
        { rhi::Format::BC7_UNORM_SRGB,               GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM },
    } };
    


IFNITY_END_NAMESPACE