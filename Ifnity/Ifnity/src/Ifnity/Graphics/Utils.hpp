// File: Utils.hpp
// This header file is built to define utility functions and classes for the Graphics module.


#pragma once
#include "pch.h"
#include  "Interfaces/IBuffer.hpp"
#include "Bitmap.hpp"
#include "../vendor/stb_image/stb_image_write.h"
#include "Ifnity\Graphics\Interfaces\IShader.hpp"

IFNITY_NAMESPACE

namespace Utils
{
	using BinaryShaderTuple = std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, std::vector<uint8_t>>;

	IFNITY_API BufferDescription CreateConstantBufferDescription(uint64_t byteSize, const std::string& debugName);

	//Util function to save HDR images 
	IFNITY_API void saveHDR(const Bitmap& bitmap, const char* filename);

	IFNITY_API void printShaderSource(const char* text);
	IFNITY_API std::string readShaderFile(const char* fileName);

	std::tuple<std::string, std::string, std::string> readShaderFilesByAPI(const rhi::GraphicsAPI& api, const  IShader* vs, const IShader* fs, const IShader* gs);

	// Read a shader file as binary data
	BinaryShaderTuple readShaderBinarysByAPI(const rhi::GraphicsAPI& api, const  IShader* vs, const IShader* fs, const IShader* gs);

	IFNITY_API std::vector<uint8_t> readShaderFileBinary( const char* fileName );


	template<typename ReturnType>
	std::tuple<ReturnType, ReturnType, ReturnType>
		readShadersMode( const rhi::GraphicsAPI& api,
							  const IShader* vs,
							  const IShader* fs,
							  const IShader* gs,
							  ShaderFileMode mode );

	const char* processEntryPoint( const std::wstring& wname, std::string& outString );
	
	
   
	template<typename T>
	IFNITY_API T getNumMipMapLevels2D(T w, T h) noexcept
	{
		T levels = 1;
		while((w | h) >> levels)
			levels += 1;
		return levels;
	}

	IFNITY_API uint32_t getTextureBytesPerLayer(uint32_t width, uint32_t height, rhi::Format format, uint32_t level) noexcept;

}

IFNITY_END_NAMESPACE