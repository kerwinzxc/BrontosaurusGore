#include "stdafx.h"
#include "HealthComponent.h"


HealthComponent::HealthComponent()
{
	myMaxHeath = 0;
	myCurrentHealth = 0;
	myIsAlive = true;
}


HealthComponent::~HealthComponent()
{
}

void HealthComponent::SetMaxHealth(const healthPoint aHealthPointValue)
{
	myMaxHeath = aHealthPointValue;
}

void HealthComponent::SetHealth(const healthPoint aHealthPointValue)
{
	myCurrentHealth = aHealthPointValue;
}

void HealthComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTakeDamage:
		if(myIsAlive == true)
		{
			TakeDamage(aMessageData.myInt);
		}
		break;
	case eComponentMessageType::eHeal:
		if (myIsAlive == true)
		{
			Heal(aMessageData.myInt);
		}
		break;
	default:
		break;
	}
}
void HealthComponent::Destroy()
{

}

void HealthComponent::TakeDamage(const healthPoint aDamage)
{
	if(aDamage < 0)
	{
		Heal(-aDamage);
		return;
	}

	GetParent()->NotifyComponents(eComponentMessageType::eTookDamage, SComponentMessageData());
	if(myCurrentHealth - aDamage <= 0)
	{
		myCurrentHealth = 0;
		GetParent()->NotifyComponents(eComponentMessageType::eDied, SComponentMessageData());


		SComponentMessageData data;
		data.myBool = false;
		GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data);
	}
	else
	{
		myCurrentHealth -= aDamage;
	}
}
void HealthComponent::Heal(const healthPoint aHealAmount)
{
	if (aHealAmount < 0)
	{
		TakeDamage(-aHealAmount);
		return;
	}

	if (myCurrentHealth + aHealAmount > myMaxHeath)
	{
		myCurrentHealth = myMaxHeath;
	}
	else
	{
		myCurrentHealth += aHealAmount;
	}
}