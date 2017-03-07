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

PixelOutput PS_PosTex(PosNormBinormTanTex_InputPixel input)
{
	PixelOutput output;
	float alpha = diffuse.Sample(samplerWrap, input.uv).a;
	if (alpha <= ALPHA_THRESHOLD)
	{
		discard;
	}
	output.shadow = float4(1.0f, 1.0f, 1.0f, 1.0f);
	output.shadow.xyz = input.position.zzz;
	output.shadow /= input.position.w;
	return output;
}
