#include "particleCommon.fx"

Texture2D sprite : register (t1);
SamplerState Sampler;

struct PixelOut
{
    float4 amplitude: SV_TARGET1;
    float4 normal: SV_TARGET0;
};

float3 GetNormal(InputPixel input)
{
    const float3 worldPosition = input.worldPosition.xyz;
    const float3 center = input.center.xyz;

    const float3 dif = normalize(worldPosition - center);

    const float3 texturized = (dif + 1.f) / 2.f;
    return texturized;
}

PixelOut PS_PosSizeColor(InputPixel input)
{
    const float3 dif = input.worldPosition.xyz - input.center.xyz;
    const float dist = length(dif);
    const float interpol = saturate(dist / input.radius);
    
    float amplitude;

    const float minValue = 0.0000001;
    if(interpol < minValue)
    {
        amplitude = 1.f / minValue;
    }
    else
    {
        amplitude = 1.f / interpol;
    }
    PixelOut output = (PixelOut)0;
    output.amplitude.rgba = input.color * (1.f - 1.f / amplitude);
    const float3 normalTexturized = GetNormal(input);

    output.normal.rgba = float4(normalTexturized.xyz, 1.f - interpol);

    

    return output;
}