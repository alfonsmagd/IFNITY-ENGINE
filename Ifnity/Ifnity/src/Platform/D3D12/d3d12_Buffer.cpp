//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-01 by alfonsmagd


#include "d3d12_Buffer.hpp"
#include <D3D12MemAlloc.h>


IFNITY_NAMESPACE

namespace D3D12
{



	void D3D12Buffer::bufferSubData( const DeviceD3D12& ctx, size_t offset, size_t size, const void* data )
	{}

	void D3D12Buffer::getBufferSubData( const DeviceD3D12 & ctx, size_t offset, size_t size, void* data )
	{}

	void D3D12Buffer::flushMappedMemory( const DeviceD3D12 & ctx, size_t offset, size_t size ) const
	{}

	void D3D12Buffer::invalidateMappedMemory( const DeviceD3D12 & ctx, size_t offset, size_t size ) const
	{}

}

IFNITY_END_NAMESPACE