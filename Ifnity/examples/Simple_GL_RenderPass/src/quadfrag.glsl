//
#version 460 core

layout (location=0) in vec2 uv;
layout (location=0) out vec4 fragColor;


layout(binding = 0) uniform sampler2D texScene;
layout(binding = 1) uniform sampler2D texDepth;

void main()
{

	fragColor = texture(texScene, uv) * vec4(0.0,0.0,1.0,1.0); //filter blue color 
}
