// File: Utils.hpp
// This header file is built to define utility functions and classes for the Graphics module.



#include "pch.h"
#include  "Interfaces/IBuffer.hpp"
#include "Bitmap.hpp"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../vendor/stb_image/stb_image_write.h"

IFNITY_NAMESPACE

namespace Utils
{

	IFNITY_API BufferDescription CreateConstantBufferDescription(uint64_t byteSize, const std::string& debugName);

	//Util function to save HDR images 
	IFNITY_API void saveHDR(const Bitmap& bitmap, const char* filename);




}

IFNITY_END_NAMESPACE