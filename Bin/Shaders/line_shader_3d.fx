struct VertexInput
{
	float4 position : POSITION;
};

struct PixelInput
{
	float4 position : SV_POSITION;
};

cbuffer CameraBuffer : register(b0) //to vertex
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;
	
	float4x4 garbage1;
	float4x4 garbage2;

	float garbage3;
	float garbage4;
	float garbage5;
	float garbage6;
}

PixelInput VS_Pos(VertexInput input)
{
	PixelInput output;

	output.position = input.position;
	output.position = mul(cameraSpaceInversed, output.position);
	output.position = mul(projectionSpace, output.position);
	
	return output;
}

float4 PS_Pos(PixelInput input) : SV_TARGET
{
	return float4(0.f, 1.f, 0.f, 1.f);
}
