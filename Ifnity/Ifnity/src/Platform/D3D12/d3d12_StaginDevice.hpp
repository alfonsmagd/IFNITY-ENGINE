#pragma once 

#include <pch.h>
#include <d3d12.h>

IFNITY_NAMESPACE

class DeviceD3D12;



namespace D3D12
{

	class Device;
	class D3D12Buffer;

	class D3D12StagingDevice final
	{
	public:
		explicit D3D12StagingDevice( DeviceD3D12& ctx );
		~D3D12StagingDevice() = default;

		D3D12StagingDevice( const D3D12StagingDevice& ) = delete;
		D3D12StagingDevice& operator=( const D3D12StagingDevice& ) = delete;
		void bufferSubData( D3D12Buffer& buffer, size_t dstOffset, size_t size, const void* data );


	private:
		DeviceD3D12& ctx_;

	};


}

IFNITY_END_NAMESPACE













