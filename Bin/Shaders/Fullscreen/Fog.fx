#include <structs.fx>
#include <../shader_structs.fx>
#include <../OncePerFrame.fx>

Texture2D renderPackage : register(t1);
Texture2D depthPackage	: register(t2);
Texture2D depthPackage2 : register(t3);


SamplerState Sampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer CameraBuffer : register(b1)
{
	float4x4 projectionInverse;
	float4x4 cameraSpace;
}


float4 DistanceFog(PosTex_InputPixel aInput, float4 aColorInput);

PixelOutput PS_PosTex(PosTex_InputPixel input)
{
	PixelOutput output;
	output.color = renderPackage.SampleLevel(Sampler, input.tex, 0);
	output.color = DistanceFog(input, output.color);
	return output;
}

float3 CameraPosition(float4x4 aCameraSpace)
{
	float4 cameraPosition = (float4) 0;
	cameraPosition.w = 1.0f;
	cameraPosition = mul(aCameraSpace, cameraPosition);
	cameraPosition = cameraPosition / cameraPosition.w;
	return cameraPosition.xyz;
}


float GetDepth(float2 uv)
{
	float depth = depthPackage2.Sample(Sampler, uv).x;
	return (depth == 1.0f) ? depthPackage.Sample(Sampler, uv).x : depth;
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

static const float lFogStart = 60.0f;
static const float lFogEnd = 250.0f;
static const float4 lFogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

float4 DistanceFog(PosTex_InputPixel aInput, float4 aColorInput)
{

	if (aColorInput.a <= ALPHA_THRESHOLD)
	{
		discard;
	}
	float depth = GetDepth(aInput.tex);
	float3 worldPosition = WorldPosition(aInput.tex, depth);

	float3 cameraPosition = CameraPosition(cameraSpace);

	float4 inputColor = aColorInput; //Color From PBL

	float4 fogColor = lFogColor;

	float3 CamVec = (cameraPosition - worldPosition);

	float SubOp = (length(CamVec) - lFogStart);
	float SubOp41 = (lFogEnd - lFogStart);

	float SatOp = saturate((SubOp / SubOp41));
	float PowOp = pow((1.0 - SatOp), 2.0);
	float4 LerpOp = lerp(fogColor, inputColor, PowOp);
	float4 VectorConstruct = float4(LerpOp.x, LerpOp.y, LerpOp.z, 1.0);


	//return float4(depth.xxx, 1.0f);
	//return float4(normalize(cameraPosition), 1.0f);

	return VectorConstruct;
}
