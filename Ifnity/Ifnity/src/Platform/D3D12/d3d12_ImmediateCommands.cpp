//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-09 by alfonsmagd





#include "d3d12_ImmediateCommands.hpp"
#include "d3d12_constants.hpp"


IFNITY_NAMESPACE

namespace D3D12
{
	D3D12ImmediateCommands::~D3D12ImmediateCommands()
	{
		// 1. Wait for all command buffers to finish executing
		for( auto& buf : buffers_ )
		{
			if( buf.fence && buf.fence->GetCompletedValue() < buf.fenceValue )
			{
				buf.fence->SetEventOnCompletion(buf.fenceValue, buf.fenceEvent);
				WaitForSingleObject(buf.fenceEvent, INFINITE);
			}
		}

		// 2. Destroy all per-buffer resources
		for( auto& buf : buffers_ )
		{
			if( buf.fenceEvent )
			{
				CloseHandle(buf.fenceEvent);
				buf.fenceEvent = nullptr;
			}

			buf.fence.Reset();
			buf.commandList.Reset();
			buf.allocator.Reset();
		}

		// 3. Optionally destroy the command queue if owned

	}


	D3D12ImmediateCommands::D3D12ImmediateCommands(ID3D12Device* device, ID3D12CommandQueue* queue, uint32_t numContexts): device_(device), queue_(queue), numAvailableCommandBuffers_(numContexts)
	{
		//Check if CommandQueue is valid
		if( queue_ == nullptr )
		{
			IFNITY_LOG(LogCore, ERROR, "CommandQueue is null");
			return;
		}

		for( uint32_t i = 0; i != kMaxCommandBuffers; ++i )
		{
			auto& buf = buffers_[ i ];

			
			// 1. Create a Command Allocator (equivalent to VkCommandPool)
			HRESULT hr = device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, // You can use COMPUTE or COPY if needed
				IID_PPV_ARGS(&buf.allocator)
			);
			assert(SUCCEEDED(hr) && "Failed to create CommandAllocator");

			// 2. Create a Command List (equivalent to VkCommandBuffer)
			hr = device->CreateCommandList(
				0,                               // nodeMask (used for multi-GPU, 0 = default)
				D3D12_COMMAND_LIST_TYPE_DIRECT, // Same type as the allocator
				buf.allocator.Get(),            // Associated allocator
				nullptr,                        // Initial PSO (can be null here)
				IID_PPV_ARGS(&buf.commandList)
			);
			assert(SUCCEEDED(hr) && "Failed to create CommandList");
			// Set debug name for the command list
			std::string name = "CommandList: " + std::to_string( i );
			DEBUG_D3D12_NAME( name, buf.commandList );
			

			// D3D12 command lists are created in the recording state, so we must close them immediately
			buf.commandList->Close();
			// Reset allocator and command list
		
			// 3. Create a Fence (used for GPU synchronization)
			hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&buf.fence));
			assert(SUCCEEDED(hr) && "Failed to create Fence");
			std::string fenceName = "Fence: " + std::to_string( i );
			
			DEBUG_D3D12_NAME( fenceName, buf.fence );

			// 4. Create an Event to wait on the fence from CPU side
			buf.fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			assert(buf.fenceEvent && "Failed to create Event");

			// 5. Setup internal metadata (like  Vulkan handle_.bufferIndex_)
			buf.handle_.bufferIndex_ = i;
			buf.fenceValue = 0;
			buf.isEncoding_ = false;
		}
		



	}





	const D3D12ImmediateCommands::CommandListWrapper& D3D12ImmediateCommands::acquire()
	{
		if( !numAvailableCommandBuffers_ )
		{
			purge();
		}

		while( !numAvailableCommandBuffers_ )
		{
			// Optional logging like IFNITY_LOG
			waitAll();
		}

		CommandListWrapper* current = nullptr;

		// Find an available command list (i.e., not in use)
		for( auto& buf : buffers_ )
		{
			if (!buf.isEncoding_ && buf.fenceValue == 0)
			{
				current = &buf;
				break;
			}
		}

		assert(current);
		assert(numAvailableCommandBuffers_);

		
		current->allocator->Reset();
		current->commandList->Reset(current->allocator.Get(), nullptr); // No PSO bound yet

		current->handle_.submitId_ = fenceCounter_; // track which submission this belongs to
		current->isEncoding_ = true;

		nextSubmitHandle_ = current->handle_;


		numAvailableCommandBuffers_--;

		return *current;
	}

	SubmitHandle D3D12ImmediateCommands::submit(const CommandListWrapper& wrapper)
	{
		assert(wrapper.isEncoding_);
		wrapper.commandList->Close();

		ID3D12CommandList* commandLists[] = { wrapper.commandList.Get() };
		queue_->ExecuteCommandLists(1, commandLists);


		// Signal the fence with current fenceCounter_
		queue_->Signal(wrapper.fence.Get(), fenceCounter_);
		

		// Store current fence value in the wrapper for later checks
		wrapper.fenceValue = fenceCounter_;
		const_cast<CommandListWrapper&>(wrapper).isEncoding_ = false;
		// Update handle and internal state
		lastSubmitHandle_ = wrapper.handle_;
		lastSubmitHandle_.submitId_ = fenceCounter_;

		// Advance fence counter
		fenceCounter_++;
		if( fenceCounter_ == 0 )
		{
			// skip 0  reserved as "empty"
			fenceCounter_++;
		}

		return lastSubmitHandle_;


	}

	SubmitHandle D3D12ImmediateCommands::getLastSubmitHandle() const
	{
		return lastSubmitHandle_;
	}

	SubmitHandle D3D12ImmediateCommands::getNextSubmitHandle() const
	{
		return nextSubmitHandle_;
	}

	bool D3D12ImmediateCommands::isReady(SubmitHandle handle, bool fastCheckNoD3D12) const
	{
		assert(handle.bufferIndex_ < kMaxCommandBuffers);

		if (handle.empty())
		{
			// A null handle
			return true;
		}

		const CommandListWrapper& buf = buffers_[handle.bufferIndex_];

		if (!buf.commandList)
		{
			// Already recycled and not yet reused
			return true;
		}

		if (buf.handle_.submitId_ != handle.submitId_)
		{
			// Already recycled and reused by another command buffer
			return true;
		}

		if (fastCheckNoD3D12)
		{
			// Don't ask the D3D12 fence, just wait until fenceValue changes
			return false;
		}

		// Ask the fence if the GPU has completed that work
		if (buf.fence->GetCompletedValue() >= buf.fenceValue)
		{
			return true;
		}

		return false;
	}

	void D3D12ImmediateCommands::wait(SubmitHandle handle)
	{
		assert(handle.bufferIndex_ < kMaxCommandBuffers);
		if( handle.empty() )
		{
			// A null handle
			return;
		}

		if(isReady(handle))
		{
			return;
		}

		const CommandListWrapper& buf = buffers_[ handle.bufferIndex_ ];
		if(buf.isEncoding_)
		{
			// we are waiting for a buffer which has not been submitted - this is probably a logic error somewhere in the calling code
			IFNITY_LOG(LogCore, ERROR, "Waiting for a buffer which has not been submitted , this is a probably logic error somewhere in the calling code");
			return;
		}
		
		if( buf.fence->GetCompletedValue() < buf.fenceValue )
		{
			buf.fence->SetEventOnCompletion(buf.fenceValue, buf.fenceEvent);
			WaitForSingleObject(buf.fenceEvent, INFINITE);
		}
		//purge();
	
	
	
	}

	void D3D12ImmediateCommands::waitAll()
	{

		for( auto& buf : buffers_ )
		{
			// Only wait on command buffers that were submitted and not currently recording
			if( buf.commandList && buf.isEncoding_ )
			{
				if( buf.fence->GetCompletedValue() < buf.fenceValue )
				{
					// Wait on this fence until it's complete
					buf.fence->SetEventOnCompletion(buf.fenceValue, buf.fenceEvent);
					WaitForSingleObject(buf.fenceEvent, INFINITE);
					++numAvailableCommandBuffers_; // Increment available count since we are done with this buffer
				}
			}
		}

		purge(); // Mark available buffers for reuse

	}

	void D3D12ImmediateCommands::purge()
	{

		const uint32_t numBuffers = static_cast<uint32_t>(ARRAY_NUM_ELEMENTS(buffers_));


		for( uint32_t i = 0; i < numBuffers; ++i )
		{
			// Same wrap-around index logic
			CommandListWrapper& buf = buffers_[ (i + lastSubmitHandle_.bufferIndex_ + 1) % numBuffers ];

			if (buf.fenceValue == 0)
				continue; //not uses yet 


			// Check if GPU has finished with this command list
			if( buf.fence->GetCompletedValue() >= buf.fenceValue )
			{
				buf.fenceValue = 0; // Reset fence value its completted.
				numAvailableCommandBuffers_++;
			}
			else
			{
				
				//Error handling 
				/*IFNITY_LOG(LogCore, INFO, "not purgin buffer position {}",i);
				IFNITY_LOG(LogCore, INFO, "Buffer {} is still in use by the GPU", buf.handle_.bufferIndex_);*/
			    //this->wait(buf.handle_); // Wait for the GPU to finish with this command list
				// This buffer is still in use by the GPU, so we can't recycle it yet
				return;
			}

		}


	}

}
IFNITY_END_NAMESPACE













