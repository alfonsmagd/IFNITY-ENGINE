//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-17 by alfonsmagd



#include "d3d12_StaginDevice.hpp"
#include "Platform\Windows\DeviceD3D12.hpp"
#include "d3d12_Buffer.hpp"

#include "Ifnity/Graphics/Utils.hpp"

IFNITY_NAMESPACE



namespace D3D12
{

	D3D12StagingDevice::D3D12StagingDevice( DeviceD3D12& ctx ): ctx_( ctx )
	{
		// Initialize the staging device
		// Checking if the device is null
		IFNITY_ASSERT_MSG( ctx_.m_Device.Get() != nullptr, "ID3D12Device is null" );
	}

    void D3D12StagingDevice::bufferSubData( D3D12Buffer& buffer, size_t dstOffset, size_t size, const void* data )
    {


        //Upload buffer 
        if( buffer.isMapped() )
        {
            buffer.bufferSubData( ctx_, dstOffset, size, data );
            return;
        }

        //If buffer is not mapped, create to map it and copy the data in staggin buffer 

        ComPtr<ID3D12Resource> stagingBuffer;
        CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_UPLOAD );
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer( size );

        HRESULT hr = ctx_.m_Device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS( &stagingBuffer )
        );
        IFNITY_ASSERT( SUCCEEDED( hr ) );

        void* mapped = nullptr;
        CD3DX12_RANGE readRange( 0, 0 ); //not read by cpu
        hr = stagingBuffer->Map( 0, &readRange, &mapped );
        IFNITY_ASSERT( SUCCEEDED( hr ) );
        memcpy( mapped, data, size );
        stagingBuffer->Unmap( 0, nullptr );

        auto& cmd = ctx_.m_ImmediateCommands->acquire();
        cmd.commandList->CopyBufferRegion( buffer.resource_.Get(), dstOffset, stagingBuffer.Get(), 0, size );

		//Transition the buffer to the appropriate state
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( buffer.resource_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER );
		cmd.commandList->ResourceBarrier( 1, &barrier );

        SubmitHandle handle = ctx_.m_ImmediateCommands->submit( cmd );

		//Defer the destruction of the staging buffer
		ctx_.addDeferredTask( DESTROY_D3D12_BUFFER_DEFERRED( stagingBuffer ), handle );

    }

}// namespace D3D12

IFNITY_END_NAMESPACE