struct VertexInput
{
	float4 position : POSITION;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv : TEX_COORD0;
};

cbuffer BarVertexBuffer : register(b1)
{
	float2 position;
	float2 size;
}

cbuffer BarPixelBuffer : register(b2)
{
	float4 fullColour;
	float4 emptyColour;
	float4 backgroundColour;
	float state;
	float3 TRASH;
}

PixelInput VS_Bar(VertexInput input)
{
	float2 outPosition = input.position.xy;
	
	outPosition *= size;

	outPosition.x = outPosition.x + position.x;
	outPosition.y = outPosition.y + position.y;

	outPosition = outPosition * 2.f;
	outPosition = outPosition - float2(1.f, 1.f);

	PixelInput output;
	output.position = float4(outPosition.xy, float2(0.5f, 1.f));

	output.uv.xy = input.position.xy;

	return output;
}

float4 PS_Bar(PixelInput input) : SV_TARGET
{
	float4 outColour;

	if(state >= input.uv.x)
	{
		float4 barColour = lerp(emptyColour, fullColour, state);
		outColour = barColour;
	}
	else
	{
		outColour = backgroundColour;
	}

	return outColour;
}