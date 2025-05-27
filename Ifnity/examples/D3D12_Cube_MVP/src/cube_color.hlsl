
struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};


cbuffer root : register(b0)
{
     float4x4 mvp; 
};


struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(mvp, input.position); 
    output.color = input.color;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}