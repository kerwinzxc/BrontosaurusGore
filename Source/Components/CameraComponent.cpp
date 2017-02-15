#include "stdafx.h"
#include "CameraComponent.h"
#include "GameObject.h"
#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/Camera.h"

void CCameraComponent::InitOffsetPosition()
{
	myOffsetPosition = myCamera->GetPosition(); 
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
		if (aMessageData.myComponentTypeAdded != eComponentType::eCamera) break;
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
	{
		if (myUnlocked == false)
		{
			CU::Vector3f parentPosition = GetParent()->GetWorldPosition();
			parentPosition.y = 0.f;
			myCamera->SetPosition(myOffsetPosition + parentPosition);
		}
	}
		break;
	}
}

void CCameraComponent::Destroy()
{
	DL_ASSERT("Not yet implemented");
}

CCameraComponent::CCameraComponent()
{
	myType = eComponentType::eCamera;
	myUnlocked = false;
}

CCameraComponent::~CCameraComponent()
{
}
