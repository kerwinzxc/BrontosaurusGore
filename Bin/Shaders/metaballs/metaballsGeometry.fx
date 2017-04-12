#include "metaballsCommon.fx"

static const uint NumOfOutVerts = 4;
[maxvertexcount(NumOfOutVerts)] //whaah //one input vertex gives max 4 new vertices in this case I think?
void GS_PosSizeColor(point InputGeometry input[1], inout TriangleStream<InputPixel> triStream)
{
    const float correctedSize = input[0].size;
    const float4 offset[NumOfOutVerts] =
    {
        {-correctedSize, correctedSize, 0.0f, 1.0f},
        {correctedSize, correctedSize, 0.0f, 1.0f},
        {-correctedSize, -correctedSize, 0.0f, 1.0f},
        {correctedSize, -correctedSize, 0.0f, 1.0f}
    };

    const float2 uv_coord[4] =
    {
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f}
    };

    float4x4 rotationMatrix = (float4x4)0;
    rotationMatrix._11 = cos(input[0].rotation);
    rotationMatrix._12 = sin(input[0].rotation);
    rotationMatrix._21 = -sin(input[0].rotation);
    rotationMatrix._22 = cos(input[0].rotation);
    rotationMatrix._33 = 1.0f;
    rotationMatrix._44 = 1.0f;


    for(uint i = 0 ; i < NumOfOutVerts ; ++i)
    {
        InputPixel vertex = (InputPixel)0;
        float4 offsetPos = mul(rotationMatrix, offset[i]);
        offsetPos.w = 0.0f;
        vertex.position = input[0].position + offsetPos;
        vertex.worldPosition = vertex.position;
        //vertex.position = mul(rotationMatrix, vertex.position);
        vertex.position = mul(projectionSpace, vertex.position);
        vertex.screenPos = vertex.position.xyz / vertex.position.w;
        float3 center = input[0].position.xyz;
        vertex.uv = uv_coord[i % 4];
        vertex.color = input[0].color;
        vertex.center = center.xyz;
        vertex.radius = correctedSize;

        triStream.Append(vertex);
    }
    triStream.RestartStrip();
}