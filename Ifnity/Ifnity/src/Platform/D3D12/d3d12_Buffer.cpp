//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-13 by alfonsmagd



#include "d3d12_Buffer.hpp"
#include <D3D12MemAlloc.h>
#include "d3d12_constants.hpp"

IFNITY_NAMESPACE

namespace D3D12
{



	void D3D12Buffer::bufferSubData( const DeviceD3D12& ctx, size_t offset, size_t size, const void* data )
	{
		// only host-visible buffers can be uploaded this way
		IFNITY_ASSERT( mappedPtr_ && "Buffer must be mapped" );

		if( !mappedPtr_ )
		{
			IFNITY_LOG( LogCore, ERROR, "Buffer is not mapped" );
			return;
		}

		IFNITY_ASSERT( offset + size <= bufferSize_ );

		if( data )
		{
			memcpy( static_cast< uint8_t* >(mappedPtr_) + offset, data, size );
		}
		else
		{
			memset( static_cast< uint8_t* >(mappedPtr_) + offset, 0, size );
		}

		// No need to flush memory on upload heap (it's coherent)

	}

	void D3D12Buffer::getBufferSubData( const DeviceD3D12& ctx, size_t offset, size_t size, void* data )
	{}

	void D3D12Buffer::flushMappedMemory( const DeviceD3D12& ctx, size_t offset, size_t size ) const
	{}

	void D3D12Buffer::invalidateMappedMemory( const DeviceD3D12& ctx, size_t offset, size_t size ) const
	{}

	D3D12_VERTEX_BUFFER_VIEW D3D12Buffer::getVertexBufferView(uint32_t stride_ ) const
	{


		IFNITY_ASSERT_MSG(bufferType_ == BufferType::VERTEX_BUFFER,"this buffer its not a vertexbuffer view");
		return D3D12_VERTEX_BUFFER_VIEW{
			.BufferLocation = gpuAddress_,
			.SizeInBytes = static_cast<UINT>(bufferSize_),
			.StrideInBytes = stride_ ? stride_ : static_cast< UINT >(bufferStride_),
		};

	}


	

}

IFNITY_END_NAMESPACE