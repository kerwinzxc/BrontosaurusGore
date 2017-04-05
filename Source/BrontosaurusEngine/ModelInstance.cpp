#include "stdafx.h"
#include "ModelInstance.h"
#include "ModelManager.h"
#include "Model.h"
#include "Engine.h"
#include "Renderer.h"
#include "RenderCamera.h"

//Lights
#include "PointLightInstance.h"
#include "Intersection.h"
#define HIGH_ENUF 2

#include "../TShared/AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

CModelInstance::CModelInstance(const std::string& aModelPath)
{
	myCurrentAnimation = eAnimationState::idle01;
	myNextAnimation = eAnimationState::none;

	myModel = -1;

	myAnimationLerpie = 0.f;
	myAnimationCounter = 0.f;
	myHighlightIntencity = 0.f;

	myIsVisible = true;
	myHasAnimations = false;
	myAnimationLooping = true;
	myIgnoreDepth = false;

	if (aModelPath.find("lasma") != std::string::npos)
	{
		int br = 0;
	}

	CModelManager* modelManager = MODELMGR;
	myModel = modelManager->LoadModel(aModelPath);
	CModel* model = modelManager->GetModel(myModel);

	myHasAnimations = model != nullptr && model->HasAnimations();
}

CModelInstance::~CModelInstance()
{
	CModel* model = MODELMGR->GetModel(myModel);
	if (model)
	{
		model->RemoveRef();
		model = nullptr;
		myModel = NULL_MODEL;
	}
}

void CModelInstance::RenderForward(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList)
{
	if (ShouldRender() == true)
	{

		SRenderModelMessage* msg = new SRenderModelMessage();
		msg->myModelID = myModel;
		msg->myRenderParams.myTransform = myTransformation;
		msg->myRenderParams.myTransformLastFrame = myLastFrame;
		msg->myRenderParams.aHighlightIntencity = myHighlightIntencity;
		msg->myRenderParams.myRenderToDepth = false;

		msg->myRenderParams.myDirectionalLight = *aLight;
		msg->myRenderParams.myNumLights = 0;
		
		for (unsigned short i = 0; i < aPointLightList.Size(); ++i)
		{
			if (msg->myRenderParams.myPointLightList.SafeAdd(Lights::SPointLight()) == true)
			{
				msg->myRenderParams.myPointLightList.GetLast().color = aPointLightList[i].GetColor();
				msg->myRenderParams.myPointLightList.GetLast().intensity = aPointLightList[i].GetInstensity();
				msg->myRenderParams.myPointLightList.GetLast().position = aPointLightList[i].GetPosition();
				msg->myRenderParams.myPointLightList.GetLast().range = aPointLightList[i].GetRange();
				msg->myRenderParams.myNumLights++;
			}
		}

		
		if (myHasAnimations != false)
		{
			msg->myRenderParams.aAnimationTime = myAnimationCounter;
			msg->myRenderParams.aAnimationState = myCurrentAnimation;
			msg->myRenderParams.aAnimationLooping = myAnimationLooping;
		}

		RENDERER.AddRenderMessage(msg);

		myLastFrame = myTransformation;
	}
}

void CModelInstance::RenderForward(Lights::SDirectionalLight * aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList, CRenderCamera & aRenderToCamera)
{
	if (ShouldRender() == true)
	{
		SRenderModelMessage* msg = new SRenderModelMessage();
		msg->myModelID = myModel;

		msg->myRenderParams.myTransform = myTransformation;
		msg->myRenderParams.myTransformLastFrame = myLastFrame;
		msg->myRenderParams.aHighlightIntencity = myHighlightIntencity;

		if (aLight == nullptr)
		{
			msg->myRenderParams.myRenderToDepth = true;
			msg->myType = SRenderMessage::eRenderMessageType::eRenderModelDepth;
		}
		else
		{
			CU::Intersection::Sphere model;
			CU::Intersection::Sphere light;
			msg->myRenderParams.myNumLights = 0;


			for (unsigned short i = 0; i < aPointLightList.Size(); ++i)
			{
				if (aPointLightList[i].GetIsActive() == true)
				{
					light.myCenterPosition = aPointLightList[i].GetPosition();
					light.myRadius = aPointLightList[i].GetRange();
					light.myRadiusSquared = light.myRadius * light.myRadius;

					if (CU::Intersection::SphereVsSphere(model, light))
					{
						msg->myRenderParams.myPointLightList.Add(Lights::SPointLight());
						msg->myRenderParams.myPointLightList.GetLast().color = aPointLightList[i].GetColor();
						msg->myRenderParams.myPointLightList.GetLast().intensity = aPointLightList[i].GetInstensity();
						msg->myRenderParams.myPointLightList.GetLast().position = aPointLightList[i].GetPosition();
						msg->myRenderParams.myPointLightList.GetLast().range = aPointLightList[i].GetRange();
						msg->myRenderParams.myNumLights++;
					}
				}
			}
		}
	
		if (myHasAnimations != false)
		{
			msg->myRenderParams.aAnimationTime = myAnimationCounter;
			msg->myRenderParams.aAnimationState = myCurrentAnimation;
			msg->myRenderParams.aAnimationLooping = myAnimationLooping;
		}

		aRenderToCamera.AddRenderMessage(msg);
		myLastFrame = myTransformation;
	}
}

void CModelInstance::RenderDeferred()
{
	if (!ShouldRender())
	{
		return;
	}

	CModelManager* modelManager = MODELMGR;
	int modelRefCount = modelManager->GetModelRefCount(myModel);
	
	SDeferredRenderModelParams params;
	params.myTransform = myTransformation;
	params.myTransformLastFrame = myLastFrame;
	params.myHighlightColor = myHighlightColor;
	params.myHighlightIntensivity = myHighlightIntencity;
	params.myRenderToDepth = false;
	params.aHighlightIntencity = myHighlightIntencity;
	if (myHasAnimations == true)
	{
		params.aAnimationLooping = myAnimationLooping;
		params.aAnimationState = myCurrentAnimation;
		params.aNextAnimationState = myNextAnimation;
		params.aAnimationLerper = myAnimationLerpie;
		params.aAnimationTime = myAnimationCounter;
	}
	
	SRenderMessage* msg = nullptr;
	if (!myHasAnimations && modelRefCount > HIGH_ENUF)
	{
		SRenderModelInstancedMessage* instancedMsg = new SRenderModelInstancedMessage();
		instancedMsg->myModelID = myModel;
		instancedMsg->myRenderParams = params;
		msg = instancedMsg;
	}
	else
	{
		SRenderModelDeferredMessage* lonelyMsg = new SRenderModelDeferredMessage();
		lonelyMsg->myModelID = myModel;
		lonelyMsg->myRenderParams = params;
		msg = lonelyMsg;
	}

	RENDERER.AddRenderMessage(msg);
}

void CModelInstance::RenderDeferred(CRenderCamera & aRenderToCamera)
{
	if (!ShouldRender())
	{
		return;
	}

	CModelManager* modelManager = MODELMGR;
	int modelRefCount = modelManager->GetModelRefCount(myModel);

	SDeferredRenderModelParams params;
	params.myTransform = myTransformation;
	params.myTransformLastFrame = myLastFrame;
	params.myHighlightColor = myHighlightColor;
	params.myHighlightIntensivity = myHighlightIntencity;
	params.myRenderToDepth = aRenderToCamera.GetIsShadowCamera();
	params.aHighlightIntencity = myHighlightIntencity;
	if (myHasAnimations == true)
	{
		params.aAnimationLooping = myAnimationLooping;
		params.aAnimationState = myCurrentAnimation;
		params.aNextAnimationState = myNextAnimation;
		params.aAnimationLerper = myAnimationLerpie;
		params.aAnimationTime = myAnimationCounter;
	}

	SRenderMessage* msg = nullptr;
	if (!myHasAnimations && modelRefCount > HIGH_ENUF)
	{
		SRenderModelInstancedMessage* instancedMsg = new SRenderModelInstancedMessage();
		instancedMsg->myModelID = myModel;
		instancedMsg->myRenderParams = params;
		msg = instancedMsg;
	}
	else
	{
		SRenderModelDeferredMessage* lonelyMsg = new SRenderModelDeferredMessage();
		lonelyMsg->myModelID = myModel;
		lonelyMsg->myRenderParams = params;
		msg = lonelyMsg;
	}
	aRenderToCamera.AddRenderMessage(msg);
}

void CModelInstance::Update(const CU::Time aDeltaTime)
{
	myAnimationCounter += aDeltaTime.GetSeconds();
}

void CModelInstance::SetTransformation(const CU::Matrix44f& aTransformation)
{
	myTransformation = aTransformation;
}

void CModelInstance::SetPosition(CU::Vector3f aPosition)
{
	myTransformation.m41 = aPosition.x;
	myTransformation.m42 = aPosition.y;
	myTransformation.m43 = aPosition.z;
}

bool CModelInstance::ShouldRender()
{
	return  myModel != -1 && myIsVisible == true;
}

//in worldSpace
CU::Sphere CModelInstance::GetModelBoundingSphere()
{
	CU::Sphere sphere;

	if (myModel == -1)
		return sphere;

	sphere.myCenterPos = myTransformation.GetPosition();
	sphere.myRadius = MODELMGR->GetModel(myModel)->GetRadius();

	float maxScale = max(myTransformation.myForwardVector.Length2(), myTransformation.myRightVector.Length2());
	maxScale = max(maxScale, myTransformation.myUpVector.Length2());
	sphere.myRadius *= sqrt(maxScale);
	return sphere;
}

void CModelInstance::SetHighlight(const CU::Vector4f& aColor, float anIntensivity)
{
	myHighlightColor = aColor;
	myHighlightIntencity = anIntensivity;
}

float CModelInstance::GetAnimationDuration(const eAnimationState aAnimationState) const
{
	CModelManager* modelManager = MODELMGR;
	CModel* model = modelManager->GetModel(myModel);
	if (model)
	{
		return model->GetAnimationDuration(aAnimationState);
	}

	return 0.f;
}
void CModelInstance::SetAnimation(const eAnimationState aAnimationKey)
{
	myCurrentAnimation = aAnimationKey;
}

void CModelInstance::SetAnimationLerpie(const float aLerpValue)
{
	myAnimationLerpie = aLerpValue;
	CLAMP(myAnimationLerpie, 0.f, 1.f);
}

void CModelInstance::SetNextAnimation(const eAnimationState aAnimationKey)
{
	myNextAnimation = aAnimationKey;
}

void CModelInstance::SetAnimationLooping(const bool aValue)
{
	myAnimationLooping = aValue;
	//myAnimationCounter = 0.0f;
}

void CModelInstance::ResetAnimation()
{
	myAnimationCounter = 0.0f;
}

eAnimationState CModelInstance::GetAnimationState() const
{
	return myCurrentAnimation;
}

float CModelInstance::GetAnimationCounter() const
{
	return myAnimationCounter;
}

bool CModelInstance::GetAnimationStates(CU::GrowingArray<eAnimationState>& aAnimationStatesOut) const
{
	CModelManager* modelManager = MODELMGR;
	CModel* model = modelManager->GetModel(myModel);
	if (model)
	{
		model->GetAnimationStates(aAnimationStatesOut);
		return true;
	}

	return false;
}

const std::string& CModelInstance::GetFilePath() const
{
	CModelManager* modelManager = MODELMGR;
	return modelManager->GetFilePath(myModel);
}

void CModelInstance::SetHighlightIntencity(const float aHighlightIntencity)
{
	myHighlightIntencity = aHighlightIntencity;
}
