#include "stdafx.h"
#include "ExplosionComponent.h"
#include "ExplosionData.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/DeactivateExplosionMessage.h"
#include "PollingStation.h"

CExplosionComponent::CExplosionComponent()
{
	myDuration = 1.0f;
	myCollidedWithGameObjects.Init(100);
	myIsActive = false;
}


CExplosionComponent::~CExplosionComponent()
{
}

void CExplosionComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
	{
		if(myCollidedWithGameObjects.Find(aMessageData.myComponent->GetParent()) == myCollidedWithGameObjects.FoundNone)
		{
			if(myData->isPlayerFriendly == true && CPollingStation::GetInstance()->CheckIfIsPlayerObject(aMessageData.myComponent->GetParent()) == true)
			{
				break;
			}
			SComponentMessageData damageData;
			damageData.myInt = myData->damage;
			aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
			myCollidedWithGameObjects.Add(aMessageData.myComponent->GetParent());
		}
		break;
	}
	case eComponentMessageType::eActivateExplosion:
	{
		myIsActive = true;
		myElapsedLivingTime = 0.0f;
		myData = aMessageData.myExplosionData;
		myCollidedWithGameObjects.RemoveAll();
		break;
	}
	default:
		break;
	}
}
void CExplosionComponent::Destroy()
{

}

void CExplosionComponent::Update(float aDeltaTime)
{
	if(myIsActive == true)
	{
		myElapsedLivingTime += aDeltaTime;
		if (myElapsedLivingTime > myDuration)
		{
			GetParent()->NotifyComponents(eComponentMessageType::eDied, SComponentMessageData());
			SComponentMessageData visibilityData;
			visibilityData.myBool = false;
			GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CDeactivateExplosionMessage(GetParent()));
			myCollidedWithGameObjects.RemoveAll();
			myElapsedLivingTime = 0.0f;
			myIsActive = false;
			GetParent()->SetWorldPosition(CU::Vector3f(0.0f, -10000, 0.0f));
			GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		}
	}
}