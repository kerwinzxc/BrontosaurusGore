#include <structs.fx>

Texture2D renderPackage : register(t1);
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};



// Luminance SHADER
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    float bloomCutoff = 0.65f;
    float3 resource = renderPackage.SampleLevel(Sampler, input.tex, 0).rgb;

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
    
    float3 luminanceo =  float3(resource.r * relativeLuminance.r, resource.g * relativeLuminance.g, resource.b * relativeLuminance.b);
    luminanceo = saturate(luminanceo - bloomCutoff.xxx);
    float3 resourceLuminanceo = resource * luminanceo * (1.0f / bloomCutoff);    
    float luminanceLenghto = length(resourceLuminanceo);

    PixelOutput output;
    output.color.rgb = luminanceLenght.rrr;
    output.color.a = 1.0f;    
    return output;
}
