// File: Utils.hpp
// This header file is built to define utility functions and classes for the Graphics module.



#include "pch.h"
#include  "Interfaces/IBuffer.hpp"

IFNITY_NAMESPACE

namespace Utils
{

	IFNITY_API BufferDescription CreateConstantBufferDescription(uint64_t byteSize, const std::string& debugName);













}

IFNITY_END_NAMESPACE