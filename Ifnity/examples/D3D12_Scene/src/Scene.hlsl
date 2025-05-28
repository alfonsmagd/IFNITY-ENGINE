
//Auxiliary Functions 
void runAlphaTest(float alpha, float alphaThreshold, float2 fragCoord)
{
    if (alphaThreshold > 0.0f)
    {
         // http://alex-charlton.com/posts/Dithering_on_the_GPU/
        // https://forums.khronos.org/showthread.php/5091-screen-door-transparency
        // 4x4 Bayer matrix normalized
        float thresholdMatrix[4][4] =
        {
            { 1.0f / 17.0f, 9.0f / 17.0f, 3.0f / 17.0f, 11.0f / 17.0f },
            { 13.0f / 17.0f, 5.0f / 17.0f, 15.0f / 17.0f, 7.0f / 17.0f },
            { 4.0f / 17.0f, 12.0f / 17.0f, 2.0f / 17.0f, 10.0f / 17.0f },
            { 16.0f / 17.0f, 8.0f / 17.0f, 14.0f / 17.0f, 6.0f / 17.0f }
        };

        int x = int(fmod(fragCoord.x, 4.0f));
        int y = int(fmod(fragCoord.y, 4.0f));

        alpha = clamp(alpha - 0.5f * thresholdMatrix[y][x], 0.0f, 1.0f);

        if (alpha < alphaThreshold)
            discard;
    }
}

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

//Material Data 
//I respect the order in other shaders because uint64_t is not available in D3D12 and one way to simulate it is to use two uint32_t
struct MaterialData
{
    float4 emissiveColor;
    float4 albedoColor;
    float4 roughness;

    float transparencyFactor;
    float alphaTest;
    float metallicFactor;
    uint flags;

    uint2 ambientOcclusionMap; // in opengl and vulkan this is a uint32_t, // but in d3d12 uint64_t its not avaiblable
    uint2 emissiveMap;
    uint2 albedoMap;
    uint2 metallicRoughnessMap;
    uint2 normalMap;
    uint2 opacityMap;
};

//Draw Data
struct DrawData
{
    uint transformId;
    uint materialId;
};

//Transform Data
struct TransformsData
{
    float4x4 model;
};

struct RootConstant
{
    float4x4 viewProj;
    uint bufferTransformsIndex;   //Index SRV bindless 
    uint bufferDradDataIndex;     //Index SRV bindless
    uint bufferMaterialsIndex;    //Index SRV bindless
    uint _pad0; 
    uint _pad1[35];
    uint baseInstance;
    
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct  PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 worldNormal : TEXCOORD1;
    float2 uv : TEXCOORD2;
    uint materialId : TEXCOORD3; // Material ID for pixel shader
};



ConstantBuffer<RootConstant> root : register(b0);
SamplerState g_sampler : register(s0);



PSInput VSMain(VSInput input, uint instanceID : SV_InstanceID)
{
    
    StructuredBuffer<DrawData> drawDataBuffer         = ResourceDescriptorHeap[root.bufferDradDataIndex];
    StructuredBuffer<TransformsData> transformsBuffer = ResourceDescriptorHeap[root.bufferTransformsIndex];
    StructuredBuffer<MaterialData> materialsBuffer    = ResourceDescriptorHeap[root.bufferMaterialsIndex];
    
    PSInput output;

    // Acceder a datos por instancia
    DrawData drawData = drawDataBuffer[root.baseInstance];
    TransformsData transform = transformsBuffer[drawData.transformId];
    MaterialData material = materialsBuffer[drawData.materialId];

    // Transformar position and normal
    float4x4 model = transform.model;
    float4 worldPosition = mul(model, float4(input.position, 1.0f));
    float3x3 normalMatrix = transpose(inverse3x3((float3x3) model));

    // Salida hacia el pixel shader
    output.position = mul(root.viewProj, worldPosition);
    output.worldPos = worldPosition.xyz;
    output.worldNormal = mul(normalMatrix, input.normal);
    output.uv = input.uv;
    output.materialId = material.albedoColor.x;

    return output;
}

//---PIXEL---//
float4 PSMain(PSInput input) : SV_TARGET
{
    return input.position;
}