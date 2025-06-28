#version 460
#extension GL_EXT_buffer_reference_uvec2 : require
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_samplerless_texture_functions : require
#extension GL_EXT_shader_explicit_arithmetic_types_float16 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64: enable

layout (set = 0, binding = 0) uniform texture2D kTextures2D[];
layout (set = 0, binding = 1) uniform sampler kSamplers[];

vec4 textureBindless2D(uint textureid, uint samplerid, vec2 uv) {
  return texture(nonuniformEXT(sampler2D(kTextures2D[textureid], kSamplers[samplerid])), uv);
}

#include <Shaders/common.sp>
#include <Shaders/AlphaTest.sp>
#include <Shaders/UtilsNormal.sp>


layout (location=0) in vec2 uv;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 worldPos;
layout (location=3) in flat uint materialId;

layout (location=0) out vec4 out_FragColor;

void main() {
  MaterialData mat = pc.materials.material[materialId];

	const int INVALID_HANDLE = 2000;
	vec4 albedo = mat.albedoColor_;
	vec3 normalSample = vec3(0.0);

	// fetch albedo
	if (mat.albedoMap_ < INVALID_HANDLE && mat.albedoMap_ > 0)
	{
		uint texIdx = uint(mat.albedoMap_);
		albedo = textureBindless2D(texIdx, 0, uv);
	}
	if (mat.normalMap_ < INVALID_HANDLE && mat.normalMap_ > 0)
	{
		uint texIdx = uint(mat.normalMap_);
		normalSample = textureBindless2D(texIdx, 0, uv).xyz;
	}
	runAlphaTest(albedo.a, mat.alphaTest_);
  // world-space normal
  vec3 n = normalize(normal);

  if (length(normalSample) > 0.5)
    n = perturbNormal(n, worldPos, normalSample, uv);

  

	vec3 lightDir = normalize(vec3(-1.0, 1.0, 0.1));

	float NdotL = clamp( dot(n, lightDir), 0.3, 1.0 );

	out_FragColor = vec4( albedo.rgb * NdotL, 1.0 );


 
}
