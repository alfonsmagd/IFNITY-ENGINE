#pragma once
//Summary 



#include <pch.h>
#include "Ifnity/Utils/SlotMap.hpp"

IFNITY_NAMESPACE


namespace D3D12
{
	//Using types
	using TextureHandleSM = Handle<struct D3D12Image>;
	





	struct SubmitHandle
	{
		uint32_t bufferIndex_ = 0; // Index buffer
		uint32_t submitId_ = 0;	// Submit ID
		SubmitHandle() = default;
		explicit SubmitHandle(uint64_t handle): bufferIndex_(uint32_t(handle & 0xffffffff)),
			submitId_(uint32_t(handle >> 32))
		{
			assert(submitId_);
		}
		bool empty() const
		{
			return submitId_ == 0;
		}
		//Return handle in 64 bit with information about buffer index and submit ID
		uint64_t handle() const
		{
			return (uint64_t(submitId_) << 32) + bufferIndex_;
		}
	};

}
IFNITY_END_NAMESPACE