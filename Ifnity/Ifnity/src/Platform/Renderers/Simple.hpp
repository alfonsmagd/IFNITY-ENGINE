#pragma once

#include "pch.h"
#include  <Ifnity\Graphics\Interfaces\ITexture.hpp>
#include  <Ifnity\Graphics\Interfaces\IDevice.hpp>



IFNITY_NAMESPACE


//Implementing CRTP Renderer
template<typename Derived>
class RendererPassCRTP: public IRendererPass
{
public:
	RendererPassCRTP() = default;
	virtual ~RendererPassCRTP() = default;
	// Initialize the renderer
	void Initialize( DeviceHandle device, unsigned int sizeX, unsigned int sizeY ) override
	{
		static_cast< Derived* >(this)->Initialize( device, sizeX, sizeY );
	}
	// Shutdown the renderer
	void Shutdown() override
	{
		static_cast< Derived* >(this)->Shutdown();
	}
	// Render the scene
	void Render() override
	{
		static_cast< Derived* >(this)->Render();
	}
};


class SimpleRenderer final: public RendererPassCRTP<SimpleRenderer>
{
public:
	SimpleRenderer();
	~SimpleRenderer();

	void Initialize( DeviceHandle device, unsigned int sizeX, unsigned int sizeY );
	void Shutdown();
	void Render();



private:
	TextureHandle  m_colorTexture;
	TextureHandle  m_depthTexture;

	rhi::Framebuffer<TextureHandle> m_framebuffer;

	DeviceHandle m_device;
};
















IFNITY_END_NAMESPACE

