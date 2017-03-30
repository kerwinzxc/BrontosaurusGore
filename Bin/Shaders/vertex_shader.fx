#include <shader_structs.fx>

cbuffer ConstantBuffer : register( b0 ) //to vertex
{
	float4x4 cameraSpaceInversed;
	float4x4 projectionSpace;
}

cbuffer ToWorld : register(b1)
{
	float4x4 worldSpace;
	float4x4 worldSpaceLastFrame;
}

cbuffer AnimationBuffer : register(b3)
{
	float4x4 Bones[64];
}

//VERTEX SHADER

Pos_InputPixel VS_Pos(Pos_InputVertex input)
{
	Pos_InputPixel output;
	output.position = input.position;
	output.position = mul(worldSpace, output.position);
	output.position = mul(cameraSpaceInversed, output.position);
	output.position = mul(projectionSpace, output.position);

	return output;
}

PosNormBinormTanTex_InputPixel VS_PosNormBinormTanTex(PosNormBinormTanTex_InputVertex input)
{
	PosNormBinormTanTex_InputPixel output;
	float3x3 rotation = (float3x3)worldSpace;

	output.normals = float4(mul(rotation, normalize(input.normals.xyz)), 1.0f);
	output.tangent = float4(mul(rotation, normalize(input.tangent.xyz)), 1.0f);//input.tangent;
	output.biTangent = float4(mul(rotation, normalize(input.biTangent.xyz)), 1.0f);//input.biTangent;
	output.uv = input.uv;

	output.position = input.position;
	output.worldPosLastFrame = mul(worldSpaceLastFrame, output.position);

	output.position = mul(worldSpace, output.position);
	output.worldPosition = float4(output.position.xyz, 1.0f);

	output.position = mul(cameraSpaceInversed, output.position);
	output.viewPosition = float4(output.position.xyz, 1.0f);
	output.position = mul(projectionSpace ,output.position);

	output.worldPosLastFrame = mul(cameraSpaceInversed, output.worldPosLastFrame);

	return output;
}

PosNormBinormTanTex_InputPixel VS_PosNormBinormTanTexInstanced(PosNormBinormTanTexInstanced_InputVertex input)
{
	PosNormBinormTanTex_InputPixel output;
	float3x3 rotation = (float3x3) input.toWorldInstance;

	output.normals = float4(mul(rotation, normalize(input.normals.xyz)), 1.0f);
	output.tangent = float4(mul(rotation, normalize(input.tangent.xyz)), 1.0f); //input.tangent;
	output.biTangent = float4(mul(rotation, normalize(input.biTangent.xyz)), 1.0f); //input.biTangent;
	output.uv = input.uv;

	output.position = input.position;
	output.worldPosLastFrame = mul(input.toWorldLastFrameInstance, output.position);

	output.position = mul(input.toWorldInstance, output.position);
	output.worldPosition = float4(output.position.xyz, 1.0f);

	output.position = mul(cameraSpaceInversed, output.position);
	output.viewPosition = float4(output.position.xyz, 1.0f);
	output.position = mul(projectionSpace, output.position);

	output.worldPosLastFrame = mul(cameraSpaceInversed, output.worldPosLastFrame);

	return output;

}

PosNormBinormTanTex_InputPixel VS_PosNormBinormTanTexBones(PosNormBinormTanTexBones_InputVertex input)
{
	PosNormBinormTanTex_InputPixel output;
	float3x3 rotation = (float3x3) worldSpace;

	//output.normals = float4(mul(rotation, normalize(input.normals.xyz)), 1.0f);
	//output.tangent = float4(mul(rotation, normalize(input.tangent.xyz)), 1.0f); //input.tangent;
	//output.biTangent = float4(mul(rotation, normalize(input.biTangent.xyz)), 1.0f); //input.biTangent;
	output.uv = input.uv;

	output.position = input.position;
	output.worldPosLastFrame = mul(worldSpaceLastFrame, output.position);

	output.position = mul(worldSpace, output.position);
	output.worldPosition = float4(output.position.xyz, 1.0f);

	float4 weights = input.weights;
	uint4 bones = uint4((uint) input.boneIDs.x, (uint) input.boneIDs.y, (uint) input.boneIDs.z, (uint) input.boneIDs.w);

	float4x4 finalMatrix = (float4x4)0;
	finalMatrix = mul(Bones[bones.x], weights.x);
	finalMatrix += mul(Bones[bones.y], weights.y);
	finalMatrix += mul(Bones[bones.z], weights.z);
	finalMatrix += mul(Bones[bones.w], weights.w);


	float4 animatedPosition = mul(finalMatrix, float4(input.position.xyz, 1.f));
	output.position = mul(worldSpace, animatedPosition);

	float4 animatedNormal = float4(mul((float3x3)finalMatrix, input.normals.xyz).xyz, 1.f);
	output.normals = float4(mul(rotation, animatedNormal.xyz).xyz, 1.f);
	float4 animatedTangent = float4(mul((float3x3)finalMatrix, input.tangent.xyz).xyz, 1.f);
	output.tangent = float4(mul(rotation, animatedTangent.xyz).xyz, 1.f);
	float4 animatedBiTangent = float4(mul((float3x3)finalMatrix, input.biTangent.xyz).xyz, 1.f);
	output.biTangent = float4(mul(rotation, animatedBiTangent.xyz).xyz, 1.f);




	output.worldPosLastFrame = mul(cameraSpaceInversed, output.worldPosLastFrame);
	output.position = mul(cameraSpaceInversed, output.position);
	output.viewPosition = float4(output.position.xyz, 1.0f);
	output.position = mul(projectionSpace, output.position);

	return output;
}
