#include <..\Fullscreen\structs.fx>

//**********************************************//
//					TEXTURES					//
//**********************************************//

TextureCube cubeMap             : register(t0);
Texture2D deferred_diffuse      : register(t1);
Texture2D deferred_normal       : register(t2);
Texture2D deferred_RMAO			: register(t3);
Texture2D deferred_emissive     : register(t4);
Texture2D deferred_depth        : register(t5);

//**********************************************//
//					SAMPLERS					//
//**********************************************//

SamplerState SamplerClamp       : register(s0);
SamplerState samplerWrap        : register(s1);

//**********************************************//
//					C-BUFFER					//
//**********************************************//

cbuffer ConstantBuffer          : register(b0)
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;

	float4x4 shadowCamInverse;
	float4x4 shadowCamProjection;

	float garbageo[4];
}

cbuffer ProjectionInverse      : register(b1)
{
	float4x4 projectionInverse;
	float4x4 cameraSpace;
}


//**********************************************//
//					G-BUFFER					//
//**********************************************//

struct Output
{
	float4 color                : SV_TARGET0;
};

//**********************************************//
//					FRAGMENT					//
//**********************************************//

float RoughToSPow(float fRoughness)
{
	return (2.0f / (fRoughness * fRoughness)) - 2.0f;
}

static const float k0 = 0.00098f;
static const float k1 = 0.9921f;
static const float fakeLysMaxSpecularPower = (2.0f / (0.0014f * 0.0014f)) - 2.0f;
static const float fMaxT = (exp2(-10.f / sqrt(fakeLysMaxSpecularPower)) - k0) / k1;
float GetSpecPowToMip(float fSpecPow, int nMips)
{
	float fSmulMaxT = (exp2(-10.0f / sqrt(fSpecPow)) - k0) / k1;

	return float(nMips - 1 - 0) * (1.0f - clamp(fSmulMaxT / fMaxT, 0.0f, 1.0f));
}

Output PS_PosTex(PosTex_InputPixel inputPixel)
{
	Output output;
	float2 uv = inputPixel.tex;

	float1 depth = deferred_depth.Sample(samplerWrap, uv).x;
	if (depth >= DEPTH_BIAS)
	{
		output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return output;
	}


	float3 albedo = deferred_diffuse.Sample(samplerWrap, uv).xyz;
	float3 normal = deferred_normal.Sample(samplerWrap, uv).xyz;
	normal = (normal * 2.0f);
	normal -= float3(1.0f, 1.0f, 1.0f);
	//normal = -normal;
	//normal = mul(float4(normal, 1.0f), cameraSpaceInversed).xyz;

	//float3 emissive = deferred_emissive.Sample(samplerWrap, uv).xyz;									 // eget pass?
	float4 RMAO = deferred_RMAO.Sample(samplerWrap, uv);
	//float3 ambientLight = cubeMap.SampleLevel(samplerWrap, normal, 0).xyz;
	float3 ambientLight = cubeMap.SampleLevel(samplerWrap, normal.xyz, (uint) 11 - 2).xyz; // FIX!
	//ambientLight = float3(1.0f, 0.0f, 0.0f);


	float3 metalness = RMAO.yyy;
	//metalness *= -1;
	//metalness += 1.0f;

	float3 metalnessAlbedo = albedo - (albedo * metalness);
	float3 ambientOcclusion = RMAO.zzz;
	float1 roughness = RMAO.x;
	//roughness = 0.01f;
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo * metalness;

	float4 worldPos = (float4) 0;
	//worldPos.xy = uv;
	worldPos.xy = (uv * 2.0f);
	worldPos.xy -= float2(1.0f, 1.0f); // transforms to -1 to 1
	worldPos.y *= -1.0f;
	worldPos.z = depth;
	worldPos.w = 1.0f;

	worldPos = mul(projectionInverse, worldPos);
	worldPos = mul(cameraSpace, worldPos);
	worldPos = worldPos / worldPos.w;


	float4 cameraPos = (float4)0;
	cameraPos.w = 1.0f;
	cameraPos = mul(cameraSpace, cameraPos);
	cameraPos = cameraPos / cameraPos.w;

	//cameraPos = -cameraPos;
	//cameraPos = mul(cameraSpaceInversed, cameraPos);

	float3 toEye = normalize(cameraPos.xyz - worldPos.xyz);
	float1 VdotN = dot(toEye.xyz, normal);
	VdotN = saturate(VdotN);
	VdotN = 1.0f - VdotN;
	VdotN = pow(VdotN, 5); //whaah
	float3 fresnel = VdotN * (1.f - substance);
	fresnel = fresnel / (6 - 5 * roughness);
	fresnel = substance + fresnel;
	float3 ambientDiffuse = metalnessAlbedo * ambientLight * ambientOcclusion * (float3(1.0f, 1.0f, 1.0f) - fresnel);

	//AMBIENT SPEC
	float3 reflectionVector = -reflect(toEye, normal);
	float fakeLysSpecularPower = RoughToSPow(roughness);
	float lysMipMap = GetSpecPowToMip(fakeLysSpecularPower, (uint) 11);
	float3 ambientLightSpec = cubeMap.SampleLevel(samplerWrap, reflectionVector.xyz, lysMipMap).xyz;
	float3 ambientSpecularity = ambientLightSpec * ambientOcclusion * fresnel;

	output.color = float4(ambientDiffuse + ambientSpecularity, 1.0f);

	
	return output;
}