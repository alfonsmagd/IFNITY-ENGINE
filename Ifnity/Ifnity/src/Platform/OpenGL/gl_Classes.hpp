




#include "pch.h"
#include "Platform/Windows/gl_constans.hpp"

IFNITY_NAMESPACE

//Forward declarations
class TextureHandle;
namespace rhi {
	template<typename T>
	class Framebuffer;
}

namespace OpenGl
{
	class GLFrameBuffer final
	{
	public:
		GLFrameBuffer(const rhi::Framebuffer<TextureHandle>& desc);

		~GLFrameBuffer();

		void bindAsInput( uint8_t baseSlot = 0 );

	private:
		std::vector<std::pair<GLuint, TextureHandle>> m_ColorAttachments;
		GLuint m_framebufferID;
		GLuint m_DepthAttachment; ///< Depth attachment for the framebuffer.
	};




}

IFNITY_END_NAMESPACE