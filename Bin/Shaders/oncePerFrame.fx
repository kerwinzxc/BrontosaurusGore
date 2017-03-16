
//**********************************************//
//					C-BUFFER					//
//**********************************************//

cbuffer OncePerFrame			: register(b0)
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;

	float4x4 shadowCamInverse;
	float4x4 shadowCamProjection;

	float deltaTime;
	float time; 

	float fogStart;
	float fogEnd;

	uint shadowCount;

	uint2 windowSize;
	float garbage;
}
