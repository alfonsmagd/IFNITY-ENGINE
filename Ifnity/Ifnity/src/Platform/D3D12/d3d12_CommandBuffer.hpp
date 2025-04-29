#pragma once

#include <pch.h>
#include "d3d12_classes.hpp"
#include "d3d12_ImmediateCommands.hpp"
#include <d3d12.h>
#include <wrl.h>

IFNITY_NAMESPACE



class DeviceD3D12;

namespace D3D12
{
	using namespace Microsoft::WRL;

	class CommandBuffer final
	{

	public:
		CommandBuffer() = default;
		explicit CommandBuffer(DeviceD3D12* ctx);
		~CommandBuffer();

		CommandBuffer& operator=(CommandBuffer&& other) = default;



	private:
		friend class DeviceD3D12;

		DeviceD3D12* ctx_ = nullptr;
		const D3D12ImmediateCommands::CommandListWrapper* wrapper_;

		SubmitHandle lastSubmitHandle_ = {};
		//Framebuffer framebuffer_ = {};

		bool isRendering_ = false;



	};


}
IFNITY_END_NAMESPACE