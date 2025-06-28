
//Auxiliary Functions 

// Constant for Pi, useful for angle conversions if needed
static const float M_PI = 3.141592653589793f;

// Converts sRGB color to linear color space
float4 SRGBtoLINEAR(float4 srgbIn)
{
    // Apply gamma correction (approx. gamma = 2.2)
    float3 linOut = pow(srgbIn.rgb, 2.2);
    return float4(linOut, srgbIn.a);
}
// http://www.thetenthplanet.de/archives/1180
// modified to fix handedness of the resulting cotangent frame
// Computes the tangent-bitangent-normal (TBN) matrix from geometry and UV derivatives
float3x3 cotangentFrame(float3 N, float3 p, float2 uv)
{
    // Compute screen-space derivatives of position and UVs
    float3 dp1 = ddx(p);
    float3 dp2 = ddy(p);
    float2 duv1 = ddx(uv);
    float2 duv2 = ddy(uv);

    // Compute perpendicular vectors for constructing tangent and bitangent
    float3 dp2perp = cross(dp2, N);
    float3 dp1perp = cross(N, dp1);

    // Tangent and Bitangent estimation using UV gradient
    float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    float3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // Normalize the TBN frame to be scale-invariant
    float invmax = rsqrt(max(dot(T, T), dot(B, B)));

    // Ensure correct handedness of the coordinate system
    float w = (dot(cross(N, T), B) < 0.0f) ? -1.0f : 1.0f;
    T *= w;

    // Return the orthonormal TBN matrix
    return float3x3(T * invmax, B * invmax, N);
}

// Applies normal map perturbation in tangent space
float3 perturbNormal(float3 n, float3 v, float3 normalSample, float2 uv)
{
    // Remap from [0, 1] to [-1, 1] and normalize
    float3 map = normalize(2.0f * normalSample - 1.0f);

    // Build TBN matrix using geometry and UVs
    float3x3 TBN = cotangentFrame(n, v, uv);

    // Transform the normal map vector from tangent to world space
    return normalize(mul(map, TBN)); // HLSL uses mul(vector, matrix)
}


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
};
struct BaseInstance
{
    uint id;
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
    float3 worldPos : NORMAL;
    float3 worldNormal : TEXCOORD1;
    float2 uv : TEXCOORD0;
    uint materialId : TEXCOORD3; // Material ID for pixel shader
};



ConstantBuffer<RootConstant> root : register(b0);
ConstantBuffer<BaseInstance> baseInstance : register(b1); // Base instance ID for instancing
SamplerState g_sampler : register(s0);



PSInput VSMain(VSInput input, uint instanceID : SV_InstanceID)
{
    
    StructuredBuffer<DrawData> drawDataBuffer         = ResourceDescriptorHeap[root.bufferDradDataIndex];
    StructuredBuffer<TransformsData> transformsBuffer = ResourceDescriptorHeap[root.bufferTransformsIndex];
 
    
    PSInput output;

    // Acceder a datos por instancia
    DrawData drawData = drawDataBuffer[baseInstance.id];
    
    TransformsData transform = transformsBuffer[drawData.transformId];
    

    // Transformar position and normal
    float4x4 model = transform.model;
    float4 modelpos = mul(model, float4(input.position, 1.0f));
    float3x3 normalMatrix = transpose(inverse3x3((float3x3) model));

    // Salida hacia el pixel shader
    output.position = mul(root.viewProj, modelpos);
    output.worldPos = modelpos.xyz;
    output.worldNormal = mul(normalMatrix, input.normal);
    output.uv = input.uv;
    output.materialId = drawData.materialId;

    return output;
}

//---PIXEL---//
float4 PSMain(PSInput input) : SV_TARGET
{
    StructuredBuffer<MaterialData> materialsBuffer = ResourceDescriptorHeap[root.bufferMaterialsIndex];
    //Get material data
    MaterialData mat = materialsBuffer[input.materialId];
    
    float4 albedo = mat.albedoColor;
    float3 normalSample = float3(0.0f, 0.0f, 0.0f);
   
    const int INVALID_HANDLE = 2000;
    // Albedo map
    if (mat.albedoMap.x > 0 && mat.albedoMap.x < INVALID_HANDLE)
    {
        uint texIdx = mat.albedoMap.x;
        Texture2D texAlbedo = ResourceDescriptorHeap[texIdx];
        albedo = texAlbedo.SampleLevel(g_sampler, input.uv, 0.0f);
    }

// Normal map
    if (mat.normalMap.x > 0 && mat.normalMap.x < INVALID_HANDLE)
    {
        uint texIdx = mat.normalMap.x;
        Texture2D texNormal = ResourceDescriptorHeap[texIdx];
        normalSample = texNormal.SampleLevel(g_sampler, input.uv, 0.0f).xyz;
    }

    // Alpha test
    runAlphaTest(albedo.a, mat.alphaTest,input.position.xy);

    // Normal mapping
    float3 n = normalize(input.worldNormal);

    if (length(normalSample) > 0.5f)
        n = perturbNormal(n, input.worldPos, normalSample, input.uv);

    // Light direction and shading
    float3 lightDir = normalize(float3(-1.0f, 1.0f, 0.1f));
    float NdotL = clamp(dot(n, lightDir), 0.3f, 1.0f);

    return float4(albedo.rgb * NdotL, 1.0f);
    
    
    
    //return float4(input.worldNormal.x, input.worldNormal.y, input.materialId, 1.0f);
}