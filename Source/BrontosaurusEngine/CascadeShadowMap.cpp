#include "stdafx.h"
#include <limits>
#include <climits>

#include "CascadeShadowMap.h"
#include "ModelInstance.h"
#include "RenderMessages.h"
#include "Engine.h"
#include "Renderer.h"
#include "..\CommonUtilities\Sphere.h"

#define SHADOWMAP_SIZE 2048u
#define SHADOWBUFFER_SIZE 1024u
#define LAMBDA 0.8f // Strenght of correction
#define NUM_CASCADES 4
#define NUM_FRUSTUM_CORNERS 8 // I guess, shuld it ever be something else?

#ifndef TO_RADIAN
#define TO_RADIAN(DEGREE) ((3.1415/180)  * DEGREE)
#endif

CCascadeShadowMap::CCascadeShadowMap(const int /*aNumOfCascades*/, const float aNear, const float aFar)
{
	myCurrentCascade = 0;

	myRenderCamera.InitOrthographic(32.f , 32.f, aFar, aNear, SHADOWBUFFER_SIZE, SHADOWBUFFER_SIZE, nullptr, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT);
	myRenderCamera.ShadowInit();

	CU::Vector2ui size = { SHADOWMAP_SIZE, SHADOWMAP_SIZE };
	myShadowMap.Init(size, nullptr, DXGI_FORMAT_R32_FLOAT);

	myNumCascades = NUM_CASCADES;
	myDirection = { 0.5f, 0.5f, 0.0f };
	
	myCascadeBuffer.myCascades[0].myRect = { 0.0f, 0.0f, 0.5f, 0.5f };
	myCascadeBuffer.myCascades[1].myRect = { 0.5f, 0.0f, 1.0f, 0.5f };
	myCascadeBuffer.myCascades[2].myRect = { 0.0f, 0.5f, 0.5f, 1.0f };
	myCascadeBuffer.myCascades[3].myRect = { 0.5f, 0.5f, 1.0f, 1.0f };

	const float ends[] =
	{
		aNear,
		aFar / 16.0f,
		aFar / 8.0f,
		aFar / 2.0f,
		aFar
	};

	//for more information myNumCascades this wierd ass equation http://developer.download.nvidia.com/SDK/10/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
	for (int i = 0; i < myNumCascades + 1; ++i)
	{
		float end = LAMBDA * aNear * (pow(abs(aFar / aNear), ((float)i / (float)myNumCascades)));
		end += (1.0f - LAMBDA) * (aNear + ((float)i / (float)myNumCascades) * (aFar - aNear));
		myCascadeBuffer.myCascadeEnds[i] = ends[i];// end;
		DL_PRINT("Frustum Sclice %d: %f", i, end);
	}
}

CCascadeShadowMap::~CCascadeShadowMap()
{
}

void CCascadeShadowMap::ComputeShadowProjection(const CU::Camera& aCamera)
{
	myCurrentCascade++;
	myCurrentCascade %= myNumCascades;

	CU::Matrix44f projection = aCamera.GetProjection();
	CU::Matrix44f toWorld = aCamera.GetTransformation();

	CU::Matrix44f lightSpace;
	CU::Vector3f shadowCameraPosition = aCamera.GetPosition() + (-myDirection * 10);
	lightSpace.SetPosition(shadowCameraPosition);
	lightSpace.LookAt(aCamera.GetPosition());

	float aspectRatio = aCamera.GetProjectionHeight() / aCamera.GetProjectionWidth();
	float tanHalfHFOV = tanf(TO_RADIAN(aCamera.GetFOV() / 2.0f));
	float tanHalfVFOV = tanf(TO_RADIAN((aCamera.GetFOV() * aspectRatio) / 2.0f));

	//for (unsigned int i = 0; i < NUM_CASCADES; ++i)
	{
		float xn = myCascadeBuffer.myCascadeEnds[myCurrentCascade]		* tanHalfHFOV;
		float xf = myCascadeBuffer.myCascadeEnds[myCurrentCascade + 1]	* tanHalfHFOV;
		float yn = myCascadeBuffer.myCascadeEnds[myCurrentCascade]		* tanHalfVFOV;
		float yf = myCascadeBuffer.myCascadeEnds[myCurrentCascade + 1]	* tanHalfVFOV;

		CU::Vector4f frustumCorners[NUM_FRUSTUM_CORNERS] =
		{
			//near face
			CU::Vector4f(xn,  yn,  myCascadeBuffer.myCascadeEnds[myCurrentCascade], 1.0f),
			CU::Vector4f(-xn,  yn, myCascadeBuffer.myCascadeEnds[myCurrentCascade], 1.0f),
			CU::Vector4f(xn, -yn,  myCascadeBuffer.myCascadeEnds[myCurrentCascade], 1.0f),
			CU::Vector4f(-xn, -yn, myCascadeBuffer.myCascadeEnds[myCurrentCascade], 1.0f),

			//far face
			CU::Vector4f(xf,  yf,  myCascadeBuffer.myCascadeEnds[myCurrentCascade + 1], 1.0f),
			CU::Vector4f(-xf,  yf, myCascadeBuffer.myCascadeEnds[myCurrentCascade + 1], 1.0f),
			CU::Vector4f(xf, -yf,  myCascadeBuffer.myCascadeEnds[myCurrentCascade + 1], 1.0f),
			CU::Vector4f(-xf, -yf, myCascadeBuffer.myCascadeEnds[myCurrentCascade + 1], 1.0f)
		};

		CU::Vector3f lightViewPos;
		lightViewPos.z = (myCascadeBuffer.myCascadeEnds[myCurrentCascade + 1] - myCascadeBuffer.myCascadeEnds[myCurrentCascade]) / 2.f;
		lightViewPos.z += myCascadeBuffer.myCascadeEnds[myCurrentCascade];

		lightViewPos = lightViewPos * toWorld;
		lightSpace.SetPosition(lightViewPos);


		CU::Vector4f frustumCornersL[NUM_FRUSTUM_CORNERS];

		float maxX = FLT_MIN;
		float maxY = FLT_MIN;
		float maxZ = FLT_MIN;
		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float minZ = FLT_MAX;

		CU::Matrix44f LightM = lightSpace.GetInverted();

		float radiusSquared = 0.0f;

		for (unsigned int j = 0; j < NUM_FRUSTUM_CORNERS; j++) 
		{
			CU::Vector4f frustumWorldPosition = frustumCorners[j] * toWorld;
			CU::Vector3f difference(frustumWorldPosition - toWorld.myPosition4);

			// Transform the frustum coordinate from world to light space
			frustumCornersL[j] = frustumWorldPosition * LightM;

			minX = min(minX, frustumCornersL[j].x);
			maxX = max(maxX, frustumCornersL[j].x);
			minY = min(minY, frustumCornersL[j].y);
			maxY = max(maxY, frustumCornersL[j].y);
			minZ = min(minZ, frustumCornersL[j].z);
			maxZ = max(maxZ, frustumCornersL[j].z);

			if (difference.Length2() > radiusSquared)
			{
				radiusSquared = difference.Length2();
			}
		}
		radiusSquared = sqrt(radiusSquared);
		myCascadeBuffer.myCascades[myCurrentCascade].myTransformation = lightSpace;
		myCascadeBuffer.myCascades[myCurrentCascade].InitProjection(
			-radiusSquared, radiusSquared,
			radiusSquared, -radiusSquared,
			radiusSquared * 2.0f, -radiusSquared * 2.0f);
	}
}

void CCascadeShadowMap::Render(const CU::GrowingArray<CModelInstance*, InstanceID>& aModelList)
{
	SChangeStatesMessage statemsg;

	//for (SCascade& cascade : myCascadeBuffer.myCascades)
	{
		statemsg.myRasterizerState = eRasterizerState::eNoCulling;
		statemsg.myDepthStencilState = eDepthStencilState::eDefault;
		statemsg.myBlendState = eBlendState::eNoBlend;
		statemsg.mySamplerState = eSamplerState::eClamp;
		RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
		myRenderCamera.GetCamera().ReInit(
			myCascadeBuffer.myCascades[myCurrentCascade].myOrthoProjection, 
			myCascadeBuffer.myCascades[myCurrentCascade].myTransformation);

		for (CModelInstance* modelInstance: aModelList)
		{
			if (modelInstance == nullptr || modelInstance->ShouldRender() == false)
			{
				continue;
			}

		/*	if (myRenderCamera.GetCamera().IsInside(modelInstance->GetModelBoundingSphere()) == false)
			{
				continue;
			}*/
			modelInstance->RenderDeferred(myRenderCamera);
		}
		myRenderCamera.AddRenderMessage(new SRenderModelBatches());
		myRenderCamera.Render();

		// DRAW SHADOWBUFFER
		statemsg.myBlendState = eBlendState::eNoBlend;
		statemsg.myRasterizerState = eRasterizerState::eDefault;
		statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
		statemsg.mySamplerState = eSamplerState::eClamp;

		RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));

		SActivateRenderPackageMessage * activateMsg = new SActivateRenderPackageMessage();
		activateMsg->myRenderPackage = myShadowMap;
		RENDERER.AddRenderMessage(activateMsg);

		SRenderFullscreenEffectMessage * msg = new SRenderFullscreenEffectMessage();
		msg->myRect = myCascadeBuffer.myCascades[myCurrentCascade].myRect;
		msg->myFirstPackage = myRenderCamera.GetRenderPackage();
		msg->myEffectType = CFullScreenHelper::eEffectType::eCopyR;
		msg->myFirstUseDepthResource = true;
		RENDERER.AddRenderMessage(msg);
		RENDERER.AddRenderMessage(new SActivateRenderToMessage());

	}

	SSetShadowBuffer *shadowMSG = new SSetShadowBuffer();
	shadowMSG->cascadeBuffer = myCascadeBuffer;
	//shadowMSG->myCameraProjection = myRenderCamera.GetCamera().GetProjection();
	//shadowMSG->myCameraTransformation = myRenderCamera.GetCamera().GetInverse();
	shadowMSG->myShadowBuffer = myShadowMap;
	//for (SCascade& cascade : shadowMSG->cascadeBuffer.myCascades)
	{
		myCascadeBuffer.myCascades[myCurrentCascade].myTransformation.Invert();
	}

	RENDERER.AddRenderMessage(shadowMSG);
	RENDERER.AddRenderMessage(new SActivateRenderToMessage());
}

const SCascade& CCascadeShadowMap::GetCascade(const int aIndex)
{
	return myCascadeBuffer.myCascades[aIndex];
}

CRenderPackage& CCascadeShadowMap::GetShadowMap()
{
	return myShadowMap;
}

