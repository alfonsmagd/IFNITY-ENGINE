



#include "d3d12_classes.hpp"
#include "d3d12_Buffer.hpp"
#include "Platform\Windows\DeviceD3D12.hpp"


IFNITY_NAMESPACE


namespace D3D12
{

	//Generic Implementation Destroy function 
	template<typename Handle>
	void destroy(DeviceD3D12* ctx, Handle handle)
	{
		if(ctx)
		{
			ctx->destroy(handle);
		}
	}

	//Make linker happy =) 
	template void destroy<BufferHandleSM>(DeviceD3D12*, BufferHandleSM);
	template void destroy<TextureHandleSM>( DeviceD3D12*, TextureHandleSM );



}
IFNITY_END_NAMESPACE