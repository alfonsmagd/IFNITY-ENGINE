//


#version 460
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_samplerless_texture_functions : require
#extension GL_EXT_shader_explicit_arithmetic_types_float16 : require

#include <Shaders/common.sp>

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location=0) out PerVertex vtx;

void main() {
	gl_Position = pc.proj * pc.view * pc.model * vec4(pos, 1.0);

	mat4 model = pc.model;
	mat3 normalMatrix = transpose( inverse(mat3(pc.model)) );

	vtx.uv = uv;
	vtx.worldNormal = normalMatrix * normal;
	vtx.worldPos = (model * vec4(pos, 1.0)).xyz;
}
