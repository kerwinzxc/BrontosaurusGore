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

	float4x4 shadowCamInverse;
	float4x4 shadowCamProjection;

	float garbageo[4];
}

//**********************************************//
//					G-BUFFER					//
//**********************************************//

struct PixelOutput
{
	float4 diffuse              : SV_TARGET0;
	float4 normal				: SV_TARGET1;
	float4 RMAO					: SV_TARGET2;
	float3 emissive				: SV_TARGET3;
};

//**********************************************//
//					FRAGMENT					//
//**********************************************//

float3 PS_ObjectNormal(PosNormBinormTanTex_InputPixel input);

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
	output.emissive = emissive.Sample(samplerWrap, input.uv).xyz;
	return output;
}


PixelOutput PS_PosNormBinormTanTexBones(PosNormBinormTanTex_InputPixel input)
{
	return PS_PosNormBinormTanTex(input);
}

float3 PS_ObjectNormal(PosNormBinormTanTex_InputPixel input)
{
	float3 normal = normalMap.Sample(samplerWrap, input.uv).xyz;
	normal = (normal * 2.0f) - float3(1.0f, 1.f, 1.f);
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

