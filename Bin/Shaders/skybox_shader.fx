
TextureCube skybox : register(t0);
SamplerState Sampler;

cbuffer ConstantBuffer : register( b0 ) //to vertex & geometry
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;
}

cbuffer ToWorld : register(b1) // to vertex
{
	float4x4 worldSpace;
}

cbuffer PixelBuffer : register( b1 ) // to Pixel 
{
	float4 cameraPos;
}


struct InputVertex
{
	float4 position : POSITION; //camera position
};

struct InputPixel
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD_POS;
};

InputPixel VS_Pos(InputVertex input)
{
	InputPixel output = (InputPixel)0;

	output.position = input.position;
	output.position = mul(worldSpace, output.position);
	output.worldPosition = output.position;
	output.position = mul(cameraSpaceInversed, output.position);
	output.position = mul(projectionSpace, output.position);

	return output;
}

float4 PS_Pos(InputPixel input) : SV_TARGET
{
	float3 toPixel = normalize(input.worldPosition.xyz - cameraPos.xyz);
	float3 ambientLight = skybox.SampleLevel(Sampler, toPixel.xyz, 0).xyz;

	return float4(ambientLight, 1.0f);
}
