#version 450

out gl_PerVertex
{
    vec4 gl_Position;
};

const vec3 _46[4] = vec3[](vec3(-1.0, 0.0, -1.0), vec3(1.0, 0.0, -1.0), vec3(1.0, 0.0, 1.0), vec3(-1.0, 0.0, 1.0));
const int _47[6] = int[](0, 1, 2, 2, 3, 0);
vec3 _48;

layout(binding = 0, std140) uniform type_PerFrameData
{
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
} PerFrameData;

layout(location = 0) out vec2 out_var_TEXCOORD0;
layout(location = 1) out vec2 out_var_TEXCOORD1;

void main()
{
    vec3 _60 = _46[_47[uint(gl_VertexID)]] * 100.0;
    float _65 = _60.x + PerFrameData.cameraPos.x;
    vec3 _66;
    _66.x = _65;
    float _70 = _60.z + PerFrameData.cameraPos.z;
    _66.z = _70;
    gl_Position = (PerFrameData.proj * PerFrameData.view) * vec4(_65, _60.y, _70, 1.0);
    out_var_TEXCOORD0 = _66.xz;
    out_var_TEXCOORD1 = PerFrameData.cameraPos.xz;
}

