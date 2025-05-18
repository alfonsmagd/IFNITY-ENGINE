// Input structure for the vertex shader
struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD0;
};

// Output structure from the vertex shader / input to the pixel shader
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD0;
};

// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
    result.position = input.position;
    result.color = input.color;
    result.texcoord = input.texcoord;
    return result;
}

// Texture and sampler declaration
Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = myTexture.Sample(mySampler, input.texcoord);
    return texColor * input.color; // You can adjust blending here
}