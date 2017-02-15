

SamplerState samplerClamp : register(s0);
SamplerState samplerWrap : register(s1);

Texture2D fireTexture : register(t1);
Texture2D noiseTexture : register(t2);
Texture2D alphaTexture : register(t3);

cbuffer PerFrameBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;

	float deltaTime;
	float time;

	float fogStart;
	float fogEnd;
}

cbuffer MatrixBuffer : register(b1)
{
	float4x4 worldMatrix;
};

cbuffer NoiseBuffer : register(b2)
{
	float frameTime;
	float3 scrollSpeeds;
	float3 scales;
	float padding;
};

cbuffer DistortionBuffer : register(b1)
{
	float2 distortion1;
	float2 distortion2;
	float2 distortion3;
	float distortionScale;
	float distortionBias;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : UV;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoords[3] : TEXCOORD1;
};

PixelInputType VS_Fire(VertexInputType input)
{
	PixelInputType output;
	output.position = input.position;

	output.position = mul(worldMatrix, output.position);
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);

	output.tex = input.tex;

	[unroll] for (uint i = 0; i < 3; ++i)
	{
		output.texCoords[i] = (input.tex * scales[i]);
		output.texCoords[i].y = output.texCoords[i].y + (frameTime * scrollSpeeds[i]);
	}

	return output;
}

float4 PS_Fire(PixelInputType input) : SV_TARGET
{
	float2 noises[3];
	[unroll] for (uint i = 0; i < 3; ++i)
	{
		noises[i] = noiseTexture.Sample(samplerWrap, input.texCoords[i]).xy;
		noises[i] = (noises[i] - 0.5f) * 2.0f;
	}

	noises[0].xy = noises[0].xy * distortion1.xy;
	noises[1].xy = noises[1].xy * distortion2.xy;
	noises[2].xy = noises[2].xy * distortion3.xy;

	float2 finalNoise = noises[0] + noises[1] + noises[2];

	float perturb = ((1.0f - input.tex.y) * distortionScale) + distortionBias;

	float2 noiseCoords = (finalNoise.xy * perturb) + input.tex;

	float4 fireColor = fireTexture.Sample(samplerClamp, noiseCoords);

	float4 alphaColor = alphaTexture.Sample(samplerClamp, noiseCoords);

	fireColor.a = alphaColor.r; //DON'T KNOW WHAT I'M DOING HERE MVH CARL

	return fireColor;
}
