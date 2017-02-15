#include <structs.fx>

Texture2D renderPackage : register(t1);
Texture2D AverageColor : register(t2);

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;
//Meeeh :C

float3 Uncharted2Tonemap(float3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}


float LinearToLum(float3 color)
{
    float bloomCutoff = 0.5f;
    float3 resource = color;

    float3 relativeLuminance = (float3)0;

    relativeLuminance.r = 0.2126f;
    relativeLuminance.g = 0.7152f;
    relativeLuminance.b = 0.0722f;
    
    float luminance =   resource.r * relativeLuminance.r + 
                        resource.g * relativeLuminance.g + 
                        resource.b * relativeLuminance.b;

    luminance = saturate(luminance - bloomCutoff);
    float3 resourceLuminance = resource * luminance * (1.0f / bloomCutoff);
    float luminanceLenght = length(resourceLuminance);
    //return luminance;
    return luminanceLenght;
}

float Exposure = 0.5f;
// Tonemap
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    PixelOutput output;
    // float3 resource = renderPackage.SampleLevel(LinearSampler, input.tex, 0).rgb;
    // float3 relativeLuminance = (float3)0;
    // // relativeLuminance.r = 0.2126f;
    // // relativeLuminance.g = 0.7152f;
    // // relativeLuminance.b = 0.0722f;
    // relativeLuminance.r = 0.3f;
    // relativeLuminance.g = 0.59f;
    // relativeLuminance.b = 0.11f;
    // float luminance = resource.r * relativeLuminance.r +
    //                   resource.g * relativeLuminance.g +
    //                   resource.b * relativeLuminance.g;
    // float lumWhite = 20.f;
    // float lumDivWhite = (luminance / (lumWhite * lumWhite));
    // float lumPlusOne = luminance +1;
    // float lumDivWhitePlusOne = lumDivWhite + 1;
    // float lumTimesLumDivWhitePlusOne = luminance * lumDivWhitePlusOne;
    // float luminanceTonemapped = lumTimesLumDivWhitePlusOne / lumPlusOne;                
    // float scale = luminanceTonemapped / luminance;
    // resource = resource * scale;
    // resource = pow(resource, 1 / 2.2f);
    // output.color = float4(resource.rgb, 1.0f);
    // return output;

    float3 color = float3(0.f, 0.f, 0.f);
    float4 textuerSample = renderPackage.Sample(LinearSampler, input.tex);
    float2 uv = float2(0.5f, 0.5f);
    float average = AverageColor.Sample(PointSampler, uv).r;
    
    //Tonemap and exposure
    float pixelLuminance = LinearToLum(textuerSample.xyz);
    pixelLuminance = pixelLuminance * Exposure / average;
    color = textuerSample.xyz * (pixelLuminance / (1 + pixelLuminance));
    
    output.color.xyz = textuerSample.xyz;
    output.color.a = 1.0f;
    return output;


}