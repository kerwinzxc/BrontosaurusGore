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
	float3 velocity : SV_TARGET1;
};


	// PIXEL SHADER
float4 PS_Pos(Pos_InputPixel input) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}


PixelOutput PS_PBL(PosNormBinormTanTex_InputPixel input);
PixelOutput DistanceFog(PosNormBinormTanTex_InputPixel aInput, float4 aColorInput);
PixelOutput PS_ReflectionFresnel(PosNormBinormTanTex_InputPixel input);
PixelOutput PS_ObjectNormal(PosNormBinormTanTex_InputPixel input);

float4 PS_FresnelHighlight(PosNormBinormTanTex_InputPixel input, float4 aColorInput);

PixelOutput PS_PosNormBinormTanTex(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;

	float4 pblColor = PS_PBL(input).color;
	pblColor += PS_FresnelHighlight(input, pblColor) * highlightColor.a;
	//DistanceFog(input, pblColor).color;

	output.color = pblColor;
	output.velocity = (input.viewPosition.xyz - input.worldPosLastFrame.xyz) * 100.f;
	return output;
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
PixelOutput PS_PosNormBinormTanTexInstanced(PosNormBinormTanTex_InputPixel input)
{
	return PS_PosNormBinormTanTex(input);
}


	// Coola värden för att ändra foggen, detta kommer att komma in i Lua inladdningen och sättas via Engine asball!
static const float fogStart = 50000.0f; // womba
static const float fogEnd = 75000.0f;

PixelOutput DistanceFog(PosNormBinormTanTex_InputPixel aInput, float4 aColorInput)
{
	PixelOutput output;
	float3 CameraPosition = cameraPos.xyz;
	float3 CamToObj = (aInput.worldPosition.xyz - CameraPosition);

	float4 inputColor = aColorInput; //Color From PBL

	float4 fogColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 CamVec = (CameraPosition - aInput.worldPosition.xyz);



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






PixelOutput PS_Lambert(PosNormBinormTanTex_InputPixel input, float3 aNormal, float3 aDirection)
{
	PixelOutput output;
	output.color.xyz = saturate(dot(aNormal, -aDirection));
	output.color.a = 1.0f;
	return output;
}



	//LYS i think!
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


PixelOutput PS_Substance(PosNormBinormTanTex_InputPixel input)
{
	float3 albedo = PS_Albedo(input).color.xyz;
	float3 metalness = PS_Metalness(input).color.xyz;
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo * metalness;

	PixelOutput output;
	output.color = float4(substance.xyz, 1.0f);
	return output;
}

PixelOutput PS_ReflectionFresnel(PosNormBinormTanTex_InputPixel input)
{
	float roughness = PS_Roughness(input).color.x;
	float3 substance = PS_Substance(input).color.xyz;
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 toEye = normalize(cameraPos.xyz - input.worldPosition.xyz);
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

PixelOutput PS_Fresnel(PosNormBinormTanTex_InputPixel input, float3 aDirection)
{
	float3 toLight = -aDirection;
	float3 toEye = normalize(cameraPos.xyz - input.worldPosition.xyz);
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


PixelOutput PS_Distribution(PosNormBinormTanTex_InputPixel input, float3 aDirection)
{
	float3 toLight = -aDirection.xyz;

	float roughness = PS_Roughness(input).color.x;
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 toEye = normalize(cameraPos.xyz - input.worldPosition.xyz);
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
PixelOutput PS_Visibility(PosNormBinormTanTex_InputPixel input, float3 aDirection)
{
	float3 toLight = -aDirection.xyz;

	float roughness = PS_Roughness(input).color.x;
	float roughnessRemapped = (roughness + 1.0f) / 2.0f;
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 toEye = normalize(cameraPos.xyz - input.worldPosition.xyz);

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

PixelOutput PS_MetalnessAlbedo(PosNormBinormTanTex_InputPixel input)
{
	float3 albedo = PS_Albedo(input).color.xyz;
	float3 metalness = PS_Metalness(input).color.xyz;
	float3 metalnessAlbedo = albedo - (albedo * metalness);

	PixelOutput output;
	output.color = float4(metalnessAlbedo.xyz, 1.0f);
	return output;
}

PixelOutput PS_AmbientDiffuse(PosNormBinormTanTex_InputPixel input)
{
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 metalnessAlbedo = PS_MetalnessAlbedo(input).color.xyz;
	float3 ambientOcclusion = PS_AmbientOcclusion(input).color.xyz;

	float3 ambientLight = cubeMap.SampleLevel(samplerWrap, normal.xyz, (uint) CubeMap_MipCount.x - 2).xyz; // FIX!
	float3 fresnel = PS_ReflectionFresnel(input).color.xyz;

	PixelOutput output;
	output.color = float4(metalnessAlbedo * ambientLight * ambientOcclusion * (float3(1.0f, 1.0f, 1.0f) - fresnel), 1.0f);
	return output;
}

PixelOutput PS_AmbientSpecularity(PosNormBinormTanTex_InputPixel input)
{
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float roughness = PS_Roughness(input).color.x;
	float3 ambientOcclusion = PS_AmbientOcclusion(input).color.xxx;

	float3 toEye = normalize(cameraPos.xyz - input.worldPosition.xyz);
	float3 reflectionVector = -reflect(toEye, normal);

	float fakeLysSpecularPower = RoughToSPow(roughness);
	float lysMipMap = GetSpecPowToMip(fakeLysSpecularPower, (uint) CubeMap_MipCount.x);

	float3 ambientLight = cubeMap.SampleLevel(samplerWrap, reflectionVector.xyz, lysMipMap).xyz;
	float3 fresnel = PS_ReflectionFresnel(input).color.xyz;

	PixelOutput output;
	output.color = float4(ambientLight * ambientOcclusion * fresnel, 1.0f);
	return output;
}

PixelOutput PS_DirectDiffuse(PosNormBinormTanTex_InputPixel input, float3 normal, float3 aLightColor, float3 aLightDirection)
{
	float3 lightColor = aLightColor.xyz;

	float3 metalnessAlbedo = PS_MetalnessAlbedo(input).color.xyz;

	float3 lambert = PS_Lambert(input, normal, aLightDirection).color.xyz;
	float3 fresnel = PS_Fresnel(input, aLightDirection).color.xyz;

	float3 one = float3(1.0f, 1.0f, 1.0f);
	PixelOutput output;
	output.color = float4(metalnessAlbedo * lightColor * lambert * (one - fresnel), 1.0f);
	return output;
}

PixelOutput PS_DirectSpecularity(PosNormBinormTanTex_InputPixel input, float3 normal, float3 aLightColor, float3 aLightDirection)
{
	float3 lightColor = aLightColor.xyz;

	float3 lambert = PS_Lambert(input, normal, aLightDirection).color.xyz;
	float3 fresnel = PS_Fresnel(input, aLightDirection).color.xyz;
	float3 distribution = PS_Distribution(input, aLightDirection).color.xxx;
	float3 visibility = PS_Visibility(input, aLightDirection).color.xxx;

	PixelOutput output;
	output.color = float4(lightColor * lambert * fresnel * distribution * visibility, 1.0f);
		//output.color = float4(distribution, 1.0f);
	return output;
}

PixelOutput PS_PBL(PosNormBinormTanTex_InputPixel input)
{
	float3 normal = PS_ObjectNormal(input).color.xyz;
	float3 emissive = PS_Emissive(input).color.xyz;
	float3 albedo = PS_Albedo(input).color.xyz;
	float4 RoughnessMetalnessAO = roughnessMetalnessAO.Sample(samplerWrap, input.uv.xy);

	float3 metalness = RoughnessMetalnessAO.yyy;



	float3 metalnessAlbedo = albedo - (albedo * metalness);
	float3 ambientOcclusion = RoughnessMetalnessAO.zzz;
	float3 ambientLight = cubeMap.SampleLevel(samplerWrap, normal.xyz, (uint) CubeMap_MipCount.x - 2).xyz; // FIX!
	float roughness = RoughnessMetalnessAO.x;
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo * metalness;
	float3 toEye = normalize(cameraPos.xyz - input.worldPosition.xyz);
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

	float3 lightColor = myDirectionalLight.color.rgb;
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
	float3 distribution = PS_Distribution(input, myDirectionalLight.direction.xyz).color.xxx;
	float3 visibility = PS_Visibility(input, myDirectionalLight.direction.xyz).color.xxx;
	float3 directionSpecularity = lightColor * lambert * Dirrfresnel * distribution * visibility;


	//Shadow magicz

	float4 shadowCamPosition = input.worldPosition;
	float4x4 sdwCinv = shadowCamInverse;
	float4x4 sdwCPrj = shadowCamProjection;

	shadowCamPosition = mul(shadowCamInverse, shadowCamPosition);
	shadowCamPosition = mul(shadowCamProjection, shadowCamPosition);

	shadowCamPosition /= shadowCamPosition.w;

	float2 texCord;
	texCord.x = shadowCamPosition.x * 0.5f + 0.5f;
	texCord.y = shadowCamPosition.y * -0.5f + 0.5f;
	float shadowCamDepth = shadowBuffer.SampleLevel(samplerWrap, texCord, 0).x;



	if(shadowCamDepth < shadowCamPosition.z - 0.001f && shadowCamDepth != 0.f)
	{
		directionDiffuse.rgb = 0.0f;
		directionSpecularity.rgb = 0.0f;
	}


	//
	//float fluff = 0.0005f;
	//float exposure = 0.0f;
	//[unroll]for(int y = -1; y < 2; ++y)
	//{
	//	[unroll]for(int x = -1; x < 2; ++x)
	//	{
	//
	//		float zOffsetX = (input.tangent * (x * fluff)).z;
	//		float zOffsetY = (input.biTangent * (y * fluff)).z;
	//
	//		float2 offset;
	//		offset.x = x * fluff;
	//		offset.y = y * fluff;
	//		float depth = shadowBuffer.SampleLevel(samplerWrap, texCord + offset, 0).x + 0.0001f;
	//
	//		if(shadowCamPosition.z + zOffsetX + zOffsetY < depth)
	//		{
	//			exposure += (1.0f / 9.0f);
	//		}
	//	}
	//}
	//
	//directionDiffuse.rgb *= exposure;
	//directionSpecularity.rgb *= exposure;

// PointLight

	for(uint i = 0; i < numberOfPointLights; ++i)
	{
		float3 difference = (myPointLights[i].position.xyz) - input.worldPosition.xyz;
		float l = length(difference);

		float lightRange = l / myPointLights[i].range;
		lightRange = saturate(lightRange);
		lightRange = 1.0f - lightRange;


		float3 direction = normalize(difference);
		float4 color = (float4)0;
		lightColor = myPointLights[i].color.rgb;
		lambert = PS_Lambert(input, normal, -direction).color.xyz;
		toLight = direction;
		halfvec = normalize(toLight + toEye);
		LdotH = dot(toLight, halfvec);
		LdotH = saturate(LdotH);
		LdotH = 1.0f - LdotH;
		LdotH = pow(LdotH, 5);
		Dirrfresnel = LdotH * (1.f - substance);
		Dirrfresnel = substance + Dirrfresnel;
		color = float4(metalnessAlbedo * myPointLights[i].color.rgb * lambert * (one - Dirrfresnel), 1.0f);
		directionDiffuse += color.rgb * myPointLights[i].intensity * lightRange;
		distribution = PS_Distribution(input, -direction).color.xxx;
		visibility = PS_Visibility(input, -direction).color.xxx;
		//float3 directionSpecularity = lightColor * lambert * Dirrfresnel * distribution * visibility;
		directionSpecularity += lightColor * lambert * Dirrfresnel * distribution * visibility * myPointLights[i].intensity * lightRange;



	}



	PixelOutput output;


	//output.color.xyz = normal.xyz;
	//output.color.a = 1.0f;
	output.color = float4(ambientDiffuse + ambientSpecularity + directionDiffuse + directionSpecularity + emissive, 1.0f);
	//output.color.rgb = shadowCamPosition.xyz;
	return output;






		//ye Olde way
		//{
		//    float3 normal = PS_ObjectNormal(input).color.xyz;
		//
		//
		//    float3 emissive = PS_Emissive(input).color.xyz;
		//    float3 ambientDiffuse = PS_AmbientDiffuse(input).color.xyz;
		//    float3 ambientSpecularity = PS_AmbientSpecularity(input).color.xyz;
		//
		//    float3 directionDiffuse = PS_DirectDiffuse(input, normal, myDirectionalLight.color.rgb, myDirectionalLight.direction.xyz).color.xyz;
		//    float3 directionSpecularity = PS_DirectSpecularity(input, normal, myDirectionalLight.color.rgb, myDirectionalLight.direction.xyz).color.xyz;
		//
		//    for(int i = 0; i < NUMBER_POINTLIGHTS; ++i)
		//    {
		//        float3 difference = (myPointLights[i].position.xyz) - input.worldPosition.xyz;
		//        float l = length(difference);
		//
		//        float lightRange = myPointLights[i].range / l;
		//        lightRange = saturate(lightRange);
		//
		//        float3 direction = normalize(difference);
		//        directionDiffuse += (PS_DirectDiffuse(input, normal, myPointLights[i].color.rgb, -direction).color.xyz) * myPointLights[i].intensity * lightRange;
		//        directionSpecularity += (PS_DirectSpecularity(input, normal,myPointLights[i].color.rgb, -direction).color.xyz) * myPointLights[i].intensity * lightRange;
		//
		//    }
		//
		//    PixelOutput output;
		//    output.color = float4(ambientDiffuse + ambientSpecularity + directionDiffuse + directionSpecularity + emissive, 1.0f);
		//    ///output.color = float4(directionSpecularity, 1.0f);
		//    return output;
		//  }
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
