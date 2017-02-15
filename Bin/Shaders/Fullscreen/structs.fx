
struct PosTex_InputVertex
{
    float4 position : POSITION;
    float2 tex : UV;
};

struct PosTex_InputPixel
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};
