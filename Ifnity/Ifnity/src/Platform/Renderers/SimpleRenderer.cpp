#pragma once

#include "pch.h"
#include "Simple.hpp"


IFNITY_NAMESPACE

SimpleRenderer::SimpleRenderer()
{
	IFNITY_LOG( LogApp, INFO, "SimpleRenderer created" );
	IFNITY_LOG( LogApp, WARNING, "Not forget bind pipeline handle , or call create it" );	
}

SimpleRenderer::~SimpleRenderer()
{
}

void SimpleRenderer::Initialize(DeviceHandle device, unsigned int sizeX, unsigned int sizeY)
{
	if(!device)
	{
		IFNITY_LOG(LogApp, ERROR, "Device is null in SimpleRenderer::Initialize");
		return;
	}

	m_device = device;

	//Get Information

	// Create color texture
	TextureDescription colorDesc;
	colorDesc.format = rhi::Format::R8G8B8A8;
	colorDesc.dimension = rhi::TextureType::TEXTURE2D;
	colorDesc.flags = rhi::TextureUsageBits::ATTACHMENT | TextureDescription::IS_RENDER_TARGET;
	colorDesc.width = sizeX;
	colorDesc.height = sizeY;

	m_colorTexture = m_device->CreateTexture(colorDesc);

	// Create depth texture
	TextureDescription depthDesc;
	depthDesc.format = rhi::Format::Z_UNORM16;
	depthDesc.dimension = rhi::TextureType::TEXTURE2D;
	depthDesc.flags = rhi::TextureUsageBits::ATTACHMENT | TextureDescription::IS_RENDER_TARGET;
	depthDesc.width = sizeX;
	depthDesc.height = sizeY;

	m_depthTexture = m_device->CreateTexture(depthDesc);

	//Set Framebuffer

	rhi::Framebuffer<TextureHandle>::AttachmentDesc colorAttachment;
	colorAttachment.texture = m_colorTexture;

	m_framebuffer.addAttachment( colorAttachment, 0 );
	m_framebuffer.depthStencil = { .texture = m_depthTexture };

}

void SimpleRenderer::Shutdown()
{
	
}

void SimpleRenderer::Render()
{
	
}

IFNITY_END_NAMESPACE

