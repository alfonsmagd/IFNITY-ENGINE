#pragma once




#include "pch.h"
#include "Platform/Windows/gl_constans.hpp"
#include <Ifnity\Graphics\Interfaces\ITexture.hpp>

IFNITY_NAMESPACE


namespace OpenGL
{
	#define BACKFRAMEBUFFER_OPENGL_ID 0

	class GLFrameBuffer final
	{
	public:
		GLFrameBuffer(const rhi::Framebuffer<TextureHandle>& desc);

		~GLFrameBuffer();

		void bindAsInput(GLuint framebuffer = BACKFRAMEBUFFER_OPENGL_ID, uint8_t baseSlot = 0);
		void bindAsRenderTarget() {
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
			//glClearNamedFramebufferfv(m_framebufferID, GL_COLOR, 0, glm::value_ptr(vec4(0.0f, 0.0f, 0.0f, 1.0f)));
			glClearNamedFramebufferfi(m_framebufferID, GL_DEPTH_STENCIL, 0, 1.0f, 0);
		}

	private:

		friend class Device;

		std::vector<std::pair<GLuint, TextureHandle>> m_ColorAttachments;
		GLuint m_framebufferID;
		GLuint m_DepthAttachment; ///< Depth attachment for the framebuffer.
	};




}

IFNITY_END_NAMESPACE