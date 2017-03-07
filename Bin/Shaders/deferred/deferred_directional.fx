#include <..\Fullscreen\structs.fx>
#include <..\oncePerFrame.fx>


//**********************************************//
//					TEXTURES					//
//**********************************************//

TextureCube cubeMap             : register(t0);
Texture2D deferred_diffuse      : register(t1);
Texture2D deferred_normal       : register(t2);
Texture2D deferred_RMAO			: register(t3);
Texture2D deferred_emissive     : register(t4);
Texture2D deferred_depth        : register(t5);

Texture2D shadowBuffer          : register(t8);

//**********************************************//
//					SAMPLERS					//
//**********************************************//

SamplerState SamplerClamp       : register(s0);
SamplerState samplerWrap        : register(s1);

//**********************************************//
//					C-BUFFER					//
//**********************************************//

// OncePer Framebuffer defined in oncePerFrame.fx

// Add to once per frame buffer mebe?
cbuffer CameraBuffer			: register(b1)
{
	float4x4 projectionInverse;
	float4x4 cameraSpace;
}

cbuffer directionalLight        : register(b2)
{
	struct DirectionaLight
	{
		float3 color;
		float intensity;
		float3 direction;
		int shadowIndex;
	} directionalLight;
}



//**********************************************//
//					G-BUFFER					//
//**********************************************//

struct Output
{
	float4 color				: SV_TARGET0;
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
	float3 halfvec = normalize(toLight + toEye);
	float HdotN = saturate(dot(halfvec, normalize(normal)));

	float m = roughness * roughness; // roughness ^2 or what?
	float m2 = m * m; //roughness ^4 or what?

	float denominator = HdotN * HdotN * (m2 - 1.0f) + 1.0f;
	float distribution = m2 / (3.1415926538f * denominator * denominator);

	return distribution;
}

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

float3 CameraPosition(float4x4 aCameraSpace)
{
	float4 cameraPosition = (float4) 0;
	cameraPosition.w = 1.0f;
	cameraPosition = mul(aCameraSpace, cameraPosition);
	cameraPosition = cameraPosition / cameraPosition.w;
	return cameraPosition.xyz;
}

float ShadowBuffer(float3 worldPosition)
{
	float output = 1.0f;
	float4 shadowCamPosition = float4(worldPosition, 1.0f);

	shadowCamPosition = mul(shadowCamInverse, shadowCamPosition);
	shadowCamPosition = mul(shadowCamProjection, shadowCamPosition);
	shadowCamPosition /= shadowCamPosition.w;

	float2 texCord;
	texCord.x = shadowCamPosition.x * 0.5f + 0.5f;
	texCord.y = shadowCamPosition.y * -0.5f + 0.5f;
	float shadowCamDepth = shadowBuffer.Sample(samplerWrap, texCord).x;

	if (shadowCamDepth < shadowCamPosition.z - 0.001f && shadowCamDepth != 0.f)
	{
		output = 0.0f;
	}
	if (directionalLight.shadowIndex == -1)
	{
		output = 1.0f;
	}
	if (texCord.x < 0.0f || texCord.x > 1.0f)
	{
		output = 1.0f;
	}
	if (texCord.y < 0.0f || texCord.y > 1.0f)
	{
		output = 1.0f;
	}
	return output;
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
	float3 worldPosition = WorldPosition(uv, depth);
	float3 cameraPosition = CameraPosition(cameraSpace);

	float3 albedo = deferred_diffuse.Sample(samplerWrap, uv).xyz;
	float3 normal = Normal(uv);

	float4 RMAO = deferred_RMAO.Sample(samplerWrap, uv);
	float3 metalness = RMAO.yyy;

	float3 metalnessAlbedo = albedo - (albedo * metalness);
	float3 ambientOcclusion = RMAO.zzz;
	float1 roughness = RMAO.x;
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo * metalness;

	float3 toEye = normalize(cameraPosition - worldPosition);
	float1 VdotN = dot(toEye.xyz, normal);
	VdotN = saturate(VdotN);
	VdotN = 1.0f - VdotN;
	VdotN = pow(VdotN, 5); //whaah
	float3 fresnel = VdotN * (1.f - substance);
	fresnel = fresnel / (6 - 5 * roughness);
	fresnel = substance + fresnel;

	// DIF
	float3 lightColor = directionalLight.color;
	float3 lambert = Lambert(normal, directionalLight.direction);
	float3 toLight = -directionalLight.direction;
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
	lightColor = directionalLight.color;
	float3 distribution = Distribution(directionalLight.direction, normal, roughness, cameraPosition, worldPosition).xxx;
	float3 visibility = Visibility(directionalLight.direction, normal, roughness, cameraPosition, worldPosition).xxx;
	float3 directionSpecularity = lightColor * lambert * dirrfresnel * distribution * visibility;


	float shadow = ShadowBuffer(worldPosition);

	float3 finalColor = (directionDiffuse + directionSpecularity);
	finalColor *= directionalLight.intensity * shadow;
	output.color = float4(finalColor, 1.0f);
	return output;
}