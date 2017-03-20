#include <../Fullscreen/structs.fx>

Texture2D diffuse : register(t1);
Texture2D normal : register(t2);
Texture2D rmao : register(t3);

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct SurfacePixelOutput
{
    float4 diffuse : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 rmao : SV_TARGET2;
};

// COPY SHADER
SurfacePixelOutput PS_PosTex(PosTex_InputPixel input)
{
    SurfacePixelOutput output;
    
    output.diffuse = diffuse.SampleLevel(Sampler, input.tex, 0);
    output.normal = normal.SampleLevel(Sampler, input.tex, 0);
    output.rmao = rmao.SampleLevel(Sampler, input.tex, 0);
    const float threshold = 0.5;
    if(output.diffuse.a > threshold)
    {
        output.diffuse.a = 1.f;
         output.normal.a = 1.f;
          output.rmao.a = 1.f;
    }
    else
    {
        output.diffuse.rgba = float4(0,0,0,0);
        output.normal.rgba = float4(0,0,0,0);
        output.rmao.rgba = float4(0,0,0,0);
    }

    output.normal.rgb = normalize(output.normal.rgb);

    return output;
}
