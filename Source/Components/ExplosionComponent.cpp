#include "stdafx.h"
#include "ExplosionComponent.h"
#include "ExplosionData.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/DeactivateExplosionMessage.h"

CExplosionComponent::CExplosionComponent()
{
	myDuration = 10.0f;
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
		SComponentMessageData damageData; 
		damageData.myInt = myData->damage;
		aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
	}

		break;
	default:
		break;
	}
}
void CExplosionComponent::Destroy()
{

}

void CExplosionComponent::Update(float aDeltaTime)
{
	myElapsedLivingTime += aDeltaTime;
	if(myElapsedLivingTime > myDuration)
	{
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CDeactivateExplosionMessage(GetParent()));
	}
}