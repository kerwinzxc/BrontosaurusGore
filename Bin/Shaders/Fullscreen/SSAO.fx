#include <../OncePerFrame.fx>

Texture2D normal : register(t2);
Texture2D RMAO	 : register(t3);
Texture2D depth  : register(t5);
Texture2D random : register(t6);

SamplerState SamplerClamp : register(s0);
SamplerState samplerWrap  : register(s1);

cbuffer ProjectionInverse : register(b1)
{
	float4x4 projectionInverse;
	float4x4 cameraSpace;
}

static const float random_size = 32.f;
static const float g_sample_rad = .08f;
static const float g_intensity = 1.0f;
static const float g_scale = .25f;
static const float g_bias = 0.2f;

float3 getPosition(in float2 uv)
{
	float zDepth = depth.Sample(SamplerClamp, uv).x;

	float4 worldPosition = (float4) 0;
	worldPosition.xy = (uv * 2.0f);
	worldPosition.xy -= float2(1.0f, 1.0f); // transforms to -1 to 1
	worldPosition.y *= -1.0f; // flip y for wth!
	worldPosition.z = zDepth;
	worldPosition.w = 1.0f;
	worldPosition = mul(projectionInverse, worldPosition);
	worldPosition = worldPosition / worldPosition.w;
	return worldPosition.xyz;
}

float4 getNormal(in float2 uv)
{
	return normalize(normal.Sample(SamplerClamp, uv) * 2.0f - 1.0f);
}

float3 getRandom(in float2 uv)
{
	float2 texCoord = windowSize * uv / random_size;
	return normalize(random.SampleLevel(samplerWrap, texCoord, 0).xyz * 2.0f - 1.0f).xyz;
}

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
	float3 diff = getPosition(tcoord + uv) - p;
	const float3 v = normalize(diff);
	const float d = length(diff) * g_scale;
	return (1.0f - max(0.0, dot(cnorm, v) - g_bias) * (1.0 / (1.0 + d))) * g_intensity;
}

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD;
};

struct PixelOutput
{
	float4 RMAO : SV_TARGET;
};

// Cooly Shader(SSAO)
PixelOutput PS_PosTex(PixelInput input)
{
	PixelOutput output;
	

	output.RMAO.rgba = 1.0f;
	const float2 vec[4] =
	{
		float2(1, 0), 
		float2(-1, 0),
		float2(0, 1), 
		float2(0, -1)
	};

	float3 p = getPosition(input.uv);
	//float4 normal = getNormal(input.tex);

	float4 norm = normal.Sample(SamplerClamp, input.uv);
	norm = (norm * 2.0f);
	norm.xyz -= float3(1.0f, 1.0f, 1.0f);
	float alpha = norm.a;

	float3 n = norm.xyz;
	if (norm.a <= 0.f)
	{
		discard;
	}
	n = mul((float3x3)cameraSpaceInversed, n);

	float3 rand = getRandom(input.uv);

	float ao = 0.0f;	
	float rad = g_sample_rad / p.z;

//**SSAO Calculation**//
	int iterations = 4;
	for (int j = 0; j < iterations; ++j)
	{
		float2 coord1 = reflect(vec[j], rand.xy) * rad;
		float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707,
			  coord1.x * 0.707 + coord1.y * 0.707);
  
		ao += doAmbientOcclusion(input.uv, coord1 * 0.25, p, n);
		ao += doAmbientOcclusion(input.uv, coord2 * 0.5, p, n);
		ao += doAmbientOcclusion(input.uv, coord1 * 0.75, p, n);
		ao += doAmbientOcclusion(input.uv, coord2, p, n);
	}
	ao /= (float) iterations * 4.0;
//**END**//

	output.RMAO.rg = 0.0f;
	output.RMAO.b = ao;
	output.RMAO.a = 1.0f;

	return output;
}