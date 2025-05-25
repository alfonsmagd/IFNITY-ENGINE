//VERTEX SHADER SM6.6 BINDLESS//
//Only draw triangle with a texture 

// SM 6.6 bindless + root constants


struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
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
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
 
};
    
    ConstantBuffer<RootConstants> root : register(b0);
    SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput output;
    
    //Update position with the MVP matrix
    output.position = mul(root.mvp, float4(input.position, 1.0f));
    output.normal = float4(input.normal, 1.0f);
    output.uv = input.uv;
   
    return output;
}

//---PIXEL---//
float4 PSMain(PSInput input) : SV_TARGET
{
   
    
    
    float4 albedo = ResourceDescriptorHeap[root.texIndex].SampleLevel(g_sampler, input.uv, 0);

    float3 normal = normalize(input.normal); // ya en mundo
    float3 lightDir = normalize(float3(0.0f, -1.0f, 0.0f)); // luz fija desde arriba (en mundo)

    float NdotL = saturate(dot(normal, -lightDir)); // Lambert
    float ambient = 0.2f;
    float3 lighting = albedo.rgb * (ambient + (1.0f - ambient) * NdotL);

    return float4(lighting, albedo.a);
    
    
    
    
    
    
    
    
    
    
    
}