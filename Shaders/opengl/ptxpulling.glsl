#version 450

layout(binding = 0, std140) uniform type_PerFrameData
{
    mat4 MVP;
    float thickness;
} PerFrameData;

uniform sampler2D _55;

layout(location = 0) in vec2 in_var_TEXCOORD0;
layout(location = 1) in vec3 in_var_TEXCOORD1;
layout(location = 0) out vec4 out_var_SV_TARGET;

void main()
{
    vec4 _40 = texture(_55, in_var_TEXCOORD0);
    vec3 _46 = smoothstep(vec3(0.0), fwidth(in_var_TEXCOORD1) * PerFrameData.thickness, in_var_TEXCOORD1);
    float _47 = _46.x;
    float _48 = _46.y;
    float _49 = isnan(_48) ? _47 : (isnan(_47) ? _48 : min(_47, _48));
    float _50 = _46.z;
    out_var_SV_TARGET = mix(_40 * vec4(0.800000011920928955078125, 0.800000011920928955078125, 0.800000011920928955078125, 1.0), _40, vec4(isnan(_50) ? _49 : (isnan(_49) ? _50 : min(_49, _50))));
}

