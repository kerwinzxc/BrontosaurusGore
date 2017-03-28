#include "stdafx.h"
#include "ProjectileComponent.h"
#include "ProjectileData.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/CreateExplosionMessage.h"

CProjectileComponent::CProjectileComponent()
{
	myIsActive = false; 
	myData = nullptr;
}


CProjectileComponent::~CProjectileComponent()
{
}

void CProjectileComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnCollisionEnter:
	{
		if(myData->shouldRayCast == false)
		{
			SComponentMessageData damageData;
			damageData.myInt = myData->damage;
			aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
		}
		if(myData && myData->shouldExplodeOnImpact == true)
		{
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CCreateExplosionMessage(GetParent()->GetWorldPosition(), myData->explosionData));
		}
		Deactivate();
		break;
	}
	default:
		break;
	}
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
	GetParent()->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
}

void CProjectileComponent::Update(float aDeltaTime)
{
	if(myIsActive == true)
	{
		GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myData->movementSpeed * aDeltaTime));

		
		NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());

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
	DL_PRINT("Deactivate");
	myIsActive = false;
	SComponentMessageData visibilityData;
	visibilityData.myBool = false;
	GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
	GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
}