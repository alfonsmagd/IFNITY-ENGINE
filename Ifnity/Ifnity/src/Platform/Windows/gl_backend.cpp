#include "gl_backend.hpp"


IFNITY_NAMESPACE


namespace OpenGL
{
	Device::Device()
	{}
	Device::~Device()
	{}
	void Device::Draw(DrawDescription& desc)
	{}

	GraphicsPipeline Device::CreateGraphicsPipeline()
	{
		return GraphicsPipeline();
	}
};

IFNITY_END_NAMESPACE