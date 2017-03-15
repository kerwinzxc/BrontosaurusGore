#include "nurbsCommon.fx"

Texture2D sprite : register (t1);
SamplerState Sampler;

struct PixelOut
{
    float4 diffuse: SV_TARGET1;
    float4 normal: SV_TARGET0;
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
    return texturized;
}

PixelOut PS_PosSizeColor(const InputPixel input)
{
    const float2 dif = input.worldPosition.xy - input.center.xy;
    const float dist = length(dif);
    const float interpol = saturate(dist / input.radius);

    PixelOut output = (PixelOut)0;
    
    output.diffuse.rgba = float4(1,0,0,1);
    
    output.normal = float4(GetNormal(input),1);
    if(interpol >= 1)
    {
        output.diffuse.a = 0;
        output.normal.a = 0;
    }


    return output;
}