#include <structs.fx>

Texture2D renderPackage : register(t1);
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};



// COPY SHADER
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    PixelOutput output;
    output.color = renderPackage.SampleLevel(Sampler, input.tex, 0);
    return output;
}
