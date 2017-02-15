struct VertexInputPos
{
	float4 position : POSITION;
};

struct VertexInputTex
{
	float2 uv : UV;
	float2 scale : SCALE;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv : TEX_COORD;
};

cbuffer VertexConstantBuffer : register(b2)
{
	float2 position;
	float size;
	float garbage;
}

cbuffer PixelConstantBuffer : register(b2)
{
	float4 color;
}

Texture2D textAtlas : register(t1);
SamplerState globalSamplerState;


PixelInput VS_Text(VertexInputPos inputPos, VertexInputTex inputTex)
{
	float2 outPosition;
	outPosition = inputPos.position.xy * inputTex.scale;

	outPosition.x = outPosition.x;
	outPosition.y = outPosition.y + 1.f - inputTex.scale.y;

	outPosition += float2(position.x, -position.y);

	outPosition *= size.xx;

	//0 - 1 to -1 - 1
	outPosition = outPosition * 2.f;
	outPosition = outPosition - float2(1.f, 1.f);

	PixelInput output;
	output.position = float4(outPosition, 0.5f, 1.f);

	output.uv = inputTex.uv;
	
	return output;
}

//static const uint KernelSize = 25;
//static const float2 surroundingPixels[KernelSize] =
//{
//	float2(-2, -2), float2(-1, -2), float2(0, -2), float2(1, -2), float2(2, -2),
//	float2(-2, -1), float2(-1, -1), float2(0, -1), float2(1, -1), float2(2, -1),
//	float2(-2, 0), float2(-1, 0), float2(0, 0), float2(1, 0), float2(2, 0),
//	float2(-2, 1), float2(-1, 1), float2(0, 1), float2(1, 1), float2(2, 1),
//	float2(-2, 2), float2(-1, 2), float2(0, 2), float2(1, 2), float2(2, 2)
//};

float4 PS_Text(PixelInput input) : SV_TARGET
{
	float4 output = textAtlas.Sample(globalSamplerState, input.uv);
	float averageAlpha = output.a;
	//float averageAlpha = 0.f;
	//for (uint i = 0; i < KernelSize; ++i)
	//{
	//	averageAlpha += textAtlas.Sample(globalSamplerState, input.uv + surroundingPixels[i]).a;
	//}

	//averageAlpha /= KernelSize;

	output.rgb *= averageAlpha;
	output.rgba *= color;

	return output;
}