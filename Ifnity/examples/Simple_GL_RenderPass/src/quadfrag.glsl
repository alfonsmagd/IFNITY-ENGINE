//
#version 460 core

layout (location=0) in vec2 uv;
layout (location=0) out vec4 fragColor;


layout(binding = 0) uniform sampler2D texScene;

void main()
{
	// Sample the texture at the given UV coordinates only.
	fragColor = texture(texScene, uv);
}
