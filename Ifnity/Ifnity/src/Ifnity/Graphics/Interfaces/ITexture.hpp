// Itexture .hpp


#pragma once
#include "pch.h"
#include <filesystem>
#include "IBuffer.hpp"

IFNITY_NAMESPACE

//Forward declaration 

using TextureHandle = std::shared_ptr<struct ITexture>;
struct TextureDescription;

//------------------------------------------------------------------------------------//
//  TEXTURE INTERFACE  AUXILIAR FUNCTIONS                                             //
//-------------------------------------------------------------------------------------//
const void* LoadTextureFromFile(const std::string& filePath);
const void* LoadTextureFromFileDescription(TextureDescription& desc);
bool isDepthFormat(rhi::Format format);
bool getNumImagePlanes(rhi::Format format);
void FreeTexture(const void* img);


//------------------------------------------------------------------------------------//
//  TEXTURE ISTRUCTS AND PROPERTIES                                             //
//-------------------------------------------------------------------------------------//
struct TextureFormatProperties
{
	const rhi::Format format = rhi::Format::UNKNOWN;
	const uint8_t bytesPerBlock : 5 = 1;
	const uint8_t blockWidth : 3 = 1;
	const uint8_t blockHeight : 3 = 1;
	const uint8_t minBlocksX : 2 = 1;
	const uint8_t minBlocksY : 2 = 1;
	const bool depth : 1 = false;
	const bool stencil : 1 = false;
	const bool compressed : 1 = false;
	const uint8_t numPlanes : 2 = 1;
};

#define PROPS(fmt, bpb, ...) \
  TextureFormatProperties { .format = rhi::Format::fmt, .bytesPerBlock = bpb, ##__VA_ARGS__ }

static constexpr TextureFormatProperties properties[] = {
	PROPS(R8G8B8_UINT, 3),
	PROPS(R8G8B8, 3),
	PROPS(R8G8B8A8, 4),
	PROPS(R32G32B32_FLOAT, 12),
	PROPS(R8G8B8A8_UNORM, 4),
	PROPS(B8G8R8A8_UNORM, 4),
	PROPS(R_UNORM8, 1),
	PROPS(R_UINT16, 2),
	PROPS(R_UINT32, 4),
	PROPS(R_UNORM16, 2),
	PROPS(R_FLOAT16, 2),
	PROPS(R_FLOAT32, 4),
	PROPS(RG_UNORM8, 2),
	PROPS(RG_UINT16, 4),
	PROPS(RG_UINT32, 8),
	PROPS(RG_UNORM16, 4),
	PROPS(RG_FLOAT16, 4),
	PROPS(RG_FLOAT32, 8),
	PROPS(RGBA_UNORM8, 4),
	PROPS(RGBA_UINT32, 16),
	PROPS(RGBA_FLOAT16, 8),
	PROPS(RGBA_FLOAT32, 16),
	PROPS(RGBA_SRGB8, 4),
	PROPS(BGRA_UNORM8, 4),
	PROPS(BGRA_SRGB8, 4),
	PROPS(ETC2_RGB8, 8, .blockWidth = 4, .blockHeight = 4, .compressed = true),
	PROPS(ETC2_SRGB8, 8, .blockWidth = 4, .blockHeight = 4, .compressed = true),
	PROPS(BC7_RGBA, 16, .blockWidth = 4, .blockHeight = 4, .compressed = true),
	PROPS(Z_UNORM16, 2, .depth = true),
	PROPS(Z_UNORM24, 3, .depth = true),
	PROPS(Z_FLOAT32, 4, .depth = true),
	PROPS(Z_UNORM24_S_UINT8, 4, .depth = true, .stencil = true),
	PROPS(Z_FLOAT32_S_UINT8, 5, .depth = true, .stencil = true)
};




struct IFNITY_API Dimensions
{
	uint32_t width = 1;
	uint32_t height = 1;
	uint32_t depth = 1;
	inline Dimensions divide1D(uint32_t v) const
	{
		return { .width = width / v, .height = height, .depth = depth };
	}
	inline Dimensions divide2D(uint32_t v) const
	{
		return { .width = width / v, .height = height / v, .depth = depth };
	}
	inline Dimensions divide3D(uint32_t v) const
	{
		return { .width = width / v, .height = height / v, .depth = depth / v };
	}
	inline bool operator==(const Dimensions& other) const
	{
		return width == other.width && height == other.height && depth == other.depth;
	}
};


struct IFNITY_API Offset3D
{
	int32_t x = 0;
	int32_t y = 0;
	int32_t z = 0;
};

struct IFNITY_API TextureLayers
{
	uint32_t mipLevel = 0;
	uint32_t layer = 0;
	uint32_t numLayers = 1;
};

struct IFNITY_API TextureRangeDesc
{
	Offset3D offset = {};
	Dimensions dimensions = { 1, 1, 1 };
	uint32_t layer = 0;
	uint32_t numLayers = 1;
	uint32_t mipLevel = 0;
	uint32_t numMipLevels = 1;
};


struct IFNITY_API TextureDescription
{
	union
	{
		struct
		{
			uint32_t width;
			uint32_t height;
			uint32_t depth;
		};
		Dimensions dimensions{ 1,1,1 };
	};
	uint32_t arraySize = 1;
	uint32_t mipLevels = 1;
	uint32_t sampleCount = 1;
	uint32_t sampleQuality = 0;
	uint8_t  comp = 0;
	rhi::Format format = rhi::Format::R8G8B8;
	rhi::TextureType dimension = rhi::TextureType::TEXTURE2D;
	rhi::TextureWrapping  wrapping = rhi::TextureWrapping::REPEAT;
	rhi::TextureUsageBits usage = rhi::TextureUsageBits::UNKNOW;
	rhi::DepthStencilTextureFlags depthStencil = rhi::DepthStencilTextureFlags::DEPTH;
	StorageType storage = StorageType::DEVICE; //for now , not implementint stagin buffer;
	std::string debugName;
	std::string filepath;

	bool generateMipMaps = false;
	bool isDepth = false;
	bool isStencil = false;
	const void* data = nullptr;


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
	constexpr TextureDescription& setDimension(rhi::TextureType value) { dimension = value; return *this; }
	TextureDescription& setDebugName(const std::string& value) { debugName = value; return *this; }
	TextureDescription& setFilePath(std::string filepath) { this->filepath = filepath;  return *this; }
	constexpr TextureDescription& setWrapping(rhi::TextureWrapping value) { wrapping = value; return *this; }
	constexpr TextureDescription& setData(const void* data) { this->data = data; return *this; }
	constexpr TextureDescription& setStorageType(StorageType value) { storage = value; return *this; }
	constexpr TextureDescription& setGenerateMipMaps(bool value) { generateMipMaps = value; return *this; }
	constexpr TextureDescription& setUsage(rhi::TextureUsageBits value) { usage = value; return *this; }

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