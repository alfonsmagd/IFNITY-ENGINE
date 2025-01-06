//
#version 460 core

#include <Shaders/GridParameters.h>
#include <Shaders/GridCalculation.h>

layout (location=0) in vec2 uv;
layout (location=1) in vec3 camPos;
layout (location=2) in vec3 fragpos;
layout (location=0) out vec4 out_FragColor;

void main()
{
	out_FragColor = gridColor(uv, camPos.xz);
};
