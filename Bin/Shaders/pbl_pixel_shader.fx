#include <shader_structs.fx>
#include <Fullscreen/structs.fx>
TextureCube cubeMap : register(t0);

Texture2D albedoTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D depthTexture : register(t3);



SamplerState SamplerClamp : register(s0);
SamplerState samplerWrap : register(s1);

cbuffer ConstantBuffer : register( b0 ) //to vertex
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;
	
	float4x4 cameraSpace;
	float4x4 projectionInverse;

	float4x4 shadowCamInverse;
	float4x4 shadowCamProjection;

	float numberOfLights;
	float3 garbage;
}

 #define NUMBER_POINTLIGHTS 8
cbuffer PixelShaderBuffer : register(b1) //to pixel
 {
 	float4 crap;

 	struct DirectionalLight
 	{
 		float4 direction;
 		float4 color;
 	} otherCrap;

 	struct PointLight
 	{
 		float3 position;
 		float range;
 		float3 color;
 		float intensity;
 	} mostCrap[NUMBER_POINTLIGHTS];

 	float4 highlightColor;

 	uint numberOfPointLights;
 	uint alsocrap;
 	float2 trash;
 }

cbuffer PointLightBuffer : register(b2)
{
	float4 cameraPos;
 	struct DirectionalLight myDirectionalLight;
	struct PointLight pointLight;
	uint CubeMap_MipCount;
	float3 soMuchCrap;
}

float PS_Depth(PosTex_InputPixel input)
{
	return depthTexture.Sample(samplerWrap, input.tex.xy).r;
}

float3 WorldPosition(PosTex_InputPixel input, float aDepth)
{
	float4 viewPos = float4(input.tex.x * 2 - 1, -(input.tex.y * 2 - 1), aDepth, 1);
	viewPos = mul(projectionInverse, viewPos);
	viewPos = mul(cameraSpace, viewPos);
	
	viewPos = viewPos / viewPos.w;
	
	return viewPos.xyz;
}

PixelOutput PS_Emissive(PosTex_InputPixel input)
{
	PixelOutput output;
	float3 roughness = float3(0,0,0);//emissiveMap.Sample(samplerWrap, input.uv.xy).xyz;
	output.color = float4(roughness.xyz, 1.0f);
	return output;
}

PixelOutput PS_Albedo(PosTex_InputPixel input)
{
	PixelOutput output;
	output.color.xyz = albedoTexture.Sample(samplerWrap, input.tex.xy).xyz;
	output.color.w = 1.f;
	return output;
}

PixelOutput PS_AmbientOcclusion(PosTex_InputPixel input)
{
	PixelOutput output;
	output.color.xyz = float3(0,0,0);//roughnessMetalnessAO.Sample(samplerWrap, input.uv).zzz; //UV på allt ELLEH ?
	output.color.a = 1.0f;
	return output;
}

PixelOutput PS_ObjectNormal(PosTex_InputPixel input)
{
	float3 normal = normalTexture.Sample(samplerWrap, input.tex.xy).xyz;
	PixelOutput output;
	output.color.xyz = normal * 2 - 1;
	output.color.w = 1.f;
	return output;
}

PixelOutput PS_Metalness(PosTex_InputPixel input)
{
	PixelOutput output;
	output.color.xyz = float3(0,0,0);//roughnessMetalnessAO.Sample(samplerWrap, input.uv).xxx;
	output.color.a = 1.0f;
	return output;
}

PixelOutput PS_Roughness(PosTex_InputPixel input)
{
	PixelOutput output;
	output.color.xyz = float3(0.5,0.5,0.5);//roughnessMetalnessAO.Sample(samplerWrap, input.uv.xy).xxx;
	output.color.w = 1.f;
	return output;
}

	// PIXEL SHADER
float4 PS_Pos(Pos_InputPixel input) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}


PixelOutput PS_PBL(PosNormBinormTanTex_InputPixel input);
PixelOutput DistanceFog(PosNormBinormTanTex_InputPixel aInput, float4 aColorInput);
PixelOutput PS_ReflectionFresnel(PosNormBinormTanTex_InputPixel input);

	// Coola värden för att ändra foggen, detta kommer att komma in i Lua inladdningen och sättas via Engine asball!
static const float fogStart = 50000.0f; // womba
static const float fogEnd = 75000.0f;

PixelOutput DistanceFog(PosTex_InputPixel aInput, float4 aColorInput, float3 worldPosition)
{
	PixelOutput output;
	float3 CameraPosition = cameraPos.xyz;
	float3 CamToObj = (worldPosition.xyz - CameraPosition);

	float4 inputColor = aColorInput; //Color From PBL

	float4 fogColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 CamVec = (CameraPosition - worldPosition.xyz);



	float SubOp = (length(CamVec) - fogStart); // y is near
	float SubOp41 = (fogEnd - fogStart); // x is far

	float SatOp = saturate((SubOp / SubOp41));
	float PowOp = pow((1.0 - SatOp), 2.0);
	int mipLevel = lerp(0, 5, PowOp);
	float4 CubeMapColor = cubeMap.SampleLevel(samplerWrap, CamToObj.xyz, mipLevel); // color from CubeMap


	float4 LerpOp = lerp(CubeMapColor, inputColor, PowOp);

	float4 VectorConstruct = float4(LerpOp.xyz.x, LerpOp.xyz.y, LerpOp.xyz.z, 1.0);

	output.color = VectorConstruct;
	return output;
}

PixelOutput PS_Lambert(PosTex_InputPixel input, float3 aNormal, float3 aDirection)
{
	PixelOutput output;
	output.color.xyz = saturate(dot(aNormal, -aDirection));
	output.color.a = 1.0f;
	return output;
}



	//LYS i think!
float RoughToSPow(float fRoughness)
{
	return 1;//(2.0f / (fRoughness * fRoughness)) - 2.0f;
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

PixelOutput PS_Substance(PosTex_InputPixel input)
{
	float3 albedo = PS_Albedo(input).color.xyz;
	float3 metalness = PS_Metalness(input).color.xyz;
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo * metalness;

	PixelOutput output;
	output.color = float4(substance.xyz, 1.0f);
	return output;
}

PixelOutput PS_ReflectionFresnel(PosTex_InputPixel input, float3 worldPosition)
{
	float roughness = PS_Roughness(input).color.x;
	float3 substance = PS_Substance(input).color.xyz;
	float3 normal = PS_ObjectNormal(input).color.xyz;
	
	float3 toEye = normalize(cameraPos.xyz - worldPosition.xyz);
	float VdotN = dot(toEye.xyz, normal);

	VdotN = saturate(VdotN);
	VdotN = 1.0f - VdotN;
	VdotN = pow(VdotN, 5); //whaah

	float3 fresnel = VdotN * (1.f - substance);
	fresnel = fresnel / (6 - 5 * roughness);
	fresnel = substance + fresnel;

	PixelOutput output;
	output.color = float4(fresnel.xyz, 1.0f);
	return output;
}

PixelOutput PS_Fresnel(PosTex_InputPixel input, float3 aDirection, float3 worldPosition)
{
	float3 toLight = -aDirection;
	
	float3 toEye = normalize(cameraPos.xyz -worldPosition.xyz);
	float3 substance = PS_Substance(input).color.xyz;
	float3 halfvec = normalize(toLight + toEye);

	float LdotH = dot(toLight, halfvec);
	LdotH = saturate(LdotH);
	LdotH = 1.0f - LdotH;
	LdotH = pow(LdotH, 5);

	float3 fresnel = LdotH * (1.f - substance);
	fresnel = substance + fresnel;

	PixelOutput output;
	output.color = float4(fresnel.xyz, 1.0f);
	return output;
}


PixelOutput PS_Distribution(PosTex_InputPixel input, float3 aDirection,float3 worldPosition)
{
	float3 toLight = -aDirection.xyz;

	float roughness = PS_Roughness(input).color.x;
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 toEye = normalize(cameraPos.xyz - worldPosition.xyz);
	float3 halfvec = normalize(toLight + toEye.xyz);
	float HdotN = saturate(dot(halfvec, normal));

	float m = roughness * roughness; // roughness ^2 or what?
	float m2 = m * m; //roughness ^4 or what?

	float denominator = HdotN * HdotN * (m2 - 1.0f) + 1.0f;
	float distribution = m2 / (3.1415926538f * denominator * denominator);

	PixelOutput output;
	output.color = float4(distribution.xxx, 1.0f);
		//output.color = float4(roughness.xxx, 1.0f);
	return output;
}

	//mebe only able to run PBL if we're using PosNormBinormTanTex.
PixelOutput PS_Visibility(PosTex_InputPixel input, float3 aDirection,float3 worldPosition)
{
	float3 toLight = -aDirection.xyz;

	float roughness = PS_Roughness(input).color.x;
	float roughnessRemapped = (roughness + 1.0f) / 2.0f;
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 toEye = normalize(cameraPos.xyz - worldPosition.xyz);

	float NdotL = saturate(dot(normal, toLight));
	float NdotV = saturate(dot(normal, toEye));

	float k = roughnessRemapped * roughnessRemapped * 1.7724f; //sqrt(PI) wth is K?
	float G1V = NdotV * (1.0f - k) + k;
	float G1L = NdotL * (1.0f - k) + k;
	float visibility = (NdotV * NdotL) / (G1V * G1L);

	PixelOutput output;
	output.color.xyz = visibility.xxx;
	output.color.a = 1.0f;
	return output;
}

PixelOutput PS_MetalnessAlbedo(PosTex_InputPixel input)
{
	float3 albedo = PS_Albedo(input).color.xyz;
	float3 metalness = PS_Metalness(input).color.xyz;
	float3 metalnessAlbedo = albedo - (albedo * metalness);

	PixelOutput output;
	output.color = float4(metalnessAlbedo.xyz, 1.0f);
	return output;
}

PixelOutput PS_AmbientDiffuse(PosTex_InputPixel input,float3 worldPosition)
{
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 metalnessAlbedo = PS_MetalnessAlbedo(input).color.xyz;
	float3 ambientOcclusion = PS_AmbientOcclusion(input).color.xyz;

	float3 ambientLight = cubeMap.SampleLevel(samplerWrap, normal.xyz, (uint) CubeMap_MipCount.x - 2).xyz; // FIX!
	float3 fresnel = PS_ReflectionFresnel(input, worldPosition).color.xyz;

	PixelOutput output;
	output.color = float4(metalnessAlbedo * ambientLight * ambientOcclusion * (float3(1.0f, 1.0f, 1.0f) - fresnel), 1.0f);
	return output;
}

PixelOutput PS_AmbientSpecularity(PosTex_InputPixel input,float3 worldPosition)
{
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float roughness = PS_Roughness(input).color.x;
	float3 ambientOcclusion = PS_AmbientOcclusion(input).color.xxx;
	float3 toEye = normalize(cameraPos.xyz - worldPosition.xyz);
	float3 reflectionVector = -reflect(toEye, normal);

	float fakeLysSpecularPower = RoughToSPow(roughness);
	float lysMipMap = GetSpecPowToMip(fakeLysSpecularPower, (uint) CubeMap_MipCount.x);

	float3 ambientLight = cubeMap.SampleLevel(samplerWrap, reflectionVector.xyz, lysMipMap).xyz;
	float3 fresnel = PS_ReflectionFresnel(input, worldPosition).color.xyz;

	PixelOutput output;
	output.color = float4(ambientLight * ambientOcclusion * fresnel, 1.0f);
	return output;
}

PixelOutput PS_DirectDiffuse(PosTex_InputPixel input, float3 normal, float3 aLightColor, float3 aLightDirection, float3 worldPosition)
{
	float3 lightColor = aLightColor.xyz;

	float3 metalnessAlbedo = PS_MetalnessAlbedo(input).color.xyz;

	float3 lambert = PS_Lambert(input, normal, aLightDirection).color.xyz;
	float3 fresnel = PS_Fresnel(input, aLightDirection, worldPosition).color.xyz;

	float3 one = float3(1.0f, 1.0f, 1.0f);
	PixelOutput output;
	output.color = float4(metalnessAlbedo * lightColor * lambert * (one - fresnel), 1.0f);
	return output;
}

PixelOutput PS_DirectSpecularity(PosTex_InputPixel input, float3 normal, float3 aLightColor, float3 aLightDirection, float3 worldPosition)
{
	float3 lightColor = aLightColor.xyz;

	float3 lambert = PS_Lambert(input, normal, aLightDirection).color.xyz;
	float3 fresnel = PS_Fresnel(input, aLightDirection,worldPosition).color.xyz;
	float3 distribution = PS_Distribution(input, aLightDirection,worldPosition).color.xxx;
	float3 visibility = PS_Visibility(input, aLightDirection, worldPosition).color.xxx;

	PixelOutput output;
	output.color = float4(lightColor * lambert * fresnel * distribution * visibility, 1.0f);
	return output;
}

PixelOutput PS_PBL(PosTex_InputPixel input)
{
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 emissive = PS_Emissive(input).color.xyz;
	float3 albedo = PS_Albedo(input).color.xyz;
	float4 RoughnessMetalnessAO = float4(1,1,1,1);//roughnessMetalnessAO.Sample(samplerWrap, input.uv.xy);

	float3 metalness = RoughnessMetalnessAO.yyy;



	float3 metalnessAlbedo = albedo - (albedo * metalness);
	float3 ambientOcclusion = RoughnessMetalnessAO.zzz;
	float3 ambientLight = cubeMap.SampleLevel(samplerWrap, normal.xyz, (uint) CubeMap_MipCount.x - 2).xyz; // FIX!
	float roughness = RoughnessMetalnessAO.x;
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo * metalness;
	
	float depth = PS_Depth(input);

	PixelOutput output = (PixelOutput)0;;
	
	output.color = float4(0,0,0,0);
	if(depth < 0.999)
	{

	

	float3 worldPosition = WorldPosition(input, depth);
	float3 toEye = normalize(cameraPos.xyz - worldPosition.xyz);
	float VdotN = dot(toEye.xyz, normal);
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
	float lysMipMap = GetSpecPowToMip(fakeLysSpecularPower, (uint) CubeMap_MipCount.x);
	float3 ambientLightSpec = cubeMap.SampleLevel(samplerWrap, reflectionVector.xyz, lysMipMap).xyz;
	float3 ambientSpecularity = ambientLightSpec * ambientOcclusion * fresnel;

	//DIRR DIFF

	float3 lightColor = float3(0,0,0);//myDirectionalLight.color.rgb;
	float3 lambert = PS_Lambert(input, normal, myDirectionalLight.direction.xyz).color.xyz;
	float3 toLight = -myDirectionalLight.direction.xyz;
	float3 halfvec = normalize(toLight + toEye);
	float LdotH = dot(toLight, halfvec);
	LdotH = saturate(LdotH);
	LdotH = 1.0f - LdotH;
	LdotH = pow(LdotH, 5);
	float3 Dirrfresnel = LdotH * (1.f - substance);
	Dirrfresnel = substance + Dirrfresnel;
	float3 one = float3(1.0f, 1.0f, 1.0f);
	float3 directionDiffuse = metalnessAlbedo * lightColor * lambert * (one - Dirrfresnel);



//DIRR SPEC
		//float3 directionSpecularity = PS_DirectSpecularity(input, normal, myDirectionalLight.color.rgb, myDirectionalLight.direction.xyz).color.xyz;

	lightColor = myDirectionalLight.color.rgb;
	float3 distribution = PS_Distribution(input, myDirectionalLight.direction.xyz, worldPosition).color.xxx;
	float3 visibility = PS_Visibility(input, myDirectionalLight.direction.xyz, worldPosition).color.xxx;
	float3 directionSpecularity = lightColor * lambert * Dirrfresnel * distribution * visibility;


	//Shadow magicz

	float4 shadowCamPosition = float4(worldPosition,1.f);
	float4x4 sdwCinv = shadowCamInverse;
	float4x4 sdwCPrj = shadowCamProjection;

	shadowCamPosition = mul(shadowCamInverse, shadowCamPosition);
	shadowCamPosition = mul(shadowCamProjection, shadowCamPosition);

	shadowCamPosition /= shadowCamPosition.w;

	float2 texCord;
	texCord.x = shadowCamPosition.x * 0.5f + 0.5f;
	texCord.y = shadowCamPosition.y * -0.5f + 0.5f;
	float shadowCamDepth = 0.f;//shadowBuffer.SampleLevel(samplerWrap, texCord, 0).x;



	if(shadowCamDepth < shadowCamPosition.z - 0.001f && shadowCamDepth != 0.f)
	{
		directionDiffuse.rgb = 0.0f;
		directionSpecularity.rgb = 0.0f;
	}	

		float3 difference = (pointLight.position.xyz) - worldPosition.xyz;
    	float l = length(difference);

    	float lightRange = l / pointLight.range;
		lightRange = saturate(lightRange);
		lightRange = 1.0f - lightRange;


    	float3 direction = normalize(difference);
    	float4 color = (float4)0;
    	lightColor = pointLight.color.rgb;
    	lambert = PS_Lambert(input, normal, -direction).color.xyz;
    	toLight = direction;
    	halfvec = normalize(toLight + toEye);
    	LdotH = dot(toLight, halfvec);
    	LdotH = saturate(LdotH);
    	LdotH = 1.0f - LdotH;
    	LdotH = pow(LdotH, 5);
    	Dirrfresnel = LdotH * (1.f - substance);
    	Dirrfresnel = substance + Dirrfresnel;
    	color = float4(metalnessAlbedo * pointLight.color.rgb * lambert * (one - Dirrfresnel), 1.0f);
    	directionDiffuse = color.rgb * pointLight.intensity * lightRange;
    	distribution = PS_Distribution(input, -direction, worldPosition).color.xxx;
    	visibility = PS_Visibility(input, -direction, worldPosition).color.xxx;
    	directionSpecularity = lightColor * lambert * Dirrfresnel * distribution * visibility * pointLight.intensity * lightRange;

	

		output.color = float4(ambientDiffuse + ambientSpecularity + directionDiffuse + directionSpecularity + emissive, 1.0f);
	}
	return output;
}

PixelOutput PS_PosTex(PosTex_InputPixel input)
{
	PixelOutput output = (PixelOutput)0;
	output.color = PS_PBL(input).color;
	output.color.w = 1.f/ numberOfLights;

	return output;
}