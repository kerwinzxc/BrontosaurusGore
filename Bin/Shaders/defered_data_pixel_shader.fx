#include <shader_structs.fx>

TextureCube cubeMap : register(t0);

Texture2D diffuseTexture : register(t1);
Texture2D roughnessMetalnessAO : register(t3);
Texture2D emissiveMap : register(t4);
Texture2D normalMap : register(t5);

Texture2D shadowBuffer : register(t7);


SamplerState SamplerClamp : register(s0);
SamplerState samplerWrap : register(s1);

cbuffer ConstantBuffer : register( b0 ) //to vertex
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;

	float4x4 shadowCamInverse;
	float4x4 shadowCamProjection;

	float garbageo[4];
}

#define NUMBER_POINTLIGHTS 8
cbuffer PixelShaderBuffer : register(b1) //to pixel
{
	float4 cameraPos;

	struct DirectionalLight
	{
		float4 direction;
		float4 color;
	} myDirectionalLight;

	struct PointLight
	{
		float3 position;
		float range;
		float3 color;
		float intensity;
	} myPointLights[NUMBER_POINTLIGHTS];

	float4 highlightColor;

	uint numberOfPointLights;
	uint CubeMap_MipCount;
	float2 trash;
}

struct PixelOutput
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float depth : SV_TARGET2;
};


	// PIXEL SHADER
float4 PS_Pos(Pos_InputPixel input) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}


PixelOutput PS_Data(PosNormBinormTanTex_InputPixel input);
PixelOutput DistanceFog(PosNormBinormTanTex_InputPixel aInput, float4 aColorInput);
PixelOutput PS_ReflectionFresnel(PosNormBinormTanTex_InputPixel input);
PixelOutput PS_ObjectNormal(PosNormBinormTanTex_InputPixel input);

float4 PS_FresnelHighlight(PosNormBinormTanTex_InputPixel input, float4 aColorInput);

PixelOutput PS_PosNormBinormTanTex(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput pblColor = PS_Data(input);
	
	pblColor.color += PS_FresnelHighlight(input, pblColor.color) * highlightColor.a;
	//DistanceFog(input, pblColor).color;


	return pblColor;
}

float4 PS_FresnelHighlight(PosNormBinormTanTex_InputPixel input, float4 aColorInput)
{
	float3 normal = PS_ObjectNormal(input).color.xyz;


	float4 TextureIn = float4(0.0f, 0.0f, 0.0f, 1.0f);//aColorInput;
	float4 RimlightColor = highlightColor;

	float4x4 viewI = cameraSpaceInversed;

	float3 CameraPosition = cameraPos.xyz;
	float3 CamVec = (CameraPosition - input.worldPosition.xyz);
	float3 CamVecNorm = normalize(CamVec);

	float AddOp = (-0.4 + CamVecNorm.y);
	float3 VectorConstruct = float3(CamVecNorm.x, AddOp, CamVecNorm.z);
	float3 NormOp = normalize(normal);

	float AddOp75 = (dot(VectorConstruct.xyz, NormOp) + -0.4);

	float power = 0.5f;// 0.182219

	float PowOp = (pow(AddOp75, power));
	float Contrastmulstiplier = ((PowOp - 0.5) * 200.0);
	float ContrastAdd = (Contrastmulstiplier + 0.5);
	float SatOp = saturate(ContrastAdd);
	float AddOp92 = max(0, (dot(CamVecNorm, NormOp) + -0.4));

	float PowOp90 = (pow(AddOp92, 0.144385));
	float Contrastmulstiplier98 = ((PowOp90 - 0.5) * 5.0);
	float ContrastAdd97 = (Contrastmulstiplier98 + 0.5);
	float SatOp104 = saturate(ContrastAdd97);
	//float OneMinusOp = (1.0 - (SatOp * SatOp104));
	float OneMinusOp = (1.0 - SatOp104);
	float4 AddOp84 = (TextureIn + (RimlightColor * OneMinusOp));
	float4 SatOp85 = saturate(AddOp84);
	float4 VectorConstruct102 = float4(SatOp85.xyz.x, SatOp85.xyz.y, SatOp85.xyz.z, 1.0);

	//float4 testColor = ((float4(CameraPosition.xyz, 1.0f)));
	//return testColor;
	//return aColorInput * RimlightColor;
	return VectorConstruct102;

}


PixelOutput PS_PosNormBinormTanTexBones(PosNormBinormTanTex_InputPixel input)
{
	return PS_PosNormBinormTanTex(input);
}

PixelOutput PS_Lambert(PosNormBinormTanTex_InputPixel input, float3 aNormal, float3 aDirection)
{
	PixelOutput output;
	output.color.xyz = saturate(dot(aNormal, -aDirection));
	output.color.a = 1.0f;
	return output;
}

PixelOutput PS_Albedo(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;
	output.color.xyz = diffuseTexture.Sample(samplerWrap, input.uv).xyz;
	output.color.a = 1.0f;
	return output;
}

PixelOutput PS_Roughness(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;
	float3 roughness = roughnessMetalnessAO.Sample(samplerWrap, input.uv.xy).xxx;
	output.color = float4(roughness.xyz, 1.0f);
	return output;
}

PixelOutput PS_AmbientOcclusion(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;
	output.color.xyz = roughnessMetalnessAO.Sample(samplerWrap, input.uv).zzz; //UV på allt ELLEH ?
	output.color.a = 1.0f;
	return output;
}

PixelOutput PS_Emissive(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;
	float3 roughness = emissiveMap.Sample(samplerWrap, input.uv.xy).xyz;
	output.color = float4(roughness.xyz, 1.0f);
	return output;
}

PixelOutput PS_ObjectNormal(PosNormBinormTanTex_InputPixel input)
{
	float3 normal = normalMap.Sample(samplerWrap, input.uv.xy).xyz;
	normal = (normal * 2.0f) - float3(1.0f, 1.f, 1.f);
	normal = normalize(normal);
	float3x3 tangentSpaceMatrix = float3x3(normalize(input.biTangent.xyz), normalize(input.tangent.xyz), normalize(input.normals.xyz)); // om fel, kanske här? Vem vet
	normal = mul(normal, tangentSpaceMatrix);

	PixelOutput output;
	output.color = (float4(normal.xyz, 1.0f));
	return output;
}


PixelOutput PS_Metalness(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;
	output.color.xyz = roughnessMetalnessAO.Sample(samplerWrap, input.uv).xxx;
	output.color.a = 1.0f;
	return output;
}

PixelOutput PS_Data(PosNormBinormTanTex_InputPixel input)
{
	float3 normal = PS_ObjectNormal(input).color.xyz;
    float3 albedo = PS_Albedo(input).color.xyz;
    PixelOutput output = (PixelOutput)0;

	output.color.xyz = albedo;
	output.color.w = 1;
	output.normal = float4((normal + float3(1,1,1)) / 2,0);
	
	output.depth = input.position.z;

    return output;
}


PixelOutput PS_SkyBox(PosNormBinormTanTex_InputPixel input)
{
	float3 toPixel = normalize(input.worldPosition.xyz - cameraPos.xyz);
	float3 ambientLight = cubeMap.SampleLevel(samplerWrap, toPixel.xyz, 0).xyz;

	PixelOutput output;
	output.color.xyz = ambientLight;
	output.color.a = 1.0f;
	return output;
}
