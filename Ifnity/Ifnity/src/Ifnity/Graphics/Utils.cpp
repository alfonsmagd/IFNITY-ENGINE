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













}

IFNITY_END_NAMESPACE