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
struct SVertexDataCube
{
	CU::Vector4f position;
};

struct SCameraViewProjection
{
	CU::Matrix44f myCameraSpaceInverse;
	CU::Matrix44f myProjectionSpace;
	CU::Matrix44f myCameraSpace;
	CU::Matrix44f myProjectionInverse;
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
};

struct SPointLightBuffer
{
	CU::Vector4f cameraPos;
	Lights::SDirectionalLight directionalLight;
	Lights::SPointLight pointLight;
	unsigned int cubemapMipcount;
	CU::Vector3f crap;
};

struct SAnimationBoneStruct 
{
	CU::Matrix44f boneMatrices[32];
};

struct SRenderModelParams
{
	CU::Matrix44f myTransform;
	CU::Matrix44f myTransformLastFrame;/*
	Lights::SDirectionalLight myDirectionalLight;
	CU::VectorOnStack<Lights::SPointLight, 8> myPointLightList;*/
	unsigned char myNumLights;

	const char* aAnimationState = nullptr;
	float aAnimationTime = 0.0f;
	bool aAnimationLooping = true;
	float aHighlightIntencity = 0.f;

	bool myRenderToDepth = false;
};
