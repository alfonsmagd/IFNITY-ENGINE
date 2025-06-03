//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-13 by alfonsmagd




#pragma once

#include <pch.h>
#include "d3d12_classes.hpp"
#include <d3d12.h>
#include <wrl.h>

IFNITY_NAMESPACE

namespace D3D12
{
	using namespace Microsoft::WRL;


	class D3D12ImmediateCommands final
	{
	public:
		// the maximum number of command buffers which can similtaneously exist in the system; when we run out of buffers, we stall and wait until
		// an existing buffer becomes available
		static constexpr uint32_t kMaxCommandBuffers = 3;


		D3D12ImmediateCommands(ID3D12Device* device, ID3D12CommandQueue* queue, uint32_t numContexts = kMaxCommandBuffers);
		~D3D12ImmediateCommands();
		D3D12ImmediateCommands(const D3D12ImmediateCommands&) = delete;
		D3D12ImmediateCommands& operator=(const D3D12ImmediateCommands&) = delete;

		struct CommandListWrapper
		{
			ComPtr<ID3D12CommandAllocator> allocator;
			ComPtr<ID3D12GraphicsCommandList4> commandList;
			ComPtr<ID3D12Fence> fence;
			SubmitHandle handle_{};
			HANDLE fenceEvent = nullptr;
			mutable uint64_t fenceValue = 0;
			bool isEncoding_ = false;
		};

		// returns the current command buffer (creates one if it does not exist)
		const CommandListWrapper& acquire();
		SubmitHandle submit(const CommandListWrapper& wrapper);
		
		//VkFence getVkFence(SubmitHandle handle) const;
		SubmitHandle getLastSubmitHandle() const;
		SubmitHandle getNextSubmitHandle() const;
		bool isReady(SubmitHandle handle, bool fastCheckNoD3D12 = false) const;
		void wait(SubmitHandle handle);
		void waitAll();

	private:
		void purge();

	private:
		ID3D12Device* device_;
		ID3D12CommandQueue* queue_;
		const char* debugName_ = "";
		CommandListWrapper buffers_[ kMaxCommandBuffers ];
		SubmitHandle lastSubmitHandle_ = SubmitHandle();
		SubmitHandle nextSubmitHandle_ = SubmitHandle();
		uint32_t numAvailableCommandBuffers_ = kMaxCommandBuffers;
		uint32_t fenceCounter_ = 1;          // Start from 1 to avoid confusion with the initial value of 0
	};






}

IFNITY_END_NAMESPACE