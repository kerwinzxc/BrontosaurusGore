#include <../shader_structs.fx>


//**********************************************//
//					TEXTURES					//
//**********************************************//

Texture2D diffuse               : register(t1);
Texture2D RMAO					: register(t3);
Texture2D emissive				: register(t4);
Texture2D normalMap				: register(t6);

//**********************************************//
//					SAMPLERS					//
//**********************************************//

SamplerState SamplerClamp		: register(s0);
SamplerState samplerWrap		: register(s1);

//**********************************************//
//					C-BUFFER					//
//**********************************************//

cbuffer ConstantBuffer			: register(b0) 
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;
}
cbuffer WorldSpaceBuffer : register(b1)
{
	float4x4 worldSpace;
	float4x4 lastWorldSpace;
	float4 highlightColor;
}

//**********************************************//
//					G-BUFFER					//
//**********************************************//

struct PixelOutput
{
	float4 diffuse              : SV_TARGET0;
	float4 normal				: SV_TARGET1;
	float4 RMAO					: SV_TARGET2;
	float4 emissive				: SV_TARGET3;
	float4 HighLight			: SV_TARGET4;
};

//**********************************************//
//					FRAGMENT					//
//**********************************************//

float3 PS_ObjectNormal(PosNormBinormTanTex_InputPixel input);
float4 GetHighlight(PosNormBinormTanTex_InputPixel input, float3 normal);
float3 NormalFromTexture(float3 normal);

PixelOutput PS_PosNormBinormTanTex(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;
	output.diffuse = diffuse.Sample(samplerWrap, input.uv);
	if (output.diffuse.a <= ALPHA_THRESHOLD)
	{
		discard;
	}

	output.normal = float4(PS_ObjectNormal(input), 1.0f);
	output.RMAO = RMAO.Sample(samplerWrap, input.uv);
	output.emissive = float4(emissive.Sample(samplerWrap, input.uv).xyz,1.f);
	output.HighLight = GetHighlight(input, output.normal.xyz);
	return output;
}


PixelOutput PS_PosNormBinormTanTexBones(PosNormBinormTanTex_InputPixel input)
{
	return PS_PosNormBinormTanTex(input);
}

PixelOutput PS_PosNormBinormTanTexInstanced(PosNormBinormTanTex_InputPixel input)
{
	return PS_PosNormBinormTanTex(input);

}

float3 PS_ObjectNormal(PosNormBinormTanTex_InputPixel input)
{
	float3 normal = normalMap.Sample(samplerWrap, input.uv).xyz;
	normal = NormalFromTexture(normal);
	normal = normalize(normal);
	float3x3 tangentSpaceMatrix = float3x3(normalize(input.biTangent.xyz), normalize(input.tangent.xyz), normalize(input.normals.xyz));
	normal = mul(normal, tangentSpaceMatrix);
	// Make the normal 0.0 - 1.0
	normal += float3(1.0f, 1.f, 1.f);
	normal *= 0.5f;

	float3 output;
	output.xyz = normal.xyz;
	return output;
}

float4 GetHighlight(PosNormBinormTanTex_InputPixel input, float3 normal)
{

	const float3 cameraDirWorld = mul(float3(0,0,-1),(float3x3)cameraSpaceInversed);

	const float fresnelValue = abs(dot(NormalFromTexture(normal),cameraDirWorld));

	const float lol = fresnelValue / highlightColor.a;
	const float alphaValue = saturate(1 - lol * lol * lol);

	
	const float3 HighLight = highlightColor.rgb;

	// if(fresnelValue < 0.25)
	// {
	// 	HighLight.a = 1.f;
	// }

	return float4(HighLight, alphaValue * highlightColor.a);
}

float3 NormalFromTexture(float3 normal)
{
	return (normal * 2.0f) - float3(1.0f, 1.f, 1.f);
}
