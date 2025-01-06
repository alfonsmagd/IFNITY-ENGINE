//
#version 460 core

#include <Shaders/GLBufferDeclarations.h>
#include <Shaders/GridParameters.h>

layout (location=0) out vec2 uv;
layout (location=1) out vec3 out_camPos;
layout (location=2) out vec3 out_fragpos;

void main()
{
	mat4 MVP = proj * view;

	int idx = indices[gl_VertexID];
	vec3 position = pos[idx] * gridSize;
	


	position.x += cameraPos.x;
	position.z += cameraPos.z;

	out_fragpos = (vec4(position,1.0)).xyz;

	out_camPos = cameraPos.xyz;
	
	gl_Position = MVP * vec4(position, 1.0);
	uv = position.xz;
}
