#version 450

layout(location = 0) in vec3 in_var_COLOR;
layout(location = 0) out vec4 out_var_SV_Target0;

void main()
{
    out_var_SV_Target0 = vec4(in_var_COLOR, 1.0);
}

