

#include "d3d12_Image.hpp"


IFNITY_NAMESPACE

namespace D3D12
{
	D3D12_CPU_DESCRIPTOR_HANDLE D3D12Image::getRTV() const
	{
		//Ternarie expression
		return rtvHandle_;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE D3D12Image::getDSV() const
	{
		return dsvHandle_;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE D3D12Image::getSRV() const
	{
		return srvHandle_;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE D3D12Image::getUAV() const
	{
		return uavHandle_;
	}
	 bool D3D12Image::isDepthFormat(DXGI_FORMAT format)
	{
		switch( format )
		{
			case DXGI_FORMAT_D16_UNORM:
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
			case DXGI_FORMAT_D32_FLOAT:
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
				return true;
			default:
				return false;
		}
	}

	 bool D3D12Image::isDepthStencilFormat(DXGI_FORMAT format)
	{
		return (format == DXGI_FORMAT_D24_UNORM_S8_UINT) ||
			   (format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
	}













} // namespace D3D12












IFNITY_END_NAMESPACE