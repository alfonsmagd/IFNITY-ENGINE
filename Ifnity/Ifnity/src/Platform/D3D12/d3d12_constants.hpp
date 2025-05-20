//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-05-17 by alfonsmagd


#pragma once 

#include <pch.h>
#include <d3d12.h>
#include "Ifnity/Graphics/Interfaces/IBuffer.hpp"


IFNITY_NAMESPACE




namespace D3D12
{
	using namespace Microsoft::WRL;

	struct FormatMapping
	{
		rhi::Format rhiFormat;
		DXGI_FORMAT dxgiFormat;
	};

	static const std::array<FormatMapping, size_t( rhi::Format::COUNT )> c_FormatMap = { {
		{ rhi::Format::R8G8B8_UINT,           DXGI_FORMAT_UNKNOWN },
		{ rhi::Format::R8G8B8,                DXGI_FORMAT_UNKNOWN },
		{ rhi::Format::R8G8B8A8,              DXGI_FORMAT_R8G8B8A8_UNORM },
		{ rhi::Format::R32G32B32_FLOAT,       DXGI_FORMAT_R32G32B32_FLOAT },
		{ rhi::Format::R8G8B8A8_UNORM,        DXGI_FORMAT_R8G8B8A8_UNORM },
		{ rhi::Format::B8G8R8A8_UNORM,        DXGI_FORMAT_B8G8R8A8_UNORM },
		{ rhi::Format::R_UNORM8,              DXGI_FORMAT_R8_UNORM },
		{ rhi::Format::R_UINT16,              DXGI_FORMAT_R16_UINT },
		{ rhi::Format::R_UINT32,              DXGI_FORMAT_R32_UINT },
		{ rhi::Format::R_UNORM16,             DXGI_FORMAT_R16_UNORM },
		{ rhi::Format::R_FLOAT16,             DXGI_FORMAT_R16_FLOAT },
		{ rhi::Format::R_FLOAT32,             DXGI_FORMAT_R32_FLOAT },
		{ rhi::Format::RG_UNORM8,             DXGI_FORMAT_R8G8_UNORM },
		{ rhi::Format::RG_UINT16,             DXGI_FORMAT_R16G16_UINT },
		{ rhi::Format::RG_UINT32,             DXGI_FORMAT_R32G32_UINT },
		{ rhi::Format::RG_UNORM16,            DXGI_FORMAT_R16G16_UNORM },
		{ rhi::Format::RG_FLOAT16,            DXGI_FORMAT_R16G16_FLOAT },
		{ rhi::Format::R32G32_FLOAT,          DXGI_FORMAT_R32G32_FLOAT },
		{ rhi::Format::RGBA_UNORM8,           DXGI_FORMAT_R8G8B8A8_UNORM },
		{ rhi::Format::RGBA_UINT32,           DXGI_FORMAT_R32G32B32A32_UINT },
		{ rhi::Format::RGBA_FLOAT16,          DXGI_FORMAT_R16G16B16A16_FLOAT },
		{ rhi::Format::RGBA_FLOAT32,          DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ rhi::Format::RGBA_SRGB8,            DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
		{ rhi::Format::BGRA_UNORM8,           DXGI_FORMAT_B8G8R8A8_UNORM },
		{ rhi::Format::BGRA_SRGB8,            DXGI_FORMAT_B8G8R8A8_UNORM_SRGB },
		{ rhi::Format::ETC2_RGB8,             DXGI_FORMAT_UNKNOWN }, //not supported
		{ rhi::Format::ETC2_SRGB8,            DXGI_FORMAT_UNKNOWN }, //not supported. 
		{ rhi::Format::BC7_RGBA,              DXGI_FORMAT_BC7_UNORM },
		{ rhi::Format::Z_UNORM16,             DXGI_FORMAT_D16_UNORM },
		{ rhi::Format::Z_UNORM24,             DXGI_FORMAT_D24_UNORM_S8_UINT },
		{ rhi::Format::Z_FLOAT32,             DXGI_FORMAT_D32_FLOAT },
		{ rhi::Format::Z_UNORM24_S_UINT8,     DXGI_FORMAT_D24_UNORM_S8_UINT },
		{ rhi::Format::Z_FLOAT32_S_UINT8,     DXGI_FORMAT_D32_FLOAT_S8X24_UINT },
		} };

	/**
	* @brief Get the RHI format corresponding to the given DXGI format.
	* @param dxgiFormat The DXGI format.
	* @return The corresponding RHI format, or rhi::Format::UNKNOWN if not found.
	*/
	inline rhi::Format getRHIFormat( DXGI_FORMAT dxgiFormat )
	{
		for( const auto& mapping : c_FormatMap )
		{
			if( mapping.dxgiFormat == dxgiFormat )
				return mapping.rhiFormat;
		}
		return rhi::Format::UNKNOWN;
	}

	/**
	* @brief Converts RHI format to DXGI format.
	*/
	inline DXGI_FORMAT formatToDxgiFormat( rhi::Format format )
	{
		return static_cast< uint32_t >(format) < c_FormatMap.size()
			? c_FormatMap[ static_cast< size_t >(format) ].dxgiFormat
			: DXGI_FORMAT_UNKNOWN;
	}



	inline BOOL convertToDxFrontCounterClockwise( rhi::FrontFaceType mode )
	{
		switch( mode )
		{
			case rhi::FrontFaceType::Clockwise: return FALSE;
			case rhi::FrontFaceType::CounterClockwise: return TRUE;
			default: return TRUE;
		}
	}


	inline D3D12_CULL_MODE convertToDxCullMode( rhi::CullModeType mode )
	{
		switch( mode )
		{
			case rhi::CullModeType::None: return D3D12_CULL_MODE_NONE;
			case rhi::CullModeType::Front: return D3D12_CULL_MODE_FRONT;
			case rhi::CullModeType::Back: return D3D12_CULL_MODE_BACK;
			default: return D3D12_CULL_MODE_BACK;
		}
	}

	inline D3D12_FILL_MODE convertToDxFillMode( rhi::PolygonModeType mode )
	{
		switch( mode )
		{
			case rhi::PolygonModeType::Fill: return D3D12_FILL_MODE_SOLID;
			case rhi::PolygonModeType::Line: return D3D12_FILL_MODE_WIREFRAME;
			default: return D3D12_FILL_MODE_SOLID;
		}
	}

	inline D3D12_BLEND convertToDxBlend( rhi::BlendFactor factor )
	{
		switch( factor )
		{
			case rhi::BlendFactor::ZERO: return D3D12_BLEND_ZERO;
			case rhi::BlendFactor::ONE: return D3D12_BLEND_ONE;
			case rhi::BlendFactor::SRC_COLOR: return D3D12_BLEND_SRC_COLOR;
			case rhi::BlendFactor::ONE_MINUS_SRC_COLOR: return D3D12_BLEND_INV_SRC_COLOR;
			case rhi::BlendFactor::SRC_ALPHA: return D3D12_BLEND_SRC_ALPHA;
			case rhi::BlendFactor::ONE_MINUS_SRC_ALPHA: return D3D12_BLEND_INV_SRC_ALPHA;
			default: return D3D12_BLEND_ONE;
		}
	}

	inline D3D12_PRIMITIVE_TOPOLOGY convertToDxTopology( rhi::PrimitiveType type )
	{
		switch( type )
		{
			case rhi::PrimitiveType::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case rhi::PrimitiveType::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			case rhi::PrimitiveType::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case rhi::PrimitiveType::PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			default: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
	}

	inline D3D12_COMPARISON_FUNC convertToDxCompareFunc( rhi::CompareOp op )
	{
		switch( op )
		{
			case rhi::CompareOp::CompareOp_Less: return D3D12_COMPARISON_FUNC_LESS;
			case rhi::CompareOp::CompareOp_Equal: return D3D12_COMPARISON_FUNC_EQUAL;
			case rhi::CompareOp::CompareOp_LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
			case rhi::CompareOp::CompareOp_Greater: return D3D12_COMPARISON_FUNC_GREATER;
			case rhi::CompareOp::CompareOp_NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
			case rhi::CompareOp::CompareOp_GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			case rhi::CompareOp::CompareOp_AlwaysPass: return D3D12_COMPARISON_FUNC_ALWAYS;
			case rhi::CompareOp::CompareOp_Never: return D3D12_COMPARISON_FUNC_NEVER;
			default: return D3D12_COMPARISON_FUNC_ALWAYS;
		}
	}

	inline D3D12_STENCIL_OP convertToDxStencilOp( rhi::StencilOp op )
	{
		switch( op )
		{
			case rhi::StencilOp::StencilOp_Keep: return D3D12_STENCIL_OP_KEEP;
			case rhi::StencilOp::StencilOp_Zero: return D3D12_STENCIL_OP_ZERO;
			case rhi::StencilOp::StencilOp_Replace: return D3D12_STENCIL_OP_REPLACE;
			case rhi::StencilOp::StencilOp_IncrementClamp: return D3D12_STENCIL_OP_INCR_SAT;
			case rhi::StencilOp::StencilOp_DecrementClamp: return D3D12_STENCIL_OP_DECR_SAT;
			case rhi::StencilOp::StencilOp_Invert: return D3D12_STENCIL_OP_INVERT;
			case rhi::StencilOp::StencilOp_IncrementWrap: return D3D12_STENCIL_OP_INCR;
			case rhi::StencilOp::StencilOp_DecrementWrap: return D3D12_STENCIL_OP_DECR;
			default: return D3D12_STENCIL_OP_KEEP;
		}
	}


	inline D3D12_BLEND ConvertToD3D12Blend(rhi::BlendFactor factor) {
		switch (factor) {
			case rhi::BlendFactor::ZERO: return D3D12_BLEND_ZERO;
			case rhi::BlendFactor::ONE: return D3D12_BLEND_ONE;
			case rhi::BlendFactor::SRC_COLOR: return D3D12_BLEND_SRC_COLOR;
			case rhi::BlendFactor::ONE_MINUS_SRC_COLOR: return D3D12_BLEND_INV_SRC_COLOR;
			case rhi::BlendFactor::SRC_ALPHA: return D3D12_BLEND_SRC_ALPHA;
			case rhi::BlendFactor::ONE_MINUS_SRC_ALPHA: return D3D12_BLEND_INV_SRC_ALPHA;
			default: return D3D12_BLEND_ONE;
		}
	}

	inline D3D12_BLEND_OP ConvertToD3D12BlendOp(rhi::BlendFactor op) {
		switch (op) {
			case rhi::BlendFactor::OPERATION_ADD: return D3D12_BLEND_OP_ADD;
			default: return D3D12_BLEND_OP_ADD;
		}
	}

	struct D3D12UsageMapping
	{
		BufferType type;
		D3D12_RESOURCE_FLAGS resourceFlags;
		D3D12_RESOURCE_STATES initialState;
		D3D12_RESOURCE_STATES bindState;
	};


	constexpr D3D12UsageMapping d3d12UsageMappings[] = {

		{ BufferType::INDEX_BUFFER, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_INDEX_BUFFER },
		{ BufferType::VERTEX_BUFFER, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER },
		{ BufferType::DEFAULT_BUFFER, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER },
		{ BufferType::STORAGE_BUFFER, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS },
		{ BufferType::INDIRECT_BUFFER, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT },
		{ BufferType::UNIFORM_BUFFER, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER },
		{ BufferType::CONSTANT_BUFFER, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER }

	};

	constexpr inline D3D12UsageMapping getD3D12UsageMapping( BufferType type )
	{
		for( const auto& mapping : d3d12UsageMappings )
		{
			if( mapping.type == type )
				return mapping;
		}
		return { BufferType::NO_DEFINE_BUFFER, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON };
	}

	inline D3D12_HEAP_TYPE storageTypeToD3D12HeapType( StorageType storage )
	{
		switch( storage )
		{
			case StorageType::DEVICE:
				// GPU-only access, default heap
				return D3D12_HEAP_TYPE_DEFAULT;

			case StorageType::HOST_VISIBLE:
				// CPU-writeable (for upload)
				return D3D12_HEAP_TYPE_UPLOAD;

			case StorageType::MEMORYLESS:
				// Not really supported directly; fallback to DEFAULT
				// You could use D3D12_HEAP_TYPE_DEFAULT and avoid persisting it
				return D3D12_HEAP_TYPE_DEFAULT;

			default:
				IFNITY_LOG( LogCore, ERROR, "Invalid storage type" );
				return D3D12_HEAP_TYPE_DEFAULT;
		}
	}

	inline DXGI_SAMPLE_DESC getD3D12SampleDesc(uint32_t numSamples, UINT maxSupportedSamples = D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT )
	{
		DXGI_SAMPLE_DESC sampleDesc = { 1, 0 }; // Default: no MSAA

		if (numSamples <= 1 || !(maxSupportedSamples & D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT))
			return sampleDesc;

		if (numSamples <= 2 && (maxSupportedSamples & 2)) {
			sampleDesc.Count = 2;
		}
		else if (numSamples <= 4 && (maxSupportedSamples & 4)) {
			sampleDesc.Count = 4;
		}
		else if (numSamples <= 8 && (maxSupportedSamples & 8)) {
			sampleDesc.Count = 8;
		}
		else if (numSamples <= 16 && (maxSupportedSamples & 16)) {
			sampleDesc.Count = 16;
		}
		// D3D12 usually doesn’t support 32 or 64 samples — skip for safety.

		return sampleDesc;
	}


}//end namespace D3D12


IFNITY_END_NAMESPACE
