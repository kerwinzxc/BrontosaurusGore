#include <structs.fx>

Texture2D renderPackage : register(t1);
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer windowSize  : register(b1)
{
    float windowSizeX;
	float windowSizeY;

    float2 trash;
}



// average SHADER
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    float4 averageColor;
    averageColor.xyzw = 0.0f;

    for(int x = 0; x < windowSizeX; x++)
    {
        for(int y = 0; y < windowSizeY; y++)
        {
            float2 uv = float2((float)x / windowSizeX, (float)y / (float)windowSizeY);
            averageColor += renderPackage.SampleLevel(Sampler, uv, 0);
        }    
    }
    PixelOutput output;
    output.color = averageColor / ((float)windowSizeX * (float)windowSizeY);
    return output;
}
