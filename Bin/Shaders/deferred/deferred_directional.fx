#include <..\Fullscreen\structs.fx>
#include <..\oncePerFrame.fx>

#define NUM_CASCADES 3

//**********************************************//
//					TEXTURES					//
//**********************************************//

TextureCube cubeMap             : register(t0);
Texture2D deferred_diffuse      : register(t1);
Texture2D deferred_normal       : register(t2);
Texture2D deferred_RMAO			: register(t3);
Texture2D deferred_emissive     : register(t4);
Texture2D deferred_depth        : register(t5);
Texture2D deferred_depth2		: register(t6);

Texture2D shadowBuffer          : register(t8);

//**********************************************//
//					SAMPLERS					//
//**********************************************//

SamplerState samplerClamp       : register(s0);
SamplerState samplerWrap        : register(s1);
SamplerState samplerPointWrap	: register(s2);



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

cbuffer ShadowBufferData		: register(b4)
{
	struct Cascade
	{
		float4x4 projection;
		float4x4 invTransform;
		float4 rect;
	} cascades[NUM_CASCADES];

	float4 cascadeEnds;//[NUM_CASCADES + 1];
	float cascadeFar;

	float3 shadowGarbage;
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
	float3 normal = deferred_normal.Sample(samplerClamp, uv).xyz;
	normal = (normal * 2.0f);
	normal -= float3(1.0f, 1.0f, 1.0f);
	return normal;
}

float3 CameraSpace(float2 uv, float depth)
{
	float4 cameraPosition = (float4) 0;
	cameraPosition.xy = (uv * 2.0f);
	cameraPosition.xy -= float2(1.0f, 1.0f); // transforms to -1 to 1
	cameraPosition.y *= -1.0f; // flip y for wth!
	cameraPosition.z = depth;
	cameraPosition.w = 1.0f;
	cameraPosition = mul(projectionInverse, cameraPosition);
	return cameraPosition.xyz;
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

uint GetCascadeIndex(float aDepth)
{
	uint cascadeIndex = 0;
	[unroll]
	for (uint i = 0; i < NUM_CASCADES - 1; ++i)
	{
		if (aDepth > cascadeEnds[i])
		{
			if (aDepth < cascadeEnds[i + 1])
			{
				cascadeIndex = i;
			}
		}
	}
	if (aDepth > cascadeEnds[NUM_CASCADES - 1])
	{
		cascadeIndex = NUM_CASCADES - 1;
	}
	return cascadeIndex;
}


float2 GetShadowMapUV(uint cascadeIndex, float2 cascadeProjectionCoords)
{
	float2 texCoord = (float2) 0.0f;
	texCoord.x = cascadeProjectionCoords.x * 0.5f + 0.5f;
	texCoord.y = cascadeProjectionCoords.y * -0.5f + 0.5f;
	texCoord.x *= cascades[cascadeIndex].rect.z - cascades[cascadeIndex].rect.x;
	texCoord.x += cascades[cascadeIndex].rect.x;
	texCoord.y *= cascades[cascadeIndex].rect.w - cascades[cascadeIndex].rect.y;
	texCoord.y += cascades[cascadeIndex].rect.y;
	return texCoord;
}

float4 GetCascadeProjectionSpacePosition(uint cascadeIndex, float3 worldPosition)
{
	float4 pixelCascadePos = float4(worldPosition, 1.0f);
	pixelCascadePos = mul(cascades[cascadeIndex].invTransform, pixelCascadePos);
	pixelCascadePos = mul(cascades[cascadeIndex].projection, pixelCascadePos);
	pixelCascadePos /= pixelCascadePos.w;
	return pixelCascadePos;
}

#define SHADOWMAP_TEXTURESIZE 2048.f
#define LOOP_SNURR 3
static float2 shadowmapTexelSize = 1.0f / SHADOWMAP_TEXTURESIZE;

float ShadowBuffer(float3 worldPosition, float depth, float2 uv)
{
	float output = 1.0f;

	uint cascadeIndex = GetCascadeIndex(depth);
	float4 pixelPosCascade = GetCascadeProjectionSpacePosition(cascadeIndex, worldPosition);

	float2 texCoord = GetShadowMapUV(cascadeIndex, pixelPosCascade.xy);

	//float shadowMapDepth = shadowBuffer.SampleLevel(samplerPointWrap, texCoord, 0).x;

	float3 normal = deferred_normal.SampleLevel(samplerWrap, uv, 0).xyz;
	float bias = dot(directionalLight.direction, normal);
				//max(0.05 * (1.0 - dot(normal, directionalLight.direction)), 0.005);
	bias = clamp(bias , 0.008f, 0.04f);


	//if (shadowMapDepth < pixelPosCascade.z - bias && shadowMapDepth != 0.f)
	//{
	//	output = 0.0f;
	//}
	
	float shadow = 0.0f;
	float samples = 0.0f;

	for (int x = -LOOP_SNURR; x <= LOOP_SNURR; ++x)
	{
		for (int y = -LOOP_SNURR; y <= LOOP_SNURR; ++y)
		{
			float pcfDepth = shadowBuffer.SampleLevel(samplerWrap, texCoord + float2(x, y) * shadowmapTexelSize /** ((float)(cascadeIndex + 1) / (float)NUM_CASCADES)*/, 0).x;
			shadow += pixelPosCascade.z - bias > pcfDepth ? 0.0 : 1.0;
			samples += 1.0f;
		}
	}

	shadow /= samples;
	return shadow;
}


float GetDepth(float2 uv)
{
	float depth = deferred_depth2.Sample(samplerClamp, uv).x;
	return (depth == 1.0f) ? deferred_depth.Sample(samplerClamp, uv).x : depth;
}


Output PS_PosTex(PosTex_InputPixel inputPixel)
{
	Output output;
	
	
	float2 uv = inputPixel.tex;
	float1 depth = GetDepth(uv);

	float4 fullAlbedo = deferred_diffuse.Sample(samplerClamp, uv).rgba;

	//if (depth >= DEPTH_BIAS)
	//{
	//	output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//	return output;
	//}

	float3 albedo = fullAlbedo.rgb;
	float3 worldPosition = WorldPosition(uv, depth);
	float3 cameraPosition = CameraPosition(cameraSpace);

	float3 normal = Normal(uv);

	float4 RMAO = deferred_RMAO.Sample(samplerClamp, uv);
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

	
	float1 pixelCamDepth = length(worldPosition - cameraPosition);

	float shadow = ShadowBuffer(worldPosition, pixelCamDepth, uv);

	float3 finalColor = (directionDiffuse + directionSpecularity);
	finalColor *= directionalLight.intensity * shadow;
	output.color = float4(finalColor, fullAlbedo.a);
	return output;
}