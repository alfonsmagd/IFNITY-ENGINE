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
    rhi::Format format = rhi::Format::R8G8B8;
    rhi::TextureDimension dimension = rhi::TextureDimension::TEXTURE2D;
	rhi::TextureWrapping  wrapping = rhi::TextureWrapping::REPEAT;
    std::string debugName;
    std::string filepath;

    bool isShaderResource = true; // Note: isShaderResource is initialized to 'true' for backward compatibility
    bool isRenderTarget = false;
    bool isUAV = false;
    bool isTypeless = false;
    bool isShadingRateSurface = false;


    // Indicates that the texture is created with no backing memory,
    // and memory is bound to the texture later using bindTextureMemory.
    // On DX12, the texture resource is created at the time of memory binding.
    bool isVirtual = false;

    Color clearValue;
    bool useClearValue = false;

    // If keepInitialState is true, command lists that use the texture will automatically
    // begin tracking the texture from the initial state and transition it to the initial state 
    // on command list close.
    bool keepInitialState = false;

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
    constexpr TextureDescription& setIsRenderTarget(bool value) { isRenderTarget = value; return *this; }
    constexpr TextureDescription& setIsUAV(bool value) { isUAV = value; return *this; }
    constexpr TextureDescription& setIsTypeless(bool value) { isTypeless = value; return *this; }
    constexpr TextureDescription& setIsVirtual(bool value) { isVirtual = value; return *this; }
    constexpr TextureDescription& setClearValue(const Color& value) { clearValue = value; useClearValue = true; return *this; }
    constexpr TextureDescription& setUseClearValue(bool value) { useClearValue = value; return *this; }
    constexpr TextureDescription& setKeepInitialState(bool value) { keepInitialState = value; return *this; }
    TextureDescription& setFilePath(std::string filepath) { this->filepath = filepath;  return *this; }
	constexpr TextureDescription& setWrapping(rhi::TextureWrapping value) { wrapping = value; return *this; }
  
};

class IFNITY_API ITexture
{
public:
    virtual ~ITexture() = default;

    virtual TextureDescription GetTextureDescription() = 0; 
    virtual uint32_t  GetTextureID() = 0;

};


IFNITY_END_NAMESPACE