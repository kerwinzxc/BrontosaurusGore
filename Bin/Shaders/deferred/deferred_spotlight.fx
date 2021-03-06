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

Texture2D shadowBuffer          : register(t6);

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


// l�gg till i ^
cbuffer CameraBuffer			: register(b1)
{
	float4x4 projectionInverse;
	float4x4 cameraSpace;
}

cbuffer SpotLightBuffer	        : register(b2)
{
	struct SpotLight
	{
		float3 color;
		float intensity;

		float3 position;
		float range;

		float3 direction;
		float shadowCasting;

		float theta ;// inner cone
		float phi;	// outer cone


		float garbage[2];
	} spotLight;
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


float3 Lambert(float3 aNormal, float3 aDirection)
{
	return saturate(dot(aNormal, -aDirection));
}


float Distribution(float3 lightDirection, float3 normal, float roughness, float3 cameraPosition, float3 worldPosition)
{
	float3 toLight = -lightDirection;

	float3 toEye = normalize(cameraPosition - worldPosition);
	float3 halfvec = normalize(toLight + toEye.xyz);
	float HdotN = saturate(dot(halfvec, normal));

	float m = roughness * roughness; // roughness ^2 or what?
	float m2 = m * m; //roughness ^4 or what?

	float denominator = HdotN * HdotN * (m2 - 1.0f) + 1.0f;
	float distribution = m2 / (3.1415926538f * denominator * denominator);

	return distribution;
}

	//mebe only able to run PBL if we're using PosNormBinormTanTex.
float Visibility(float3 lightDirection, float3 normal, float roughness, float3 cameraPosition, float3 worldPosition)
{
	float3 toLight = -lightDirection;

	float roughnessRemapped = (roughness + 1.0f) / 2.0f;
	float3 toEye = normalize(cameraPosition - worldPosition);

	float NdotL = saturate(dot(normal, toLight));
	float NdotV = saturate(dot(normal, toEye));

	float k = roughnessRemapped * roughnessRemapped * 1.7724f; //sqrt(PI) wth is K?
	float G1V = NdotV * (1.0f - k) + k;
	float G1L = NdotL * (1.0f - k) + k;
	float visibility = (NdotV * NdotL) / (G1V * G1L);

	return visibility;
}

float3 Normal(float2 uv)
{
	float3 normal = deferred_normal.Sample(samplerWrap, uv).xyz;
	normal = (normal * 2.0f);
	normal -= float3(1.0f, 1.0f, 1.0f);
	return normal;
}


float3 WorldPosition(float2 uv, float depth)
{
	float4 worldPosition = (float4) 0;
	worldPosition.xy = (uv * 2.0f);
	worldPosition.xy -= float2(1.0f, 1.0f); // transforms to -1 to 1
	worldPosition.y *= -1.0f; // flip y for wth!
	worldPosition.z = depth;
	worldPosition.w = 1.0f;
	worldPosition = mul(projectionInverse, worldPosition);
	worldPosition = mul(cameraSpace, worldPosition);
	worldPosition = worldPosition / worldPosition.w;
	return worldPosition.xyz;
}

float3 CameraPosition()
{
	float4 cameraPosition = (float4) 0;
	cameraPosition.w = 1.0f;
	cameraPosition = mul(cameraSpace, cameraPosition);
	cameraPosition = cameraPosition / cameraPosition.w;
	return cameraPosition.xyz;
}

Output PS_PosTex(PosTex_InputPixel inputPixel)
{
	Output output;
	
	float2 uv = inputPixel.tex;
	float1 depth = deferred_depth.Sample(samplerWrap, uv).x;
	
	float4 fullAlbedo = deferred_diffuse.Sample(samplerWrap, uv).rgba;

	// if (depth >= DEPTH_BIAS)
	// {
	// 	output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// 	return output;
	// }


	float3 albedo = fullAlbedo.rgb;
	float3 normal = Normal(uv);

	float4 RMAO = deferred_RMAO.Sample(samplerWrap, uv);
	float3 metalness = RMAO.yyy;

	float3 metalnessAlbedo = albedo - (albedo * metalness);
	float3 ambientOcclusion = RMAO.zzz;
	float1 roughness = RMAO.x;
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo * metalness;

	float3 worldPosition = WorldPosition(uv, depth);
	float3 cameraPosition = CameraPosition();

	float3 toEye = normalize(cameraPosition - worldPosition);
	float1 VdotN = dot(toEye.xyz, normal);
	VdotN = saturate(VdotN);
	VdotN = 1.0f - VdotN;
	VdotN = pow(VdotN, 5); //whaah
	float3 fresnel = VdotN * (1.f - substance);
	fresnel = fresnel / (6 - 5 * roughness);
	fresnel = substance + fresnel;


	float3 difference = worldPosition - spotLight.position;

	float lightRange = length(difference) / spotLight.range;
	lightRange = saturate(lightRange);
	lightRange *= lightRange;
	lightRange = 1.0f - lightRange;
	// DIF
	float3 lightColor = spotLight.color;
	float3 lightDirection = normalize(difference);

	float3 lambert = Lambert(normal, lightDirection);
	float3 toLight = -lightDirection;
	float3 halfvec = normalize(toLight + toEye);
	float LdotH = dot(toLight, halfvec);
	LdotH = saturate(LdotH);
	LdotH = 1.0f - LdotH;
	LdotH = pow(LdotH, 5);
	float3 dirrfresnel = LdotH * (1.f - substance);
	dirrfresnel = substance + dirrfresnel;
	float3 one = float3(1.0f, 1.0f, 1.0f);
	float3 directionDiffuse = metalnessAlbedo * lightColor * lambert * (one - dirrfresnel);



	// SPEC hugger
	float3 distribution = Distribution(lightDirection, normal, roughness, cameraPosition, worldPosition).xxx;
	float3 visibility = Visibility(lightDirection, normal, roughness, cameraPosition, worldPosition).xxx;
	float3 directionSpecularity = lightColor * lambert * dirrfresnel * distribution * visibility;



	float3 finalColor = directionDiffuse + directionSpecularity;
	//float3 finalColor = directionDiffuse + directionSpecularity;


	float spotPower = 2.0f;
	float3 L = normalize(spotLight.position - worldPosition);
	float spotScale = pow(max(dot(normalize(difference), -lightDirection), 0), spotPower);

	float angle = acos(dot(spotLight.direction, lightDirection));

	if (angle > spotLight.phi)
	{
		finalColor = float3(0.0, 0.0, 0.0);
	}


	finalColor *= spotLight.intensity * lightRange * spotScale;



	output.color = float4(finalColor, fullAlbedo.a);

	return output;
}