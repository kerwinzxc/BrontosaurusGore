#include <structs.fx>
Texture2D renderPackage : register(t1);

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

static const uint kernelSize = 5;
static const float weights[kernelSize] = {0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f};
static const float texelOffset[kernelSize] = {-2.0f, -1.0f, 0.0f, 1.0f, 2.0f };
static const float texelSize = 1.0f / 256.f;
static const float scale = 1.f;


PixelOutput PS_HorizontalBlur(PosTex_InputPixel input)
{
    float3 blurColor = (float3)0;

    for(unsigned int i = 0; i < kernelSize; ++i)
    {
        float2 textureCoords = input.tex;
        textureCoords.x += texelSize * (texelOffset[i] * scale);
        float3 resource = renderPackage.SampleLevel(Sampler, textureCoords, 0).rgb;
        blurColor += resource * weights[i];
    }
    PixelOutput output;
    output.color.rgb = blurColor.rgb;
    output.color.a = 1.0f;
    return output;
}

PixelOutput PS_VerticalBlur(PosTex_InputPixel input)
{
    float3 blurColor = (float3)0;

    for(unsigned int i = 0; i < kernelSize; ++i)
    {
        float2 textureCoords = input.tex;
        textureCoords.y += texelSize * (texelOffset[i] * scale);
        float3 resource = renderPackage.SampleLevel(Sampler, textureCoords, 0).rgb;
        blurColor += resource * weights[i];
    }
    PixelOutput output;
    output.color.rgb = blurColor.rgb;
    output.color.a = 1.0f;
    return output;
}
