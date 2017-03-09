#include "stdafx.h"
#include "ProjectileComponent.h"
#include "ProjectileData.h"


CProjectileComponent::CProjectileComponent()
{
	myIsActive = false;
}


CProjectileComponent::~CProjectileComponent()
{
}

void CProjectileComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
}

void CProjectileComponent::Destroy()
{
}

void CProjectileComponent::Activate(SProjectileData* someData, const CU::Vector3f& aDirection, const CU::Vector3f& aPosition)
{
	myData = someData;
	myDirection = aDirection;
	myStartPosition = aPosition;
	GetParent()->GetLocalTransform().SetPosition(aPosition);
	GetParent()->GetLocalTransform().LookAt(GetParent()->GetLocalTransform().GetPosition() + aDirection);
	myIsActive = true;
	SComponentMessageData visibilityData;
	visibilityData.myBool = true;
	GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
}

void CProjectileComponent::Update(float aDeltaTime)
{
	if(myIsActive == true)
	{
		GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myData->movementSpeed * aDeltaTime));
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());

		float distance2 = CU::Vector3f(myStartPosition - GetParent()->GetWorldPosition()).Length2();
		float inActivationRange = myData->maximumTravelRange;
		if(distance2 > inActivationRange * inActivationRange)
		{
			Deactivate();
		}
	}
}

void CProjectileComponent::Deactivate()
{
	myIsActive = false;
	SComponentMessageData visibilityData;
	visibilityData.myBool = false;
	GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
}