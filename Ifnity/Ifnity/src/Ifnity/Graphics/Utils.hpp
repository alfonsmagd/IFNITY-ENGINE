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


	IFNITY_API BufferDescription CreateConstantBufferDescription(uint64_t size, const std::string& debugName);

	//Util function to save HDR images 
	IFNITY_API void saveHDR(const Bitmap& bitmap, const char* filename);

	IFNITY_API void printShaderSource(const char* text);
	IFNITY_API std::string readShaderFile(const char* fileName);

	std::tuple<std::string, std::string, std::string> readShaderFilesByAPI(const rhi::GraphicsAPI& api, const  IShader* vs, const IShader* fs, const IShader* gs);
   

}

IFNITY_END_NAMESPACE