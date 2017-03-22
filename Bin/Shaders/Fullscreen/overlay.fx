#include <structs.fx>

Texture2D renderPackage1 : register(t1);
Texture2D renderPackage2 : register(t2);

cbuffer Fullscreenbuffer : register(b5)
{
    float alpha;
    
}

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
    float4 resource2 = renderPackage2.SampleLevel(Sampler, input.tex, 0).rgba;
    

    PixelOutput output;
    output.color = float4(resource1.rgba + float4(resource2.rgb * resource2.a, resource2.a));
    output.color.a *= alpha;
    return output;
}
