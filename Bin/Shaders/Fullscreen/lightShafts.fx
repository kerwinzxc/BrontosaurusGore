Texture2D depthBuffer : register(t1);
SamplerState Sampler;

cbuffer ConstantBuffer : register( b0 ) //to vertex
{
    float4x4 cameraSpaceInversed;
    float4x4 projectionSpace;
}

cbuffer VertexShaderBuffer : register(b1)
{
    float3 LightPosition;
    float trash2;
}

struct PosTex_InputVertex
{
    float4 position : POSITION;
    float2 tex : UV;
};

struct PosTex_InputPixel
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float4 screenLightPosition : LIGHTPOS;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

PosTex_InputPixel VS_PosTex(PosTex_InputVertex input)
{
    float4 lightPos = float4(LightPosition.xyz, 1.0f);
	PosTex_InputPixel output;
	output.position = input.position;
    output.tex = input.tex;
    output.screenLightPosition = mul(lightPos, cameraSpaceInversed);
    output.screenLightPosition = mul(output.screenLightPosition, projectionSpace);

	return output;
}


float Density = 1.0f;
float Weight = 0.5f;
float Decay = 0.5f;
float Exposure = 1.0f;
static const int NUM_SAMPLES = 8;


//  lightShafts 
//
PixelOutput PS_PosTex(PosTex_InputPixel input)
{  
    float2 deltaTexCoord = (input.tex - input.screenLightPosition.xy);  
    deltaTexCoord *= Density / NUM_SAMPLES;  
    float4 color = depthBuffer.SampleLevel(Sampler, input.tex, 0);   
    float illuminationDecay = 1.0f;  

    for (int i = 0; i < NUM_SAMPLES; i++)  
    {  
        input.tex -= deltaTexCoord;  
        float4 sampleo = depthBuffer.SampleLevel(Sampler, input.tex, 0);  
        sampleo *= illuminationDecay * Weight;  
        color += sampleo;  
        illuminationDecay *= Decay;  
    }  

    PixelOutput output;
    output.color = color * Exposure;
    return output;
}  
