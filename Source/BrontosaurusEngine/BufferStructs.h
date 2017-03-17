#pragma once
#include "../CommonUtilities/vector2.h"
#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/vector4.h"
#include "../CommonUtilities/VectorOnStack.h"
#include "Lights.h"
//#include <vector2.h>
//#include <vector4.h>
//#include <matrix44.h>

//Legacy ?
//Size of a vertex can be found in the FBX loader model, along with all data and number of vertices. 

struct ID3D11PixelShader;

struct SVertexDataCube
{
	CU::Vector4f position;
};

struct SCameraViewProjection
{
	CU::Matrix44f myCameraSpaceInverse;
	CU::Matrix44f myProjectionSpace;
}; 

struct SToWorldSpace
{
	CU::Matrix44f myWorldSpace;
	CU::Matrix44f myWorldSpaceLastFrame;
};


struct SLODDistances
{
	SLODDistances() { LOD_DistStart2 = LOD_DistEnd2 = 0.f; }
	float LOD_DistStart2;
	float LOD_DistEnd2;
};


struct SOncePerFrameBuffer
{
	SCameraViewProjection myCameraMatrices;
	SCameraViewProjection myShadowCameraMatrices;

	float deltaTime;
	float time;

	float fogStart;
	float fogEnd;

	unsigned int shadows;

	CU::Vector2ui windowSize;
	float garbage[1];
};

struct SAnimationBoneStruct 
{
	CU::Matrix44f boneMatrices[32];
};

struct SForwardRenderModelParams
{
	CU::Matrix44f myTransform;
	CU::Matrix44f myTransformLastFrame;
	Lights::SDirectionalLight myDirectionalLight;
	CU::VectorOnStack<Lights::SPointLight, 8> myPointLightList;
	unsigned char myNumLights;

	std::string aAnimationState;
	float aAnimationTime = 0.0f;
	bool aAnimationLooping = true;
	float aHighlightIntencity = 0.f;

	bool myRenderToDepth = false;
};

struct SDeferredRenderModelParams
{
	CU::Matrix44f myTransform;
	CU::Matrix44f myTransformLastFrame;

	std::string aAnimationState;
	std::string aNextAnimationState;
	float aAnimationLerper = 0.0f;
	float aAnimationTime = 0.0f;
	bool aAnimationLooping = true;
	float aHighlightIntencity = 0.f;

	bool myRenderToDepth = false;
};

struct SShadowRenderModelParams
{
	CU::Matrix44f myTransform;
	CU::Matrix44f myTransformLastFrame;

	std::string aAnimationState;
	std::string aNextAnimationState;
	float aAnimationLerper = 0.0f;
	float aAnimationTime = 0.0f;
	bool aAnimationLooping = true;
	float aHighlightIntencity = 0.f;
	ID3D11PixelShader* aPixelshader;
};
