#include <shader_structs.fx>
#include <oncePerFrame.fx>


//**********************************************//
//					TEXTURES					//
//**********************************************//

Texture2D diffuse               : register(t1);
Texture2D RMAO					: register(t3);
Texture2D emissive				: register(t4);
Texture2D normalMap				: register(t6);

//**********************************************//
//					SAMPLERS					//
//**********************************************//

SamplerState SamplerClamp		: register(s0);
SamplerState samplerWrap		: register(s1);

//**********************************************//
//					C-BUFFER					//
//**********************************************//

// once per frame buffer is defined in oncePerFrame.fx

//**********************************************//
//					G-BUFFER					//
//**********************************************//

struct PixelOutput
{
	float4 shadow				: SV_TARGET0;
};

//**********************************************//
//					FRAGMENT					//
//**********************************************//

float3 PS_ObjectNormal(PosNormBinormTanTex_InputPixel input);

PixelOutput PS_PosNormBinormTanTex(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;
	float alpha = diffuse.Sample(samplerWrap, input.uv).a;
	if (alpha <= ALPHA_THRESHOLD)
	{
		discard;
	}

	output.shadow.xyz = input.position.zzz;
	output.shadow.a = 1.0f;
	return output;
}


PixelOutput PS_PosNormBinormTanTexBones(PosNormBinormTanTex_InputPixel input)
{
	return PS_PosNormBinormTanTex(input);
}
