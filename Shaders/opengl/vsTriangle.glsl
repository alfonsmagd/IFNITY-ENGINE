#version 450

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(binding = 0, std140) uniform type_PerFrameData
{
    mat4 MVP;
} PerFrameData;

layout(location = 0) in vec3 in_var_POSITION0;
layout(location = 1) in vec3 in_var_COLOR1;
layout(location = 0) out vec3 out_var_COLOR;

void main()
{
    gl_Position = PerFrameData.MVP * vec4(in_var_POSITION0, 1.0);
    out_var_COLOR = in_var_COLOR1;
}

