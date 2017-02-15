struct VertexInput
{
	float4 position : POSITION;
	float4 normals : NORMALS;
	float4 tangent : TANGENT;
	float4 biTangent : BITANGENT;
	float2 uv : UV;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normals : NORMALS;
	float4 tangent : TANGENT;
	float4 biTangent : BITANGENT;
	float4 worldPosition : WORLDPOSITION;
	float2 uv : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;

	float4x4 garbageMatrix1;
	float4x4 garbageMatrix2;
	
	float deltaTime;
	float time;

	float garbageFloat1;
	float garbageFloat2;
}

cbuffer ToWorld : register(b1)
{
	float4x4 worldSpace;
}

cbuffer GUIPixelBuffer : register(b1)
{
	float isMoneyBar;
	float moneyPercent;
	float skipEmissive;
	float flashButton;
	float isHealthBar;
	float healthPercent;
	float isManaBar;
	float manaPercent;

	float4 cameraPosition;
}

TextureCube cubeMap : register(t0);
Texture2D albedo : register(t1);
Texture2D roughness : register(t2);
Texture2D ambientOcclusion : register(t3);
Texture2D emissive : register(t4);
Texture2D normalmap : register(t6);
Texture2D metallic : register(t11);

SamplerState samplerStateClamp : register(s0);
SamplerState samplerStateWrap : register(s1);

float4 PS_PBL(PixelInput input);
float4 PS_Albedo(PixelInput input);
float3 PS_Emissive(PixelInput input);
float4 PS_HealthBar(PixelInput input, float aProgressBarValue);
float4 PS_FlashButton(PixelInput input);
float4 PS_OrbShader(PixelInput input);
float4 PS_MoneyShader(PixelInput input);

static const uint CubeMap_MipCount = 11;

static struct Light
{
	float4 color;
	float4 direction;

} DirectionalLight;

PixelInput VS_PosNormBinormTanTex(VertexInput input)
{
	PixelInput output;
	output.position = input.position;

	output.position = mul(worldSpace, output.position);
	output.worldPosition = output.position;
	output.position = mul(cameraSpaceInversed, output.position);
	output.position = mul(projectionSpace, output.position);

	output.normals = input.normals;
	output.tangent = input.tangent;
	output.biTangent = input.biTangent;

	output.uv = input.uv;
	
	return output;
}

float4 PS_PosNormBinormTanTex(PixelInput input) : SV_TARGET
{
	float shouldDoPBL = (1.f - isMoneyBar) * (1.f - isManaBar) * (1.f - isHealthBar);

	float4 output = shouldDoPBL * PS_PBL(input);

	output += isManaBar * PS_OrbShader(input);
	output += isHealthBar * PS_OrbShader(input);
	//output += isMoneyBar ;
	output += isMoneyBar * PS_MoneyShader(input); // * float4(albedo.Sample(globalSamplerState, float2(input.uv.x, input.uv.y)).xyz, 1.f);
	
	return output;
}

float4 PS_HealthBar(PixelInput input, float aProgressBarValue)
{
	float Damage = 6.f * aProgressBarValue;

	float4 bGColor = float4(0.f, 0.f, 0.f, 1.f);
	float4 Sampler = albedo.Sample(samplerStateWrap, input.uv);
	float4 VectorConstruct = float4(Sampler.xyz.x, Sampler.xyz.y, Sampler.xyz.z, Sampler.w);
	float MulOp = (Damage * 0.1667);
	float MulOp1374 = (input.uv.x * 5.333);
	float FloorOp = floor((MulOp + MulOp1374));
	float SatOp = saturate((1.0 - FloorOp));
	float4 LerpOp = lerp(bGColor, VectorConstruct, SatOp);
	float4 VectorConstruct1384 = float4(LerpOp.xyz.x, LerpOp.xyz.y, LerpOp.xyz.z, 1.0);

	return VectorConstruct1384;
}

float4 PS_FlashButton(PixelInput input)
{
	float4 flashColor = float4(1.f, 1.f, 1.f, 1.f);
	flashColor *= 0.2f;
	flashColor *= flashButton;
	return flashColor;
}

float3 PS_Lambert(PixelInput input, float3 aNormal, float3 aDirection)
{
	float3 output = saturate(dot(aNormal, -aDirection));
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

float4 PS_Albedo(PixelInput input)
{
	float4 output = albedo.Sample(samplerStateWrap, input.uv);
	return output;
}

float3 PS_Roughness(PixelInput input)
{
	float3 output = roughness.Sample(samplerStateWrap, input.uv).xyz;
	return output;
}

float3 PS_AmbientOcclusion(PixelInput input)
{
	float3 output = ambientOcclusion.Sample(samplerStateWrap, input.uv).xyz;
	return output;
}

float3 PS_Emissive(PixelInput input)
{
	float3 output = emissive.Sample(samplerStateWrap, input.uv).xyz;
	return (1.f - skipEmissive) * output;
}

float3 PS_ObjectNormal(PixelInput input)
{
	float3 normal = normalmap.Sample(samplerStateWrap, input.uv).xyz;
	normal = (normal * 2.0f) - float3(1.0f, 1.0f, 1.0f);
	normal = normalize(normal);
	float3x3 tangentSpaceMatrix = float3x3(normalize(input.biTangent.xyz), normalize(input.tangent.xyz), normalize(input.normals.xyz));
	normal = mul(normal, tangentSpaceMatrix);

	return normal;
}

float3 PS_Metalness(PixelInput input)
{
	float3 output = metallic.Sample(samplerStateWrap, input.uv).xyz;
	return output;
}

float3 PS_Substance(PixelInput input)
{
	float3 albedo = PS_Albedo(input).xyz;
	float3 metalness = PS_Metalness(input);
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo * metalness;
	
	return substance;
}

float3 PS_ReflectionFresnel(PixelInput input)
{
	float roughness = PS_Roughness(input).x;
	float3 substance = PS_Substance(input);
	float3 normal = PS_ObjectNormal(input);
	float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
	float VdotN = dot(toEye.xyz, normal);

	VdotN = saturate(VdotN);
	VdotN = 1.0f - VdotN;
	VdotN = pow(VdotN, 5); //whaah

	float3 fresnel = VdotN * (1.f - substance);
	fresnel = fresnel / (6 - 5 * roughness);
	fresnel = substance + fresnel;

	return fresnel;
}

float3 PS_Fresnel(PixelInput input, float3 aDirection)
{
	float3 toLight = -aDirection;
	float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
	float3 substance = PS_Substance(input);
	float3 halfvec = normalize(toLight + toEye);

	float LdotH = dot(toLight, halfvec);
	LdotH = saturate(LdotH);
	LdotH = 1.0f - LdotH;
	LdotH = pow(LdotH, 5);

	float3 fresnel = LdotH * (1.f - substance);
	fresnel = substance + fresnel;

	return fresnel;
}

float3 PS_Distribution(PixelInput input, float3 aDirection)
{
	float3 toLight = -aDirection.xyz;

	float roughness = PS_Roughness(input).x;
	float3 normal = PS_ObjectNormal(input);
	float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
	float3 halfvec = normalize(toLight + toEye.xyz);
	float HdotN = saturate(dot(halfvec, normal));

	float m = roughness * roughness; // roughness ^2 or what?
	float m2 = m * m; //roughness ^4 or what?

	float denominator = HdotN * HdotN * (m2 - 1.0f) + 1.0f;
	float distribution = m2 / (3.1415926538f * denominator * denominator);

	return distribution.xxx;
}

float3 PS_Visibility(PixelInput input, float3 aDirection)
{
	float3 toLight = -aDirection.xyz;

	float roughness = PS_Roughness(input).x;
	float roughnessRemapped = (roughness + 1.0f) / 2.0f;
	float3 normal = PS_ObjectNormal(input);
	float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);

	float NdotL = saturate(dot(normal, toLight));
	float NdotV = saturate(dot(normal, toEye));

	float k = roughnessRemapped * roughnessRemapped * 1.7724f; //sqrt(PI) wth is K?
	float G1V = NdotV * (1.0f - k) + k;
	float G1L = NdotL * (1.0f - k) + k;
	float visibility = (NdotV * NdotL) / (G1V * G1L);

	return visibility.xxx;
}

float3 PS_MetalnessAlbedo(PixelInput input)
{
	float3 albedo = PS_Albedo(input).xyz;
	float3 metalness = PS_Metalness(input);
	float3 metalnessAlbedo = albedo - (albedo * metalness);

	return metalnessAlbedo.xyz;
}

float3 PS_AmbientDiffuse(PixelInput input)
{
	float3 normal = PS_ObjectNormal(input);
	float3 metalnessAlbedo = PS_MetalnessAlbedo(input);
	float3 ambientOcclusion = PS_AmbientOcclusion(input);

	float3 ambientLight = cubeMap.SampleLevel(samplerStateWrap, normal.xyz, (uint) CubeMap_MipCount.x - 2).xyz; // FIX!
	float3 fresnel = PS_ReflectionFresnel(input);

	float3 output = metalnessAlbedo * ambientLight * ambientOcclusion * (float3(1.0f, 1.0f, 1.0f) - fresnel);
	return output;
}

float3 PS_AmbientSpecularity(PixelInput input)
{
	float3 normal = PS_ObjectNormal(input);
	float roughness = PS_Roughness(input).x;
	float3 ambientOcclusion = PS_AmbientOcclusion(input).xxx;

	float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
	float3 reflectionVector = -reflect(toEye, normal);

	float fakeLysSpecularPower = RoughToSPow(roughness);
	float lysMipMap = GetSpecPowToMip(fakeLysSpecularPower, (uint) CubeMap_MipCount.x);

	float3 ambientLight = cubeMap.SampleLevel(samplerStateWrap, reflectionVector.xyz, lysMipMap).xyz;
	float3 fresnel = PS_ReflectionFresnel(input).xyz;

	float3 output = ambientLight * ambientOcclusion * fresnel;
	return output;
}

float3 PS_DirectDiffuse(PixelInput input, float3 normal, float3 aLightColor, float3 aLightDirection)
{
	float3 lightColor = aLightColor.xyz;

	float3 metalnessAlbedo = PS_MetalnessAlbedo(input);

	float3 lambert = PS_Lambert(input, normal, aLightDirection);
	float3 fresnel = PS_Fresnel(input, aLightDirection);

	float3 one = float3(1.0f, 1.0f, 1.0f);
	float3 output = metalnessAlbedo * lightColor * lambert * (one - fresnel);
	return output;
}

float3 PS_DirectSpecularity(PixelInput input, float3 normal, float3 aLightColor, float3 aLightDirection)
{
	float3 lightColor = aLightColor.xyz;

	float3 lambert = PS_Lambert(input, normal, aLightDirection);
	float3 fresnel = PS_Fresnel(input, aLightDirection);
	float3 distribution = PS_Distribution(input, aLightDirection).xxx;
	float3 visibility = PS_Visibility(input, aLightDirection).xxx;

	float3 output = lightColor * lambert * fresnel * distribution * visibility;
	return output;
}

float4 PS_PBL(PixelInput input)
{
	float3 normal = PS_ObjectNormal(input);
	float3 emissive = PS_Emissive(input);
	float4 albedo = PS_Albedo(input);
	float3 metalness = PS_Metalness(input);
	float3 metalnessAlbedo = albedo.xyz - (albedo.xyz * metalness);
	float3 ambientOcclusion = PS_AmbientOcclusion(input);
	float3 ambientLight = cubeMap.SampleLevel(samplerStateWrap, normal.xyz, (uint) CubeMap_MipCount.x - 2).xyz; // FIX!
	float roughness = PS_Roughness(input).x;
	float3 substance = (float3(0.04f, 0.04f, 0.04f) - (float3(0.04f, 0.04f, 0.04f) * metalness)) + albedo.xyz * metalness;
	float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
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
	float3 ambientLightSpec = cubeMap.SampleLevel(samplerStateWrap, reflectionVector.xyz, lysMipMap).xyz;
	float3 ambientSpecularity = ambientLightSpec * ambientOcclusion * fresnel;


	//DIRR DIFF

	DirectionalLight.direction = normalize(float4(-0.5f, -1.f, 0.f, 0.f));
	DirectionalLight.color = float4(1.f, 1.f, 1.f, 1.f);

	float3 lightColor = DirectionalLight.color.rgb;
	float3 lambert = PS_Lambert(input, normal, DirectionalLight.direction.xyz);
	float3 toLight = -DirectionalLight.direction.xyz;
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

	lightColor = DirectionalLight.color.rgb;
	float3 distribution = PS_Distribution(input, DirectionalLight.direction.xyz).xxx;
	float3 visibility = PS_Visibility(input, DirectionalLight.direction.xyz).xxx;
	float3 directionSpecularity = lightColor * lambert * Dirrfresnel * distribution * visibility;

	float4 output = float4(ambientDiffuse + ambientSpecularity + directionDiffuse + directionSpecularity + emissive, 1.f);
	output.a = albedo.a;
	return output;
}

float4 PS_OrbShader(PixelInput input)
{
	static const float3 manaColor2 = { 0.0, 0.0775376, 0.672 };
	static const float3 manaColor1 = { 0.0, 1.0, 0.923067 };
	
	static const float3 healthColor2 = { 0.292, 0.019, 0.303 };
	static const float3 healthColor1 = { 0.718, 0.042, 0.146 };

	static const float bubbleTimeFactor = 0.15f;
	static const float waveTimeFactor = -0.1f;

	float stars = isManaBar;
	float orbValue = manaPercent + healthPercent;

	float3 orbColor1 = isManaBar * manaColor1 + isHealthBar * healthColor1;
	float3 orbColor2 = isManaBar * manaColor2 + isHealthBar * healthColor2;

	float SubOp = (stars - 0.5);
	float AddOp = ((SubOp * 0.5) + input.uv.xy.x);
	float MulOp = (time * bubbleTimeFactor);
	float SubOp98 = (input.uv.xy.y - MulOp);
	float2 VectorConstruct = float2(AddOp, SubOp98);
	float4 Sampler = albedo.Sample(samplerStateWrap, float2(VectorConstruct.xy.x, 1 - VectorConstruct.xy.y));
	float4 Sampler100 = albedo.Sample(samplerStateWrap, float2(input.uv.xy.x, 1 - input.uv.xy.y));
	float AddOp137 = ((Sampler.xyz.z * 0.3) + Sampler100.xyz.y);
	float MulOp64 = (time * waveTimeFactor);
	float SubOp66 = (MulOp64 - input.uv.xy.x);
	float SinOp = sin((time * 15.0));
	float AddOp152 = ((SinOp * 0.001) + input.uv.xy.y);
	float MulOp126 = (orbValue * 0.5);
	float SubOp124 = ((AddOp152 + 0.25) - MulOp126);
	float2 VectorConstruct63 = float2(SubOp66, SubOp124);
	float4 Sampler36 = albedo.Sample(samplerStateWrap, float2(VectorConstruct63.xy.x, 1 - VectorConstruct63.xy.y));
	float SubOp128 = (Sampler36.xyz.x - (1.0 - Sampler100.w));
	float SatOp = saturate(SubOp128);
	float3 MulOp60 = (lerp(orbColor2.xyz, orbColor1.xyz, AddOp137) * SatOp);
	float4 VectorConstruct61 = float4(MulOp60.x, MulOp60.y, MulOp60.z, SatOp);
	
	return VectorConstruct61;
}

float4 PS_MoneyShader(PixelInput input)
{
	float moneyLevel = moneyPercent;

	float2 moneyImage = float2(input.uv.xy.x, ( input.uv.xy.y + 0.25 ) );
	float4 Sampler = albedo.Sample(samplerStateWrap, moneyImage);
	
	float uvShiftTop = (input.uv.xy.x + moneyLevel);

	float2 siluetteImage = float2(uvShiftTop, (input.uv.xy.y + 0.5) + (moneyLevel * 0.25));
	
	float4 Sampler68 = albedo.Sample(samplerStateWrap, siluetteImage);

	float3 MulOp = (Sampler.xyz * Sampler68.xyz.z);
	float4 VectorConstruct59 = float4(MulOp.x, MulOp.y, MulOp.z, Sampler68.xyz.z);
	
	float4 output = VectorConstruct59;
	return output;
}