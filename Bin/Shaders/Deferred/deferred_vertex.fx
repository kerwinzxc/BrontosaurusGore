#include <../shader_structs.fx>

cbuffer ConstantBuffer : register( b0 )
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;
	float4 highlightColor;
}

cbuffer WorldSpaceBuffer : register(b1)
{
	float4x4 worldSpace;
}

LightModel_InputPixel VS_PosNormBinormTanTex(PosNormBinormTanTex_InputVertex input)
{
	LightModel_InputPixel output;

	output.position = input.position;
	output.position = mul(worldSpace, output.position);
	output.position = mul(cameraSpaceInversed, output.position);
	output.position = mul(projectionSpace ,output.position);

	output.uv = float4((float2(output.position.x + output.position.w, output.position.w - output.position.y)) / 2, output.position.zw);
	return output;
}
