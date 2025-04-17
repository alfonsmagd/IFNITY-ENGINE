#version 460
      #extension GL_EXT_buffer_reference_uvec2 : require
      #extension GL_EXT_debug_printf : enable
      #extension GL_EXT_nonuniform_qualifier : require
      #extension GL_EXT_samplerless_texture_functions : require
      #extension GL_EXT_shader_explicit_arithmetic_types_float16 : require
      
      layout (set = 0, binding = 0) uniform texture2D kTextures2D[];
      layout (set = 1, binding = 0) uniform texture3D kTextures3D[];
      layout (set = 2, binding = 0) uniform textureCube kTexturesCube[];
      layout (set = 3, binding = 0) uniform texture2D kTextures2DShadow[];
      layout (set = 0, binding = 1) uniform sampler kSamplers[];
      layout (set = 3, binding = 1) uniform samplerShadow kSamplersShadow[];

      layout (set = 0, binding = 3) uniform sampler2D kSamplerYUV[];

      vec4 textureBindless2D(uint textureid, uint samplerid, vec2 uv) {
        return texture(nonuniformEXT(sampler2D(kTextures2D[textureid], kSamplers[samplerid])), uv);
      }
      
         //

//
#include <Shaders/common.sp>

layout (location=0) in PerVertex vtx;

layout (location=0) out vec4 out_FragColor;


void main() {
	vec3 n = normalize(vtx.worldNormal);
	vec3 v = normalize(pc.cameraPos.xyz - vtx.worldPos);
	//vec3 reflection = -normalize(reflect(v, n));

	//vec4 colorRefl = textureBindlessCube(pc.texCube, 0, reflection);
	//vec4 Ka = colorRefl * 0.3;

	float NdotL = clamp(dot(n, normalize(vec3(0,0,-1))), 0.1, 1.0);
	vec4 Kd = textureBindless2D(pc.tex, 0, vtx.uv) * NdotL;

	out_FragColor =  Kd;
};