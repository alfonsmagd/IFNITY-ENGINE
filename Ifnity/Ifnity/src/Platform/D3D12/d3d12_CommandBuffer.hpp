//------------------ IFNITY ENGINE SOURCE -------------------//
// Copyright (c) 2025 Alfonso Mateos Aparicio Garcia de Dionisio
// Licensed under the MIT License. See LICENSE file for details.
// Last modified: 2025-04-29 by alfonsmagd


#pragma once

#include <pch.h>
#include "d3d12_classes.hpp"
#include "d3d12_ImmediateCommands.hpp"
#include <d3d12.h>
#include <wrl.h>

IFNITY_NAMESPACE

class DeviceD3D12;
struct ScissorRect;
struct ViewPortState;

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

		void cmdBindViewport(const ViewPortState& state);
		void cmdBindScissorRect(const ScissorRect& rect);

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