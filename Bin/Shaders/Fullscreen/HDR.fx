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
    //float L = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    //float nL = Uncharted2Tonemap(L);
    //float scale = nL / L;
    ////color *= scale;
    //return scale;
    float r = color.x;
    float g = color.y;
    float b = color.z;
    float maxRGB = 0.f;
    float minRGB = 0.f;
    if (r >= g) { maxRGB = r ; }
    if (r >= b) { maxRGB = r ; }
    if (g >= r) { maxRGB = g ; }
    if (g >= b) { maxRGB = g ; }
    if (b >= r) { maxRGB = b ; }
    if (b >= g) { maxRGB = b ; }

    if (r <= g) { minRGB = r ; }
    if (r <= b) { minRGB = r ; }
    if (g <= r) { minRGB = g ; }
    if (g <= b) { minRGB = g ; }
    if (b <= r) { minRGB = b ; }
    if (b <= g) { minRGB = b ; }    
    float mumin = ((maxRGB + minRGB)/2);
    return mumin;
}



// HDR SHADER
float Exposure = 0.1f;
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    float3 color = float3(0.f, 0.f, 0.f);
    float4 textuerSample = renderPackage.Sample(LinearSampler, input.tex);
    float2 uv = float2(0.5f, 0.5f);
    float average = AverageColor.Sample(PointSampler, uv).r;
    
    //Tonemap and exposure
    float pixelLuminance = LinearToLum(textuerSample.xyz);
    pixelLuminance = pixelLuminance * Exposure / average;
    color = textuerSample.xyz * (pixelLuminance / (1 + pixelLuminance));
    
    PixelOutput output;
    output.color.xyz = textuerSample.xyz;
    output.color.a = 1.0f;
    return output;
}