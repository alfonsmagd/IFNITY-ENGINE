#ifndef AUX_CODE_HLSL
#define AUX_CODE_HLSL


float Clamp01(float value)
{
    return saturate(value);
}

float3 Clamp01(float3 value)
{
    return saturate(value);
}

float3 SafeNormalize(float3 v)
{
    return length(v) > 0.0001 ? normalize(v) : float3(0, 0, 1);
}

// Transformacin completa con matriz MVP
float4 TransformPosition(float3 pos, float4x4 model, float4x4 view, float4x4 proj)
{
    return mul(proj, mul(view, mul(model, float4(pos, 1.0))));
}

// Lambert lighting bsico
float3 LambertDiffuse(float3 normal, float3 lightDir, float3 lightColor)
{
    float NdotL = max(dot(normal, lightDir), 0.0);
    return lightColor * NdotL;
}

// Convierte color de gamma a linear space
float3 GammaToLinear(float3 color)
{
    return pow(color, 2.2);
}

// Convierte color de linear a gamma space
float3 LinearToGamma(float3 color)
{
    return pow(color, 1.0 / 2.2);
}

#endif // AUX_CODE_HLSL
