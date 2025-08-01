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

	const rhi::Framebuffer<TextureHandle>& GetFramebuffer() const
	{
		return static_cast< const Derived* >(this)->m_framebuffer;
	}
	TextureHandle GetDepthTexture() const
	{
		return static_cast< const Derived* >(this)->m_depthTexture;
	}

	GraphicsPipelineHandle GetPipeline() const
	{
		return static_cast< const Derived* >(this)->m_pipeline;
	}

	void Accept(IRenderPassVisitor& visitor) override
	{
		visitor.Visit(static_cast<Derived&>(*this));
	}

};


class SimpleRenderer final: public RendererPassCRTP<SimpleRenderer>
{
public:
	SimpleRenderer();
	SimpleRenderer(GraphicsPipelineHandle pipeline) : m_pipeline(pipeline) {}
	~SimpleRenderer();

	void Initialize( DeviceHandle device, unsigned int sizeX, unsigned int sizeY );
	void Shutdown();
	void Render();

protected:
	friend class RendererPassCRTP<SimpleRenderer>;

	GraphicsPipelineHandle m_pipeline;
	TextureHandle  m_colorTexture;
	TextureHandle  m_depthTexture;

	rhi::Framebuffer<TextureHandle> m_framebuffer;

	DeviceHandle m_device;
};
















IFNITY_END_NAMESPACE

