//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-01 by alfonsmagd

#pragma once
//Summary 



#include <pch.h>
#include "Ifnity/Utils/SlotMap.hpp"

#define USE_PIX //THIS FLAG WILL BE ADDED IN CMAKE  CHECKING IF PIX HAS BEEN INSTALLED IN THE SYSTEM.
#ifdef USE_PIX
	#include <pix.h>
	#define BEGIN_PIX_EVENT(cmdList, name) PIXBeginEvent(cmdList, name)
	#define END_PIX_EVENT(cmdList)         PIXEndEvent(cmdList)
#else
	#define BEGIN_PIX_EVENT(cmdList, name) ((void)0)
	#define END_PIX_EVENT(cmdList)         ((void)0)
#endif

IFNITY_NAMESPACE







namespace D3D12
{
	//Variables constexpre 
	constexpr D3D12_CPU_DESCRIPTOR_HANDLE D3D12InvalidHandle = { ~0ull };


	static void DumpDebugMessages(bool enable = true)
	{
		using namespace Microsoft::WRL;

		if( !enable )
			return;

		//ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		//if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
		//{
		//	UINT64 messageCount = dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

		//	for (UINT64 i = 0; i < messageCount; ++i)
		//	{
		//		SIZE_T messageLength = 0;
		//		dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);

		//		std::vector<char> messageData(messageLength);
		//		DXGI_INFO_QUEUE_MESSAGE* message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(messageData.data());

		//		dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength);

		//		IFNITY_LOG(LogCore, TRACE, "DXGI Message: " + std::string(message->pDescription));
		//		//std::cout << "DXGI DEBUG: " << message->pDescription << std::endl;
		//	}

		//	dxgiInfoQueue->ClearStoredMessages(DXGI_DEBUG_ALL); // clean 
		//}
	}





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