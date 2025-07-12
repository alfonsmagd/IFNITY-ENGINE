#version 450

out gl_PerVertex
{
    vec4 gl_Position;
};

const vec2 _34[3] = vec2[](vec2(-0.5), vec2(0.0, 0.5), vec2(0.5, -0.5));
const vec2 _38[3] = vec2[](vec2(0.0), vec2(1.0, 0.0), vec2(0.5, 1.0));

layout(binding = 0, std140) uniform type_PerFrameData
{
    mat4 MVP;
} PerFrameData;

layout(location = 0) out vec2 out_var_TEXCOORD0;

void main()
{
    gl_Position = PerFrameData.MVP * vec4(_34[uint(gl_VertexID)], 0.0, 1.0);
    out_var_TEXCOORD0 = _38[uint(gl_VertexID)];
}

