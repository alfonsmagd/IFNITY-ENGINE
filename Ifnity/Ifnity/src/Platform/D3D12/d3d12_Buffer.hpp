//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-13 by alfonsmagd





#pragma once

#include <pch.h>
#include "d3d12_classes.hpp"
#include "Ifnity/Graphics/Interfaces/IBuffer.hpp"

#include <optional>



namespace D3D12MA { class Allocation; } //avoid namespace collision forward declaration

IFNITY_NAMESPACE

//Forward declaration
class DeviceD3D12;


namespace D3D12
{
	struct D3D12Buffer final : public D3D12Descriptor
	{
		// clang-format off
		[[nodiscard]] inline uint8_t* getMappedPtr() const { return static_cast< uint8_t* >(mappedPtr_); }
		[[nodiscard]] inline bool isMapped() const { return mappedPtr_ != nullptr; }
		// clang-format on

		void bufferSubData( const DeviceD3D12& ctx, size_t offset, size_t size, const void* data );
		void getBufferSubData( const DeviceD3D12& ctx, size_t offset, size_t size, void* data );
		void flushMappedMemory( const DeviceD3D12& ctx, size_t offset, size_t size ) const;
		void invalidateMappedMemory( const DeviceD3D12& ctx, size_t offset, size_t size ) const;
		[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW getVertexBufferView( uint32_t stride_ = 0  ) const;
		
		
	public:
		ComPtr<ID3D12Resource> resource_;
		D3D12MA::Allocation* allocation_ = nullptr;
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress_ = 0;


		size_t bufferSize_ = 0;
		uint32_t bufferStride_ = 0;
		D3D12_RESOURCE_STATES initialState_ = D3D12_RESOURCE_STATE_COMMON;
		D3D12_RESOURCE_FLAGS resourceFlags_ = D3D12_RESOURCE_FLAG_NONE;
		D3D12_RESOURCE_DESC desc_ = {};
		BufferType bufferType_ = BufferType::NO_DEFINE_BUFFER;

		void* mappedPtr_ = nullptr;
		bool isCoherentMemory_ = false;

		// Factory function to get the resource description
		static inline D3D12_RESOURCE_DESC bufferDesc( UINT64 size,
													  D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
													  D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON )
		{
			D3D12_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Alignment = 0;
			desc.Width = size;
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = flags;
			return desc;
		}
	};



}
IFNITY_END_NAMESPACE