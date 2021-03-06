#include "stdafx.h"
#include "CameraComponent.h"
#include "../CommonUtilities/Camera.h"
#include "../Audio/AudioInterface.h"

CCameraComponent::CCameraComponent()
	: myCamera(nullptr)
	, myUnlocked(false)
{
	myType = eComponentType::eCamera;
}

CCameraComponent::~CCameraComponent()
{
}

void CCameraComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	if (myCamera == nullptr)
	{
		return;
	}

	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponentTypeAdded == eComponentType::eCamera)
		{
			GetParent()->SetName("Camera");
		}
		break;
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
		if (myUnlocked == false)
		{
			const CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
			myCamera->SetTransformation(transformation);
			Audio::CAudioInterface::GetInstance()->SetListenerPosition(transformation);
		}
		break;
	case eComponentMessageType::ePitch:
		Pitch(aMessageData.myFloat);
		break;
	}
}

bool CCameraComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eHasCameraComponent:
		aQuestionData.myBool = true;
		return true;
	case eComponentQuestionType::eGetCameraLookat:
		return GetLookat(aQuestionData.myVector3f);
	case eComponentQuestionType::eGetCameraPosition:
		return GetPosition(aQuestionData.myVector3f);
	case eComponentQuestionType::eGetCameraObject:
	{
		aQuestionData.myGameObject = GetParent();
		return true;
		break;
	}
	}

	return false;
}

void CCameraComponent::SetCamera(CU::Camera& aCamera)
{
	myCamera = &aCamera;
}

void CCameraComponent::Pitch(const float aPitch)
{
	const float PitchCap = 0.99f;

	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	float lookAtHeight = parentTransform.myForwardVector.y;
	if (lookAtHeight < PitchCap && lookAtHeight > -PitchCap)
	{
		parentTransform.Rotate(aPitch, CU::Axees::X);
	}
	else if (lookAtHeight > PitchCap && aPitch > 0.f)
	{
		parentTransform.Rotate(aPitch, CU::Axees::X);
	}
	else if (lookAtHeight < -PitchCap && aPitch < 0.f)
	{
		parentTransform.Rotate(aPitch, CU::Axees::X);
	}
}

bool CCameraComponent::GetLookat(CU::Vector3f& aLookat)
{
	if (GetParent())
	{
		aLookat = GetParent()->GetToWorldTransform().myForwardVector.GetNormalized();
		return true;
	}

	return false;
}

bool CCameraComponent::GetPosition(CU::Vector3f& aPosition)
{
	if (GetParent())
	{
		aPosition = GetParent()->GetToWorldTransform().myPosition;
		return true;
	}

	return false;
}
