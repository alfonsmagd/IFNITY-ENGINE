//VERTEX SHADER SM6.6 BINDLESS//
//Only draw triangle with a texture 
#if defined(__INTELLISENSE__) || defined(__SHADER_API_RESOURCE_DESCRIPTOR_HEAP)
Texture2D<float4> ResourceDescriptorHeap[] : register(t0, space0);
#endif

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = input.position;
    output.color = input.color;
    output.uv = input.uv;
    return output;
}

//---PIXEL---//

// SM 6.6 bindless + root constants

struct RootConstants
{
    uint texIndex;
};

ConstantBuffer<RootConstants> root : register(b0);
SamplerState g_sampler : register(s0); // Sampler declarado en root signature


float4 PSMain(PSInput input) : SV_TARGET
{
    Texture2D<float4> tex = ResourceDescriptorHeap[root.texIndex];
    return tex.Sample(g_sampler, input.uv);
}