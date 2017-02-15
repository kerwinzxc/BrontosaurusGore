struct VertexInput
{
	float4 position : POSITION;
};

struct PixelInput
{
	float4 position : SV_POSITION;
};

PixelInput VS_Pos(VertexInput input)
{
	PixelInput output;
	output.position = input.position;

	output.position.x = output.position.x * 2.f - 1.f;
	output.position.y = 1.f - output.position.y * 2.f;
	
	output.position.z = 0.f;
	output.position.w = 1.f;
	
	return output;
}

float4 PS_Pos(PixelInput input) : SV_TARGET
{
	return float4(1.f, 0.f, 0.f, 1.f);
}
