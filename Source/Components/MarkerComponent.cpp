#include "stdafx.h"
#include "MarkerComponent.h"
#include "CheckpointComponentManager.h"

CMarkerComponent::CMarkerComponent()
{
	myIsVisible = true;
}


CMarkerComponent::~CMarkerComponent()
{
}

void CMarkerComponent::Update(float aDeltaTime)
{
	CU::Vector3f lookAtPosition = CCheckpointComponentManager::GetInstance()->GetNextCheckpointPosition();
	if(GetParent() == nullptr)
	{
		return;
	}
	if(lookAtPosition != CU::Vector3f::Zero)
	{
		if(myIsVisible == false)
		{
			SComponentMessageData visibilityData;
			visibilityData.myBool = true;
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, visibilityData);
		}
		
		CU::Matrix44f globalMatrix = GetParent()->GetToWorldTransform();
		globalMatrix.LookAt(lookAtPosition);
		GetParent()->SetWorldTransformation(globalMatrix);
		GetParent()->GetLocalTransform().Rotate(PI, CU::Axees::Y);
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	}
	else
	{
		if (myIsVisible == true)
		{
			SComponentMessageData visibilityData;
			visibilityData.myBool = false;
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, visibilityData);
		}
	}
}

void CMarkerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eSetVisibility:
	{
		myIsVisible = aMessageData.myBool;
		break;
	}
	case eComponentMessageType::eDied:
	{
		myIsVisible = false;
		break;
	}
	default:
		break;
	}
}