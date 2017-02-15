#include <structs.fx>

Texture2D renderPackage : register(t1);
Texture2D velocityMap : register(t2);

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    PixelOutput output;

    const float numberOfSamples = 10.f;
    const float blurSize = 0.05f;

    float3 velocity = velocityMap.SampleLevel(LinearSampler, input.tex, 0).rgb;
    //if velocity is 0, break.
    velocity  /= 100.0f;
    velocity *= blurSize;
    velocity /= numberOfSamples;

    float2 textureSample = input.tex;
    float3 finalColor = {0.f, 0.f, 0.f};

    for(int i = 0; i < numberOfSamples; ++i)
    {
        float3 currentColor = renderPackage.SampleLevel(LinearSampler, textureSample, 0).rgb;
        finalColor += currentColor;
        textureSample += velocity.rg;
    }

    finalColor /= numberOfSamples;

    output.color.xyz = finalColor.xyz;
    output.color.a = 1.0f;
    return output;


}