#include "particleCommon.fx"

Texture2D sprite : register (t1);
SamplerState Sampler;

float4 PS_PosSizeColor(InputPixel input) : SV_TARGET
{
    float4 color = sprite.Sample(Sampler, input.uv);
    float3 dif = input.position.xyz - input.center.xyz;
    float dist = length(dif);
    float interpol = saturate(dist / input.radius);

    input.color.rgba *= (1 - interpol * 0.5);
    return  input.color;
}