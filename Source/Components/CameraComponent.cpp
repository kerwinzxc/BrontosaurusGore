#include "stdafx.h"
#include "CameraComponent.h"
#include "GameObject.h"
#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/Camera.h"

CCameraComponent::CCameraComponent()
{
	myType = eComponentType::eCamera;
	myUnlocked = false;
}

CCameraComponent::~CCameraComponent()
{
}

void CCameraComponent::InitOffsetPosition()
{
	//myOffsetPosition = myCamera->GetPosition(); 
}

void CCameraComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	if (myCamera == nullptr)
	{
		return;
	}

	switch (aMessageType)
	{
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
		if (myUnlocked == false)
		{
			//CU::Vector3f parentPosition = GetParent()->GetWorldPosition();
			//parentPosition.y = 0.f;
			//myCamera->SetPosition(myOffsetPosition + parentPosition);
			myCamera->SetTransformation(GetParent()->GetToWorldTransform());
		}
		break;
	case eComponentMessageType::ePitch:
		Pitch(aMessageData.myFloat);
		break;
	case eComponentMessageType::eSetDirectionForShooting:
		CU::Matrix44f localMatrix = GetParent()->GetToWorldTransform();
		localMatrix.Move(CU::Vector3f(0.0f, 0.0f, 10.0f));
		CU::Vector3f direction = localMatrix.GetPosition() - GetParent()->GetWorldPosition();
		direction.Normalize();
		SComponentMessageData shootData;
		shootData.myVector3f = direction;
		GetParent()->NotifyComponents(eComponentMessageType::eTryToShoot, shootData);
		break;
	}
}

void CCameraComponent::Pitch(const float aPitch)
{
	const float PitchCap = 0.999f;

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
