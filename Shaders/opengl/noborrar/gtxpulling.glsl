#version 450
layout(triangles) in;
layout(max_vertices = 3, triangle_strip) out;

in gl_PerVertex
{
    vec4 gl_Position;
} gl_in[];

out gl_PerVertex
{
    vec4 gl_Position;
};

struct GSInput
{
    vec4 position;
    vec2 uv;
};

const vec3 _41[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));

layout(location = 0) in vec2 in_var_TEXCOORD0[3];
layout(location = 0) out vec2 out_var_TEXCOORD0;
layout(location = 1) out vec3 out_var_TEXCOORD1;

void main()
{
    vec4 _44_unrolled[3];
    for (int i = 0; i < int(3); i++)
    {
        _44_unrolled[i] = gl_in[i].gl_Position;
    }
    GSInput param_var_input[3] = GSInput[](GSInput(_44_unrolled[0], in_var_TEXCOORD0[0]), GSInput(_44_unrolled[1], in_var_TEXCOORD0[1]), GSInput(_44_unrolled[2], in_var_TEXCOORD0[2]));
    for (int _57 = 0; _57 < 3; )
    {
        gl_Position = param_var_input[_57].position;
        out_var_TEXCOORD0 = param_var_input[_57].uv;
        out_var_TEXCOORD1 = _41[_57];
        EmitVertex();
        _57++;
        continue;
    }
    EndPrimitive();
}

