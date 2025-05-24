//VERTEX SHADER SM6.6 BINDLESS//
//Only draw triangle with a texture 
//#if defined(__INTELLISENSE__) || defined(__SHADER_API_RESOURCE_DESCRIPTOR_HEAP)
//Texture2D<float4> ResourceDescriptorHeap[] : register(t0, space0);
//#endif

// SM 6.6 bindless + root constants


struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
  
};
struct RootConstants
{   
    float4x4 mvp;
    uint texIndex;
};
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
 
};
    
    ConstantBuffer<RootConstants> root : register(b0);
    SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput output;
    
    //Update position with the MVP matrix
    
    output.position = mul(root.mvp, input.position);
    
    //output.position = input.position;
    output.color = input.color;
    output.uv = input.uv;
   
    return output;
}

//---PIXEL---//
float4 PSMain(PSInput input) : SV_TARGET
{
    Texture2D tex = ResourceDescriptorHeap[root.texIndex];
    
    float4 color = input.color * tex.SampleLevel(g_sampler, input.uv, 0);
    
    return color;
}