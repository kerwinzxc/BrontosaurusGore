#include "metaballsCommon.fx"

static const uint Segments = 24;
static const uint IterationIncrement = 3;
static const uint Iterations = Segments / IterationIncrement;

static const float Pi = 3.14159265f;
static const float RotationSteps = 2. * Pi * (1. / (Segments));

static const uint NumOfOutVerts = Iterations * 5;

float4 GetOffset(uint vertexNum)
{
	const float rotation = vertexNum * RotationSteps;
	return float4(sin(rotation), -cos(rotation), 0.f, 1.f);
}

InputPixel EdgeVertex(uint vertexNum, float radius, InputGeometry input, float4x4 rotation)
{
	InputPixel vertex = (InputPixel) 0;
	const float4 offset = GetOffset(vertexNum);
	float4 offsetPos = mul(rotation, offset * radius);
	offsetPos.w = 0.0f;
	vertex.position = input.position + offsetPos;
	vertex.worldPosition = vertex.position;
        //vertex.position = mul(rotationMatrix, vertex.position);
	vertex.position = mul(projectionSpace, vertex.position);
	vertex.screenPos = vertex.position.xyz / vertex.position.w;
	float3 center = input.position.xyz;
	vertex.uv = offset.xy;
	vertex.color = input.color;
	vertex.center = center.xyz;
	vertex.radius = input.size;

	return vertex;
}

InputPixel CenterVertex(InputGeometry input)
{
	InputPixel vertex = (InputPixel) 0;
	vertex.position = input.position;
	vertex.worldPosition = vertex.position;
        //vertex.position = mul(rotationMatrix, vertex.position);
	vertex.position = mul(projectionSpace, vertex.position);
	vertex.screenPos = vertex.position.xyz / vertex.position.w;
	float3 center = input.position.xyz;
	vertex.uv = float2(0.5,0.5);
	vertex.color = input.color;
	vertex.center = center.xyz;
	vertex.radius = input.size;

	return vertex;
}

[maxvertexcount(NumOfOutVerts)] //whaah //one input vertex gives max 4 new vertices in this case I think?
void GS_PosSizeColor(point InputGeometry input[1], inout TriangleStream<InputPixel> triStream)
{
    float4x4 rotationMatrix = (float4x4)0;
    rotationMatrix._11 = cos(input[0].rotation);
    rotationMatrix._12 = sin(input[0].rotation);
    rotationMatrix._21 = -sin(input[0].rotation);
    rotationMatrix._22 = cos(input[0].rotation);
    rotationMatrix._33 = 1.0f;
    rotationMatrix._44 = 1.0f;
	const float radius = input[0].size;
	uint lastVertex = 0;
	for (uint i = 0; i < Segments; i += IterationIncrement)
    {
        triStream.Append(EdgeVertex(i,radius,input[0],rotationMatrix));
		triStream.Append(CenterVertex(input[0]));
		triStream.Append(EdgeVertex(i + 1,radius, input[0], rotationMatrix));
		triStream.Append(EdgeVertex(i + 2,radius, input[0], rotationMatrix));
		lastVertex = i + 2;
	}
	triStream.Append(EdgeVertex(lastVertex + 1, radius, input[0], rotationMatrix));
	triStream.Append(CenterVertex(input[0]));

    triStream.RestartStrip();
}