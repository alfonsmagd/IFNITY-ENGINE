#version 450

uniform sampler2D _25;

layout(location = 0) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec4 out_var_SV_Target;

void main()
{
    out_var_SV_Target = texture(_25, in_var_TEXCOORD0);
}

