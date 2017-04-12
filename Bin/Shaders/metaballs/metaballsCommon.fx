cbuffer ConstantBuffer : register( b0 ) //to vertex & geometry
{
    float4x4 cameraSpaceInversed;
    float4x4 projectionSpace;
}

cbuffer ToWorld : register(b1)
{
    float4x4 worldSpace;
}

struct InputVertex
{
    float4 position : POSITION;
    float size : SIZE;
    float4 color : COLOR;
};
//missalligned?
struct InputPixel
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    
    float3 center : CENTER;
    float radius : RADIUS;
    
    float2 uv : TEXCOORD;
    float3 screenPos : SCREEN_POS;
    float3 crap : CRAP;
};

struct InputGeometry
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 center : CENTER;
    float size : SIZE;
    float rotation : ROTATION;
};