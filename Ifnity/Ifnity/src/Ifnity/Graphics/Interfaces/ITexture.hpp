// Itexture .hpp


#pragma once
#include "pch.h"
#include <filesystem>

IFNITY_NAMESPACE

//Forward declaration 

using TextureHandle = std::shared_ptr<struct ITexture>;


struct TextureDescription;

 const void* LoadTextureFromFile(const std::string& filePath);
 const void* LoadTextureFromFileDescription(TextureDescription& desc);

 void FreeTexture(const void* img);


struct IFNITY_API TextureDescription
{
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t arraySize = 1;
    uint32_t mipLevels = 1;
    uint32_t sampleCount = 1;
    uint32_t sampleQuality = 0;
    uint8_t  comp = 0;
    rhi::Format format = rhi::Format::R8G8B8;
    rhi::TextureDimension dimension = rhi::TextureDimension::TEXTURE2D;
	rhi::TextureWrapping  wrapping = rhi::TextureWrapping::REPEAT;
    std::string debugName;
    std::string filepath;
	

    enum TextureFlags: uint32_t
    {
        IS_SHADER_RESOURCE = 1 << 0,
        IS_RENDER_TARGET = 1 << 1,
        IS_UAV = 1 << 2,
        IS_TYPELESS = 1 << 3,
        IS_SHADING_RATE_SURFACE = 1 << 4,
        IS_VIRTUAL = 1 << 5,
        USE_CLEAR_VALUE = 1 << 6,
        KEEP_INITIAL_STATE = 1 << 7,
		IS_ARB_BINDLESS_TEXTURE = 1 << 8       //ARB_bindless_texture extension used in OpenGL 4.6 see glad.h for more information 
    };

    uint32_t flags = IS_SHADER_RESOURCE; // Inicializado con IS_SHADER_RESOURCE para compatib

    constexpr TextureDescription& setWidth(uint32_t value) { width = value; return *this; }
    constexpr TextureDescription& setHeight(uint32_t value) { height = value; return *this; }
    constexpr TextureDescription& setDepth(uint32_t value) { depth = value; return *this; }
    constexpr TextureDescription& setArraySize(uint32_t value) { arraySize = value; return *this; }
    constexpr TextureDescription& setMipLevels(uint32_t value) { mipLevels = value; return *this; }
    constexpr TextureDescription& setSampleCount(uint32_t value) { sampleCount = value; return *this; }
    constexpr TextureDescription& setSampleQuality(uint32_t value) { sampleQuality = value; return *this; }
    constexpr TextureDescription& setFormat(rhi::Format value) { format = value; return *this; }
    constexpr TextureDescription& setDimension(rhi::TextureDimension value) { dimension = value; return *this; }
    TextureDescription&           setDebugName(const std::string& value) { debugName = value; return *this; }
    TextureDescription& setFilePath(std::string filepath) { this->filepath = filepath;  return *this; }
	constexpr TextureDescription& setWrapping(rhi::TextureWrapping value) { wrapping = value; return *this; }

	//Set the flags
    constexpr TextureDescription& setFlag(TextureFlags flag, bool enabled)
    {
     

		flags = enabled ? flags | flag : flags & ~flag;
        return *this;
    }

    // GetFlag active
    constexpr bool hasFlag(TextureFlags flag) const
    {
        return (flags & flag) != 0;
    }
  
};

class IFNITY_API ITexture
{
public:
    virtual ~ITexture() = default;

    virtual TextureDescription GetTextureDescription() = 0; 
    virtual uint32_t  GetTextureID() = 0;

};


IFNITY_END_NAMESPACE