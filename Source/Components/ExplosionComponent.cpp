#include "stdafx.h"
#include "ExplosionComponent.h"
#include "ExplosionData.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/DeactivateExplosionMessage.h"

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
		}
	}
}