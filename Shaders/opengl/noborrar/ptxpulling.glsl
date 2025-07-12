#version 450

uniform sampler2D _46;

layout(location = 0) in vec2 in_var_TEXCOORD0;
layout(location = 1) in vec3 in_var_TEXCOORD1;
layout(location = 0) out vec4 out_var_SV_TARGET;

void main()
{
    vec4 _33 = texture(_46, in_var_TEXCOORD0);
    vec3 _37 = smoothstep(vec3(0.0), fwidth(in_var_TEXCOORD1) * 1.0, in_var_TEXCOORD1);
    float _38 = _37.x;
    float _39 = _37.y;
    float _40 = isnan(_39) ? _38 : (isnan(_38) ? _39 : min(_38, _39));
    float _41 = _37.z;
    out_var_SV_TARGET = mix(_33 * vec4(0.800000011920928955078125, 0.800000011920928955078125, 0.800000011920928955078125, 1.0), _33, vec4(isnan(_41) ? _40 : (isnan(_40) ? _41 : min(_40, _41))));
}

