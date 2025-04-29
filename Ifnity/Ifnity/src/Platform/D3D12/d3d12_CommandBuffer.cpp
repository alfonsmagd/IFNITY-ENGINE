
#include "d3d12_CommandBuffer.hpp"
#include "d3d12_ImmediateCommands.hpp"

#include "../Windows/DeviceD3D12.h"


IFNITY_NAMESPACE

namespace D3D12
{

	CommandBuffer::CommandBuffer(DeviceD3D12* ctx): ctx_(ctx), wrapper_(&ctx->m_ImmediateCommands->acquire())
	{}

	CommandBuffer::~CommandBuffer()
	{
	}

}
IFNITY_END_NAMESPACE