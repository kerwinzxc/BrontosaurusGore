#include "metaballsCommon.fx"

Texture2D sprite : register (t1);
Texture2D weaponDepth : register(t20);
SamplerState Sampler;

struct PixelOut
{
    float4 diffuse: SV_TARGET0;
    float4 normal: SV_TARGET1;
    float4 ao: SV_TARGET2;
    float4 alpha: SV_TARGET3;
};

float GetZ(const InputPixel input, float difLength)
{
    
    float output = 0;

    const float radius2 = input.radius * input.radius;
    const float diffLength2 = difLength * difLength;
    const float z2 = radius2 - diffLength2;
    
    if(difLength == 0)
    {
        output = input.radius;
    }
    else
    {
        output = sqrt(abs(z2));
    }

    return output;
}

float3 GetNormal(const InputPixel input)
{
    const float3 worldPosition = input.worldPosition.xyz;
    const float3 center = input.center.xyz;

    const float3 dif = worldPosition - center;

    const float z = GetZ(input, length(dif));
    const float3 texturized = (normalize(float3(dif.xy, z)) + 1.f) / 2.f;
    return normalize(texturized);
}

void Discard(InputPixel input)
{
    const float depth = input.screenPos.z;
    const float2 screenUv = (input.screenPos.xy + float2(1.f,1.f)) / 2.f;
    const float wDepth = weaponDepth.SampleLevel(Sampler, float2(screenUv.x, 1.f - screenUv.y),0).r;

    if(wDepth != 1.f)
    {
        discard;
    }
}

PixelOut PS_PosSizeColor(InputPixel input)
{
    Discard(input);
    const float3 dif = input.worldPosition.xyz - input.center.xyz;
    const float dist = length(dif);
    const float interpol = saturate((dist) / (input.radius));
    
    const float f = (1 - interpol * interpol);
    const float f2 = f * f;
    
    PixelOut output = (PixelOut)0;
    output.diffuse.rgba = float4(input.color.rgb, f2);
    const float3 normalTexturized = GetNormal(input);

    output.normal.rgba = float4(normalTexturized.xyz, f2);

    output.ao.rgba  = float4(0.,0,1,1.f);
    output.alpha.a = input.color.a;
    output.alpha.r = 1.f;
    

    return output;
}