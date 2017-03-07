
//******************************//
//			TWEAKERS			//
//******************************//
#define ALPHA_THRESHOLD	0.30f

//******************************//
//			STRUCTS				//
//******************************//

struct Pos_InputVertex
{
    float4 position : POSITION;
};

struct Pos_InputPixel
{
    float4 position : SV_POSITION;
};

struct LightModel_InputPixel
{
	float4 position : SV_POSITION;
	float4 uv		: TEXCOORD;
};


struct PosNormBinormTanTex_InputVertex
{
    float4 position : POSITION;
    float4 normals : NORMALS;
    float4 tangent : TANGENT;
    float4 biTangent : BITANGENT;
    float2 uv : UV;
};

struct PosNormBinormTanTex_InputPixel
{
    float4 position : SV_POSITION;
    float4 viewPosition : VIEW_POS;
    float4 worldPosition : WORLD_POS;
    float4 worldPosLastFrame : WORLD_POS_LAST_FRAME;
    float4 normals : NORMALS;
    float4 tangent : TANGENT;
    float4 biTangent : BITANGENT;
    float2 uv : TEXCOORD;
};

struct PosNormBinormTanTexBones_InputVertex
{
	float4 position : POSITION;
	float4 normals : NORMALS;
	float4 tangent : TANGENT;
	float4 biTangent : BITANGENT;
	float2 uv : UV;
	float4 boneIDs : BONE;
	float4 weights : WEIGHT;
};
