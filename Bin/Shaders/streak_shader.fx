Texture2D sprite : register (t1);
SamplerState Sampler;

cbuffer ConstantBuffer : register( b0 ) //to vertex & geometry
{
    float4x4 cameraSpaceInversed;
    float4x4 projectionSpace;
	float4 cameraPosition;
}

cbuffer ToWorld : register(b1)
{
    float4x4 worldSpace;
}


struct InputVertex
{
    float4 position : POSITION;
    float size : SIZE;
    float alpha : ALPHA;
};
//missalligned?
struct InputPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float alpha : ALPHA;
};

struct InputGeometry
{
    float4 position : SV_POSITION;
    float size : SIZE;
    float alpha : ALPHA;
};


InputGeometry VS_PosSizeAlpha(InputVertex input)
{
    InputGeometry output = (InputGeometry)0;

    output.position = input.position;
    //output.position = mul(worldSpace, output.position);
    output.position = mul(cameraSpaceInversed, output.position);

    output.size = input.size;
    output.alpha = input.alpha;

    return output;
}

float4 PS_PosSizeAlpha(InputPixel input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, input.alpha);
}

float4 PS_PosTex(InputPixel input) : SV_TARGET
{
    float4 color = sprite.Sample(Sampler, input.uv);
    return float4(color.rgb, color.a * input.alpha);
}


[maxvertexcount(4)]
void GS_PosSizeAlpha(lineadj InputGeometry aInput[4], inout TriangleStream<InputPixel> triangleStream)
{
	float3 normals[2] = { float3(0.f, 0.f, 0.f), float3(0.f, 0.f, 0.f) };

	float3 lineDirection1 = aInput[2].position.xyz - aInput[0].position.xyz;
	lineDirection1 = normalize(lineDirection1);
	float3 toEye1 = -aInput[1].position.xyz;
	toEye1 = normalize(toEye1);

	normals[0] = normalize(cross(toEye1, lineDirection1));

	float3 lineDirection2 = normalize(aInput[3].position.xyz - aInput[1].position.xyz);

	float3 toEye2 = normalize(-aInput[2].position.xyz);

	normals[1] = normalize(cross(toEye2, lineDirection2));

	const float2 uv_coordinates[4] =
	{
		{ 0, 1 },
		{ 1, 1 },
		{ 0, 0 },
		{ 1, 0 }
	};

	const float inputIndex[4] =
	{
		1,
		2,
		1,
		2
	};

	const float normalFlipper[4] =
	{
		-1.f,
		-1.f,
		1.f,
		1.f
	};

	for (int i = 0; i < 4; ++i)
	{
		InputPixel vertex = (InputPixel) 0;
		vertex.position = float4(aInput[inputIndex[i]].position.xyz + normals[inputIndex[i] - 1] * aInput[inputIndex[i]].size * normalFlipper[i], 1.f);
		vertex.position = mul(projectionSpace, vertex.position);

		vertex.uv = uv_coordinates[i];
    vertex.alpha = aInput[0].alpha;
		triangleStream.Append(vertex);
	}

	triangleStream.RestartStrip();
}
