#include <structs.fx>

Texture2D depthPackage : register(t1);
Texture2D depthPackage2 : register(t2);

//**********************************************//
//					SAMPLERS					//
//**********************************************//

SamplerState samplerClamp : register(s0);
SamplerState samplerWrap : register(s1);
SamplerState samplerPointWrap : register(s2);

// COPY SHADER
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    PixelOutput output;

	output.color = depthPackage2.SampleLevel(samplerPointWrap, input.tex, 0);
	
	if (output.color.r == 1.0f)
		output.color = depthPackage.SampleLevel(samplerPointWrap, input.tex, 0);

    return output;
}
