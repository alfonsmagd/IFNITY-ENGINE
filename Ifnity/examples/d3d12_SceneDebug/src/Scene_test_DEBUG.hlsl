//VERTEX SHADER SM6.6 BINDLESS//
//Only draw triangle with a texture 

// SM 6.6 bindless + root constants

float3x3 inverse3x3(float3x3 m)
{
    float a00 = m[0].x, a01 = m[0].y, a02 = m[0].z;
    float a10 = m[1].x, a11 = m[1].y, a12 = m[1].z;
    float a20 = m[2].x, a21 = m[2].y, a22 = m[2].z;

    float b01 = a22 * a11 - a12 * a21;
    float b11 = -a22 * a10 + a12 * a20;
    float b21 = a21 * a10 - a11 * a20;

    float det = a00 * b01 + a01 * b11 + a02 * b21;

    // Seguridad: evita dividir por 0
    if (abs(det) < 1e-5f)
        return float3x3(1.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 1.0f);

    float invDet = 1.0f / det;

    float3x3 inv;
    inv[0].x = (a11 * a22 - a12 * a21) * invDet;
    inv[0].y = -(a01 * a22 - a02 * a21) * invDet;
    inv[0].z = (a01 * a12 - a02 * a11) * invDet;

    inv[1].x = -(a10 * a22 - a12 * a20) * invDet;
    inv[1].y = (a00 * a22 - a02 * a20) * invDet;
    inv[1].z = -(a00 * a12 - a02 * a10) * invDet;

    inv[2].x = (a10 * a21 - a11 * a20) * invDet;
    inv[2].y = -(a00 * a21 - a01 * a20) * invDet;
    inv[2].z = (a00 * a11 - a01 * a10) * invDet;

    return inv;
}

struct RootConstants
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    float4 cameraPos;
    uint texIndex;
    uint indexTransform;
    uint drawIndex;
    uint baseInstance;
};

struct VSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    uint instanceID : SV_InstanceID; // Instance ID for instancing

};

struct DrawData
{
    uint transformID;
    uint materialID;
};
struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldNormal : NORMAL;
    float3 worldPos : TEXCOORD1;
    float2 uv : TEXCOORD0;
};

    
ConstantBuffer<RootConstants> root : register(b0);
SamplerState g_sampler : register(s0);


PSInput VSMain(VSInput input)
{
    PSInput output;

    StructuredBuffer<float4x4> model_rotate = ResourceDescriptorHeap[root.indexTransform];
    StructuredBuffer<DrawData> drawDataBuffer = ResourceDescriptorHeap[root.drawIndex];
    
    DrawData drawData = drawDataBuffer[root.baseInstance];
    float4x4 modelr = model_rotate[drawData.transformID];
    
    float4 modelpos = mul(modelr, float4(input.position, 1.0f));
    
    // Model-View-Projection transform
    float4 worldPos = mul(root.model, modelpos);
    //float4 viewPos = mul(root.view, worldPos);
    output.position = mul(root.proj, modelpos);
    
    // Invert the model matrix to get the normal matrix)
    
    
    float3x3 model3x3 = (float3x3) root.model;
    float3x3 normalMatrix = transpose(inverse3x3(model3x3));
    output.worldNormal = mul(normalMatrix, input.normal);
    output.worldPos = worldPos.xyz;
    output.uv = input.uv;

    return output;
}

//---PIXEL---//
float4 PSMain(PSInput input) : SV_TARGET
{
    float3 normal = normalize(input.worldNormal);
    float3 viewDir = normalize(root.cameraPos.xyz - input.worldPos);
    float3 lightDir = normalize(float3(0.0f, 0.0f, -1.0f));
    
    float3 lightVec = (lightDir.xyz - input.worldPos);
    // Luz fija desde el frente (igual que vec3(0,0,-1) en GLSL)
    
    
    float NdotL = clamp(dot(normal, lightVec), 0.1f, 1.0f);

    Texture2D texColor = ResourceDescriptorHeap[root.texIndex];
    float4 color = texColor.SampleLevel(g_sampler, input.uv, 0);
    
    float4 Kd = color * NdotL;

    return float4(input.worldNormal.x, input.worldNormal.y, input.worldNormal.z, 1.0f);
}