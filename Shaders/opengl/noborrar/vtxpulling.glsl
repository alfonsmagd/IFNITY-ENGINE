#version 450

out gl_PerVertex
{
    vec4 gl_Position;
};

struct Vertex
{
    float p[3];
    float tc[2];
};

layout(binding = 0, std140) uniform type_PerFrameData
{
    mat4 MVP;
} PerFrameData;

layout(binding = 1, std430) readonly buffer type_StructuredBuffer_Vertex
{
    Vertex _m0[];
} in_Vertices;

layout(location = 0) out vec2 out_var_TEXCOORD0;

void main()
{
    gl_Position = PerFrameData.MVP * vec4(in_Vertices._m0[uint(gl_VertexID)].p[0], in_Vertices._m0[uint(gl_VertexID)].p[1], in_Vertices._m0[uint(gl_VertexID)].p[2], 1.0);
    out_var_TEXCOORD0 = vec2(in_Vertices._m0[uint(gl_VertexID)].tc[0], in_Vertices._m0[uint(gl_VertexID)].tc[1]);
}

