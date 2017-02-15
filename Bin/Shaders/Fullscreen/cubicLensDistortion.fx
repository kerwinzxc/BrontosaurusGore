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
    float2 tex = input.tex;

    float k = -0.05f;

    // cubic distortion value
    float kcube = 0.05f;


    float r2 = (tex.x - 0.5f) * (tex.x - 0.5f) + (tex.y - 0.5f) * (tex.y - 0.5f);
    float f = 0;


    //only compute the cubic distortion if necessary
    if( kcube == 0.0)
    {
        f = 1 + r2 * k;
    }
    else
    {
        f = 1 + r2 * (k + kcube * sqrt(r2));
    };

    // get the right pixel for the current position
    float x = f*(tex.x-0.5)+0.5;
    float y = f*(tex.y-0.5)+0.5;
    float inputDistord =  renderPackage.SampleLevel(Sampler, float2(x, y), 0).g;
    float3 inputColor =  renderPackage.SampleLevel(Sampler, tex, 0).rgb;

    output.color = float4(inputColor.r, inputDistord, inputColor.b, 1.0f);
    return output;
}
