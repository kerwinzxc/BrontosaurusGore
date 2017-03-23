#include "stdafx.h"
#include "DamageOnCollisionComponent.h"


CDamageOnCollisionComponent::CDamageOnCollisionComponent()
{
	myDamage = 0;
	myDamageCooldown = 0.0f;
	myElapsedCooldownTime = 0.0f;
}


CDamageOnCollisionComponent::~CDamageOnCollisionComponent()
{
}

void CDamageOnCollisionComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnCollisionEnter:
		if(myElapsedCooldownTime > myDamageCooldown)
		{
			SComponentMessageData damageData;
			aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
			myElapsedCooldownTime = 0.0f;
		}
		break;
	case eComponentMessageType::eOnTriggerEnter:
		if (myElapsedCooldownTime > myDamageCooldown)
		{
			SComponentMessageData damageData;
			aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
			myElapsedCooldownTime = 0.0f;
		}
		break;
	default:
		break;
	}
}

void CDamageOnCollisionComponent::Update(float aDeltaTime)
{
	myElapsedCooldownTime += aDeltaTime;
}