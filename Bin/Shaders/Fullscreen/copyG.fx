#include <structs.fx>

Texture2D renderPackage : register(t1);
SamplerState Sampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};



// COPY G SHADER
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
	PixelOutput output;
	output.color = renderPackage.SampleLevel(Sampler, input.tex, 0).gggg;
	output.color.a = 1.0f;

	return output;
}
