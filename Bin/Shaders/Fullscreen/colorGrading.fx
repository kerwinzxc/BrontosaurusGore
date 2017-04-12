#include <structs.fx>

Texture2D renderPackage : register(t1);
Texture3D LutTexture	: register(t2);
Texture3D LutTexture2	: register(t3);


//**********************************************//
//					SAMPLERS					//
//**********************************************//

SamplerState SamplerClamp : register(s0);
SamplerState samplerWrap : register(s1);



//**********************************************//
//					BUFFERS						//
//**********************************************//

cbuffer lutLerp : register(b2)
{
	float4 lerpT; // time in x
}


// COkie shader SHADER
float3 PixelToTexelSpace(float3 input)
{
	const float numberOfhalfPixels = 16.0f * 2.0f;
	const float halfPixelSize = 1.0f / numberOfhalfPixels;
	input /= numberOfhalfPixels;
	input *= (numberOfhalfPixels - 2.0f);
	input += halfPixelSize.xxx;
	return input;
}

PixelOutput PS_PosTex(PosTex_InputPixel input)
{
	PixelOutput output;
	float3 rgbPosition = renderPackage.SampleLevel(SamplerClamp, input.tex, 0).rgb;
	float3 uvw = PixelToTexelSpace(rgbPosition);

	float4 color = LutTexture.SampleLevel(SamplerClamp, uvw, 0);
	float4 color2 = LutTexture2.SampleLevel(SamplerClamp, uvw, 0);
	output.color = lerp(color, color2, saturate(lerpT.x));
	return output;
}
