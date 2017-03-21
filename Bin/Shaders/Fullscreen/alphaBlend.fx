#include <structs.fx>

Texture2D renderPackage1 : register(t1);
Texture2D renderPackage2 : register(t2);

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

// COPY SHADER
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    float4 resource1 = renderPackage1.SampleLevel(Sampler, input.tex, 0).rgba;
    float resource2 = renderPackage2.SampleLevel(Sampler, input.tex, 0).a;
    
    PixelOutput output;
    output.color = float4(resource1.rgb,resource1.a * resource2);
    return output;
}
