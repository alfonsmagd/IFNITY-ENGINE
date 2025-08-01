



#include "gl_Classes.hpp"



IFNITY_NAMESPACE

namespace OpenGL
{
	

	GLFrameBuffer::GLFrameBuffer( const rhi::Framebuffer<TextureHandle>& desc )
	{
		// Set the draw buffers
		std::vector<GLenum> drawBuffers;
		// Create the framebuffer
		glCreateFramebuffers( 1, &m_framebufferID );

		// Attach color textures
		for( size_t i = 0; i < desc.getNumColorAttachments(); ++i )
		{
			const auto& tex = desc.color[ i ].texture;
			//Save the color attachment
			m_ColorAttachments.push_back( { GL_COLOR_ATTACHMENT0 + i, tex } );
			if( tex )
			{
				glNamedFramebufferTexture( m_framebufferID, GL_COLOR_ATTACHMENT0 + i, tex->GetTextureID(), 0 );
				// Set the draw buffers
				drawBuffers.push_back( GL_COLOR_ATTACHMENT0 + i );
			}
		}

		// Attach depth texture
		m_DepthAttachment = desc.depthStencil.texture->GetTextureID();
		if( m_DepthAttachment )
		{
			glNamedFramebufferTexture( m_framebufferID, GL_DEPTH_ATTACHMENT, m_DepthAttachment, 0 );
		}

		// Check framebuffer completeness
		if( glCheckNamedFramebufferStatus( m_framebufferID, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			 IFNITY_LOG( LogApp, ERROR, "Framebuffer is not complete! in OpenGl FrameBuffer" );
			 return;
		}

		if( !m_ColorAttachments.empty() )
			glNamedFramebufferDrawBuffers( m_framebufferID, drawBuffers.size(), drawBuffers.data() );

		// Unbind the framebuffer clear operation
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	GLFrameBuffer::~GLFrameBuffer()
	{
		glDeleteFramebuffers( 1, &m_framebufferID );
	}

	void GLFrameBuffer::bindAsInput(GLuint framebuffer, uint8_t baseSlot )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );

		for( size_t i = 0; i < m_ColorAttachments.size(); ++i )
		{
			const TextureHandle& tex = m_ColorAttachments[ i ].second;
			if( tex )
				glBindTextureUnit( baseSlot + i, tex->GetTextureID() );
		}
		//The last is the depth attachment
		if( m_DepthAttachment )
		{
			uint8_t depthSlot = baseSlot + static_cast<uint8_t>( m_ColorAttachments.size() );
			glBindTextureUnit( depthSlot, m_DepthAttachment );
		}
	}

}














IFNITY_END_NAMESPACE