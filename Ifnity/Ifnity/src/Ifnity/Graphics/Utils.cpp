// File: Utils.cpp
// This header file is built to define utility functions and classes for the Graphics module.


#include "Utils.hpp"


IFNITY_NAMESPACE

namespace Utils
{

	BufferDescription CreateConstantBufferDescription(uint64_t byteSize, const std::string& debugName)
	{
	
		return BufferDescription{}.SetByteSize(byteSize)
								  .SetDebugName(debugName)
			                      .SetBufferType(BufferType::CONSTANT_BUFFER);
	}

	IFNITY_API void saveHDR(const Bitmap& bitmap, const char* filename)
	{
		stbi_write_hdr(filename, bitmap.w_, bitmap.h_, bitmap.comp_, reinterpret_cast<const float*>(bitmap.data_.data()));

	}


}

IFNITY_END_NAMESPACE