#include <structs.fx>

Texture2D renderPackage : register(t1);
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_POINT;
};

float3 GetColor(float2 uv)
{
    return renderPackage.SampleLevel(Sampler, uv, 0).rgb;
}


#define FXAA_SPAN_MAX 8.0
#define FXAA_REDUCE_MUL   (1.0/FXAA_SPAN_MAX)
#define FXAA_REDUCE_MIN   (1.0/128.0)

float3 Blur(PosTex_InputPixel input, const float3 centerColor)
{
    //const float2 sampleRadius = float2(0.005,0.005); // Activate for awesomeness!
    const float2 sampleRadius = float2(0.00015,0.00015);
    const float2 uv = input.tex;
    
    const float3 nw = GetColor(uv - sampleRadius);
    const float3 ne = GetColor(uv + (float2(sampleRadius.x, -sampleRadius.y)));
    const float3 se = GetColor(uv + sampleRadius);
    const float3 sw = GetColor(uv + (float2(-sampleRadius.x, sampleRadius.y)));

    const float3 luma = float3(0.299, 0.587, 0.114);

    const float lumaNw = dot(nw, luma);
    const float lumaNe = dot(ne, luma);
    const float lumaSe = dot(se, luma);
    const float lumaSw = dot(sw, luma);
    const float lumaC = dot(centerColor, luma);

    const float lumaMin = min(lumaC, min(min(lumaNw,lumaNe),min(lumaSe, lumaSw)));
    const float lumaMax = max(lumaC, max(max(lumaNw,lumaNe),max(lumaSe, lumaSw)));

    const float dirX = -((lumaNw + lumaNe)-(lumaSw + lumaSe));
    const float dirY = ((lumaNw + lumaSw)-(lumaNe + lumaSe));
    float2 dir = float2(dirX, dirY);

    const float dirReduce = max(
        (lumaNw + lumaNe + lumaSe + lumaSw) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN
    );
    const float rcpDirMin = 1.f / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
    max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
    dir * rcpDirMin)) * sampleRadius;

    const float3 mixA = (1.f / 2.f) * (
        GetColor(uv + dir * (1.f / 3.f - 0.5f)) + GetColor(uv + dir * (2.f / 3.f - 0.5f))
    );
    const float3 mixB = mixA * (1.f / 2.f) + (1.f / 4.f) * (
        GetColor(uv + dir * (0.f / 3.f - 0.5f)) + GetColor(uv + dir * (3.f / 3.f - 0.5f))
    );

    const float lumaB = dot(mixB, luma);

    float3 returnValue = mixB;
    if((lumaB < lumaMin) || (lumaB > lumaMax))
    {
        returnValue =  mixA;
    }

    return returnValue;
}

// COPY SHADER
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    PixelOutput output;
    output.color = renderPackage.SampleLevel(Sampler, input.tex, 0);
    output.color.rgb = Blur(input, output.color.rgb);
    return output;
}
